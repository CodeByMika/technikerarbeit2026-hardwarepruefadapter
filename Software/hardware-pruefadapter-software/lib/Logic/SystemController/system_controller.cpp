/**
 * @file system_controller.cpp
 * @brief Implementierung der Systemlogik.
 *
 * Die Implementierung folgt dem EVA-Prinzip (Eingabe-Verarbeitung-Ausgabe).
 * Die Hardware-Treiber werden angewendet, ohne deren interne Details (Register)
 * zu kennen.
 */

#include "system_controller.h"

namespace hardware_pruefadapter {
namespace core {

//==================Konstruktor================
SystemController::SystemController(const SystemContext& context)
    : ctx_(context) {
  // Initialisierungsliste wird genutzt, Konstruktor-Rumpf bleibt leer.
}

//==================Initialize====================
types::ErrorCode SystemController::Initialize() {
  logic::DiagnosticsService::LogSystemInfo();
  health_ = logic::HardwareBootstrapper::Boot(ctx_, *ctx_.logic.error_manager);

  ctx_.logic.serial_streamer->SetHealthMask(health_.serial_init_success);
  ctx_.logic.led_manager->SetHealthMask(health_.led_init_success);

  ctx_.logic.led_manager->TurnAllLedsOff();

  return health_.is_healthy ? types::ErrorCode::kSuccess
                            : types::ErrorCode::kErrorInitWarning;
}

void SystemController::RunSystemLogic() {
  ctx_.logic.error_manager->ClearShadowErrors();
  ctx_.logic.process_imager->ClearSystemChangeFlags();

  InputSequence();
  ProcessSequence();
  OutputSequence();

  ctx_.logic.error_manager->ResolveErrors();
}

void SystemController::InputSequence() {
  ReadAdcInputs();
  ctx_.logic.serial_streamer->ProcessReadRx();
  FetchMailboxTargets();
}

void SystemController::ProcessSequence() {
  ctx_.logic.process_imager->SyncUpdateToActive();
  CheckOvervoltageSafety();
  CheckAndRegulateAnalogOut();
  CheckDigitalOutSafety();
  EvaluateSystemErrorState();
}

void SystemController::OutputSequence() {
  UpdateDacOutputs();
  UpdateDigitalOutputs();

  ctx_.logic.led_manager->UpdateHardwareLeds();
  ctx_.logic.led_manager->UpdateSystemLed();

  WriteRxDataFromMailbox();
  SetSerialConfigFromMailbox();
}

//==============Input Funktionen==============

void SystemController::ReadAdcInputs() {
  types::ErrorCode status = types::ErrorCode::kSuccess;
  std::int16_t raw_value = 0;

  for (std::uint8_t j = 0; j < config::kAdcCount; j++) {
    if (ctx_.converter.adc[j] != nullptr &&
        (health_.adc_init_success & (1 << j)) != 0 &&
        ctx_.converter.adc[j]->GetOnlineStatus() != 0) {
      for (std::uint8_t i = 0; i < config::kNumChannels; i++) {
        std::uint8_t hw_channel = i;

        // Index i: 0=VOUTA, 1=VOUTB, 2=VOUTC, 3=VOUTD
        if (j == types::IO::kDigitalOut) {
          const std::uint8_t kAnalogOutReadbackMap[4] = {2, 1, 3, 0};
          hw_channel = kAnalogOutReadbackMap[i];
        }
        status = ctx_.converter.adc[j]->GetValue(hw_channel, &raw_value);

        // LOG_INFO("ADC %d Ch %d raw value: %d", j + 1, i + 1, raw_value);

        if (status == types::ErrorCode::kSuccess) {
          std::uint16_t current_mv =
              ctx_.converter.adc[j]->ConvertToVoltage(raw_value);

          if (current_mv < config::kNoiseGateMv) {
            current_mv = 0;
          }

          // LOG_INFO("ADC %d Ch %d voltage: %d mV", j + 1, i + 1,
          // current_mv);

          adc_filters_[j][i].AddValue(current_mv);
        } else {
          adc_filters_[j][i].Reset();
          if (j == types::IO::kAnalogIn || j == types::IO::kDigitalIn) {
            ctx_.logic.process_imager->SetUpdateVoltage(types::IO::Group(j), i,
                                                        0);
          } else {
            ctx_.logic.process_imager->SetReadbackVoltage(types::IO::Group(j),
                                                          i, 0);
          }
          ctx_.logic.error_manager->AddCycleError(
              "ADC " + std::to_string(j + 1) + "Ch " + std::to_string(i + 1) +
                  " [GetValue]",
              status);
          break;
        }

        std::uint16_t averaged_mv = adc_filters_[j][i].GetAverage();

        // LOG_INFO("ADC %d Ch %d averaged voltage: %d mV", j + 1, i +
        // 1, averaged_mv);

        if (j == types::IO::kAnalogIn || j == types::IO::kDigitalIn) {
          ctx_.logic.process_imager->SetUpdateVoltage(types::IO::Group(j), i,
                                                      averaged_mv);
          // LOG_INFO("Set Update Voltage for ADC %d Ch %d: %d mV", j + 1, i +
          // 1,
          //          averaged_mv);
        } else {
          ctx_.logic.process_imager->SetReadbackVoltage(types::IO::Group(j), i,
                                                        averaged_mv);
          // LOG_INFO("Set Readback Voltage for ADC %d Ch %d: %d mV", j + 1, i +
          // 1,
          //          averaged_mv);
        }
      }
    } else {
      ctx_.logic.error_manager->AddCycleError(
          "ADC " + std::to_string(j + 1),
          types::ErrorCode::kErrorDeviceNotFound);
    }
  }
}

void SystemController::FetchMailboxTargets() {
  logic::IoTargetsSnapshot snapshot = ctx_.logic.mailbox->ConsumeIoTargets();
  for (std::uint8_t i = 0; i < config::kNumChannels; i++) {
    if (snapshot.new_analog[i]) {
      ctx_.logic.process_imager->SetUpdateVoltage(types::IO::kAnalogOut, i,
                                                  snapshot.analog_mv[i]);
    }
    if (snapshot.new_digital[i]) {
      ctx_.logic.process_imager->SetUpdateVoltage(
          types::IO::kDigitalOut, i, snapshot.digital_state[i] ? 1 : 0);
    }
  }

  logic::ReferenceConfigSnapshot ref_snap =
      ctx_.logic.mailbox->ConsumeReferenceConfig();
  if (ref_snap.has_config) {
    for (std::uint8_t direction = 0; direction < 2; direction++) {
      types::IO::Direction dir = static_cast<types::IO::Direction>(direction);
      for (std::uint8_t channel = 0; channel < config::kNumChannels;
           channel++) {
        if (ref_snap.new_reference[dir][channel]) {
          if (dir == types::IO::kOutput) {
            std::uint16_t old_ref =
                ctx_.logic.process_imager->GetDigitalReference(dir, channel);
            std::uint16_t new_ref = ref_snap.reference_mv[dir][channel];

            bool old_is_hv = (old_ref > 5000);
            bool new_is_hv = (new_ref > 5000);

            if (old_is_hv != new_is_hv) {
              ctx_.logic.process_imager->SetUpdateVoltage(
                  types::IO::kDigitalOut, channel, 0);
              LOG_INFO(
                  "[SICHERHEIT] DigitalOut %d: Gruppe gewechselt. Ausgang "
                  "abgeschaltet.",
                  channel + 1);
            }
          }

          ctx_.logic.process_imager->SetDigitalReference(
              dir, channel, ref_snap.reference_mv[dir][channel]);
          hardware_pruefadapter::platform::SetSimulatedReferenceVoltage(
              dir, channel, ref_snap.reference_mv[dir][channel]);
        }
      }
    }
  }
}

//==============Process Funktionen==============

void SystemController::CheckOvervoltageSafety() {
  for (std::uint8_t i = 0; i < config::kNumChannels; i++) {
    std::uint16_t a_out_mv =
        ctx_.logic.process_imager->GetReadbackVoltage(types::IO::kAnalogOut, i);
    if (a_out_mv >= config::kMaxSystemVoltageMv) {
      ctx_.logic.process_imager->SetUpdateVoltage(types::IO::kAnalogOut, i, 0);
      ctx_.logic.error_manager->AddCycleError(
          "AnalogOut " + std::to_string(i + 1) + " [Notabschaltung]",
          types::ErrorCode::kErrorInvalidValue);
    }

    std::uint16_t d_out_mv = ctx_.logic.process_imager->GetReadbackVoltage(
        types::IO::kDigitalOut, i);
    if (d_out_mv >= config::kMaxSystemVoltageMv) {
      ctx_.logic.process_imager->SetUpdateVoltage(types::IO::kDigitalOut, i, 0);
      ctx_.logic.error_manager->AddCycleError(
          "DigitalOut " + std::to_string(i + 1) + " [Notabschaltung]",
          types::ErrorCode::kErrorInvalidValue);
    }
  }
}

void SystemController::CheckAndRegulateAnalogOut() {
  for (std::uint8_t i = 0; i < config::kNumChannels; i++) {
    std::uint16_t target_mv =
        ctx_.logic.process_imager->GetVoltage(types::IO::kAnalogOut, i);
    std::uint32_t mask =
        (1UL << (i + (types::IO::kAnalogOut * types::IO::kIoGroupCount)));

    if (ctx_.logic.process_imager->HasChangeFlagMask(mask)) {
      analog_out_grace_timers_[i] = config::kOutputGracePeriodCycles;
    }

    if (target_mv == 0) {
      analog_out_grace_timers_[i] = 0;
      continue;
    }

    std::uint16_t actual_mv =
        ctx_.logic.process_imager->GetReadbackVoltage(types::IO::kAnalogOut, i);
    std::int32_t diff = static_cast<std::int32_t>(target_mv) -
                        static_cast<std::int32_t>(actual_mv);
    std::uint32_t abs_diff = std::abs(diff);

    if (analog_out_grace_timers_[i] > 0) {
      analog_out_grace_timers_[i]--;
      // LOG_INFO("AnalogOut %d in grace period. Target: %d mV, Actual: %d mV,
      // Diff:%d mV, Grace Cycles Left: %d", i + 1, target_mv, actual_mv,
      // diff, analog_out_grace_timers_[i]);
      continue;
    } else {
      // Safety Check nach Ablauf der Schonfrist ausführen
      std::uint32_t max_allowed_diff =
          (target_mv * config::kAnalogErrorThresholdPercent) / 100;
      if (max_allowed_diff < config::kAnalogControlToleranceMv + 500) {
        max_allowed_diff = config::kAnalogControlToleranceMv + 500;
      }
      if (abs_diff > max_allowed_diff && actual_mv < target_mv) {
        ctx_.logic.error_manager->AddCycleError(
            "AnalogOut " + std::to_string(i + 1) + " [Overload]",
            types::ErrorCode::kErrorInvalidValue);
        ctx_.logic.process_imager->SetVoltage(types::IO::kAnalogOut, i, 0);
        ctx_.logic.process_imager->SetUpdateVoltage(types::IO::kAnalogOut, i,
                                                    0);
        ctx_.logic.process_imager->SetAdjustedDacVoltage(i, 0);
        ctx_.logic.process_imager->AddSystemChangeFlagMask(mask);
        continue;  // Nach einem Fehler regeln wir nicht mehr!
      }
    }

    if (abs_diff > config::kAnalogControlToleranceMv) {
      std::int32_t step = diff / 2;
      if (step == 0) step = (diff > 0) ? 1 : -1;

      std::int32_t new_dac_val =
          static_cast<std::int32_t>(
              ctx_.logic.process_imager->GetAdjustedDacVoltage(i)) +
          step;
      if (new_dac_val < 0) new_dac_val = 0;
      if (new_dac_val > config::kMaxSystemVoltageMv)
        new_dac_val = config::kMaxSystemVoltageMv;

      if (new_dac_val != ctx_.logic.process_imager->GetAdjustedDacVoltage(i)) {
        ctx_.logic.process_imager->SetAdjustedDacVoltage(
            i, static_cast<std::uint16_t>(new_dac_val));
        ctx_.logic.process_imager->AddSystemChangeFlagMask(mask);
      }
    }
  }
}

void SystemController::CheckDigitalOutSafety() {
  for (std::uint8_t i = 0; i < config::kNumChannels; i++) {
    std::uint16_t target_state =
        ctx_.logic.process_imager->GetVoltage(types::IO::kDigitalOut, i);

    std::uint32_t mask =
        (1UL << (i + (types::IO::kDigitalOut * types::IO::kIoGroupCount)));

    if (ctx_.logic.process_imager->HasChangeFlagMask(mask)) {
      digital_out_grace_timers_[i] = config::kOutputGracePeriodCycles;
    }

    if (target_state == 0) {
      digital_out_grace_timers_[i] = 0;
      continue;
    }

    if (digital_out_grace_timers_[i] > 0) {
      digital_out_grace_timers_[i]--;
      continue;
    }

    std::uint16_t actual_mv = ctx_.logic.process_imager->GetReadbackVoltage(
        types::IO::kDigitalOut, i);
    std::uint16_t ref_mv =
        ctx_.logic.process_imager->GetDigitalReference(types::IO::kOutput, i);

    std::uint16_t min_expected_mv =
        (ref_mv * config::kLogicHighThresholdPercent) / 100;
    if (actual_mv < min_expected_mv) {
      ctx_.logic.error_manager->AddCycleError(
          "DigitalOut " + std::to_string(i + 1) + " [Safety]",
          types::ErrorCode::kErrorInvalidValue);
      ctx_.logic.process_imager->SetVoltage(types::IO::kDigitalOut, i, 0);
      ctx_.logic.process_imager->SetUpdateVoltage(types::IO::kDigitalOut, i, 0);
      ctx_.logic.process_imager->AddSystemChangeFlagMask(
          1UL << (i + (types::IO::kDigitalOut * types::IO::kIoGroupCount)));
    }
  }
}

void SystemController::EvaluateSystemErrorState() {
  std::int8_t current_system_error =
      (ctx_.logic.error_manager->GetErrorCount() > 0) ? 1 : 0;
  if (current_system_error !=
      ctx_.logic.process_imager->GetLastSystemErrorState()) {
    ctx_.logic.process_imager->AddSystemChangeFlagMask(types::kLedSystem);
    ctx_.logic.process_imager->SetLastSystemErrorState(current_system_error);
  }
}

//==============Output Funktionen==============

void SystemController::UpdateDacOutputs() {
  for (std::uint8_t j = 0; j < config::kDacCount; j++) {
    if (ctx_.converter.dac[j] != nullptr &&
        (health_.dac_init_success & types::kChip1Init) != 0) {
      for (uint8_t i = 0; i < config::kNumChannels; i++) {
        std::uint32_t mask =
            (1UL << (i + (types::IO::kAnalogOut * types::IO::kIoGroupCount)));

        std::uint8_t hw_channel = 3 - i;  // Mappt: 0->3, 1->2, 2->1, 3->0

        if (ctx_.logic.process_imager->HasChangeFlagMask(mask)) {
          // Hardware-Verstärkung (Gain = 5) herausrechnen
          // Der System-Wert (0-24000 mV) wird in den DAC-Wert (0-4800 mV)
          // skaliert.
          std::uint16_t system_target_mv =
              ctx_.logic.process_imager->GetAdjustedDacVoltage(i);
          std::uint16_t dac_pin_target_mv = system_target_mv / 5;

          types::ErrorCode status =
              ctx_.converter.dac[j]->SetValue(hw_channel, dac_pin_target_mv);

          if (status != types::ErrorCode::kSuccess) {
            ctx_.logic.error_manager->AddCycleError(
                "DAC Ch " + std::to_string(i + 1) + " [SetValue]", status);
            ctx_.logic.process_imager->SetVoltage(types::IO::kAnalogOut, i, 0);
            ctx_.logic.process_imager->SetUpdateVoltage(types::IO::kAnalogOut,
                                                        i, 0);
            ctx_.logic.process_imager->SetAdjustedDacVoltage(i, 0);

            std::uint16_t ref_mv =
                ctx_.logic.process_imager->GetDigitalReference(
                    types::IO::kOutput, i);
            hardware_pruefadapter::platform::SetDigitalOutPin(i, false, ref_mv);

            LOG_WARNING(
                "[FAIL-SAFE] DAC defekt. GPIO Kanal %d hart abgeschaltet!",
                i + 1);
          } else {
            LOG_INFO("[DEBUG] Schreibe DAC Kanal %d auf %.2f V", i + 1,
                     dac_pin_target_mv / 1000.0f);
          }
        }
      }
    }
  }
}

void SystemController::UpdateDigitalOutputs() {
  for (uint8_t i = 0; i < config::kNumChannels; i++) {
    std::uint32_t mask =
        (1UL << (i + (types::IO::kDigitalOut * types::IO::kIoGroupCount)));

    if (ctx_.logic.process_imager->HasChangeFlagMask(mask)) {
      bool state = (ctx_.logic.process_imager->GetVoltage(
                        types::IO::kDigitalOut, i) > 0);

      std::uint16_t ref_mv =
          ctx_.logic.process_imager->GetDigitalReference(types::IO::kOutput, i);
      hardware_pruefadapter::platform::SetDigitalOutPin(i, state, ref_mv);

      LOG_INFO("[DEBUG] Digital Out Kanal %d auf %s gesetzt", i + 1,
               state ? "HIGH" : "LOW");
    }
  }
}

void SystemController::WriteRxDataFromMailbox() {
  logic::SerialTxSnapshot snapshot = ctx_.logic.mailbox->ConsumeSerialTx();

  if (snapshot.has_data) {
    for (int i = 0; i < config::kSerialDriverCount; i++) {
      if (!snapshot.payloads[i].empty()) {
        ctx_.logic.serial_streamer->WriteTx(
            static_cast<types::Serial::Interface>(i), snapshot.payloads[i]);
      }
    }
  }
}

void SystemController::SetSerialConfigFromMailbox() {
  logic::SerialConfigSnapshot snapshot =
      ctx_.logic.mailbox->ConsumeSerialConfig();

  if (snapshot.has_config) {
    for (int i = 0; i < config::kSerialDriverCount; i++) {
      if (snapshot.new_baudrate[i]) {
        ctx_.logic.serial_streamer->SetBaudrate(
            static_cast<types::Serial::Interface>(i), snapshot.baudrates[i]);
      }
    }
  }
}

}  // namespace core
}  // namespace hardware_pruefadapter