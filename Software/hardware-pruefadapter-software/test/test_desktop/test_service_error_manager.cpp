#include <gtest/gtest.h>

#include "../include/api_types.h"
#include "../lib/Logic/Services/error_manager.h"

namespace hardware_pruefadapter {
namespace test {

// -------------------------------------------------------------------
// TEST 1: Fehler werden aus dem Schattenspeicher korrekt übernommen
// -------------------------------------------------------------------
TEST(ErrorManagerTest, LogsNewErrorCorrectly) {
  logic::ErrorManager em;

  em.ClearShadowErrors();
  em.AddCycleError("ADC 1", types::ErrorCode::kErrorDeviceNotFound);
  em.ResolveErrors();

  EXPECT_EQ(em.GetErrorCount(), 1);

  types::SystemStatusDto dto;
  em.FillErrorStatus(dto);
  EXPECT_TRUE(dto.errors[0].find("ADC 1") != std::string::npos);
}

// -------------------------------------------------------------------
// TEST 2: Das System heilt sich selbst (Auto-Recovery), wenn der Fehler weg ist
// -------------------------------------------------------------------
TEST(ErrorManagerTest, AutoRecoversFromResolvedError) {
  logic::ErrorManager em;

  em.ClearShadowErrors();
  em.AddCycleError("I2C Bus 2", types::ErrorCode::kErrorI2cCommunication);
  em.ResolveErrors();
  EXPECT_EQ(em.GetErrorCount(), 1);

  // Nächster Zyklus: Fehler wird NICHT mehr gemeldet
  em.ClearShadowErrors();
  em.ResolveErrors();

  EXPECT_EQ(em.GetErrorCount(), 0);
}

// -------------------------------------------------------------------
// TEST 3: Identische Fehler im selben Zyklus werden gebündelt (Spam-Schutz)
// -------------------------------------------------------------------
TEST(ErrorManagerTest, IgnoresDuplicateErrorsInSameCycle) {
  logic::ErrorManager em;
  em.ClearShadowErrors();

  em.AddCycleError("I2C Bus 2", types::ErrorCode::kErrorI2cCommunication);
  em.AddCycleError("I2C Bus 2", types::ErrorCode::kErrorI2cCommunication);

  em.ResolveErrors();
  EXPECT_EQ(em.GetErrorCount(), 1);
}

// -------------------------------------------------------------------
// TEST 4: Puffer-Überlauf bei massivem Hardwareausfall verhindern
// -------------------------------------------------------------------
TEST(ErrorManagerTest, PreventsBufferOverflowOnMassiveFailures) {
  logic::ErrorManager em;
  em.ClearShadowErrors();

  // Wir feuern mehr Fehler als der Puffer (config::kMaxCycleErrors) fassen kann
  for (int i = 0; i < (config::kMaxCycleErrors + 10); i++) {
    em.AddCycleError("Test " + std::to_string(i),
                     types::ErrorCode::kErrorInvalidValue);
  }

  em.ResolveErrors();

  // Der Puffer darf nicht überlaufen, das System stoppt bei der Maximalanzahl
  EXPECT_EQ(em.GetErrorCount(), config::kMaxCycleErrors);
}

}  // namespace test
}  // namespace hardware_pruefadapter