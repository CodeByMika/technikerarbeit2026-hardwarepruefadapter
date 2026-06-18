/**
 * @file api_controller.h
 * @brief Controller für REST-API Endpunkte.
 *
 * Orchestriert die Anfragen zwischen WebServer und SystemController.
 *
 * @author Techniker_Team_2025_26
 * @date 2026-01-18
 * @version 1.0.0
 * @ingroup Logic
 */
#ifndef HARDWARE_PRUEFADAPTER_LOGIC_API_CONTROLLER_H_
#define HARDWARE_PRUEFADAPTER_LOGIC_API_CONTROLLER_H_

#include <cstdint>
#include <string>

#include "../../Interfaces/web_server_interface.h"
#include "../Services/diagnostics_service.h"
#include "../Services/logger_service.h"
#include "api_types.h"
#include "system_config.h"
#include "system_context.h"

namespace hardware_pruefadapter {
namespace logic {

class ApiController {
 public:
  ApiController(interfaces::IWebServer* web_server,
                const core::SystemContext& ctx);
  ~ApiController();

  void RegisterRoutes();

 private:
  std::string HandleGetSystemStatus();
  std::string HandleGetLogs();
  std::string HandleGetIoAll();
  std::string HandleGetAnalog(std::uint8_t channel,
                              types::IO::Direction direction);
  std::string HandleGetDigital(std::uint8_t channel,
                               types::IO::Direction direction);
  std::string HandleGetI2cDevices();
  std::string HandlePutAnalogOutput(std::uint8_t channel,
                                    const std::string& body);
  std::string HandlePutDigitalOutput(std::uint8_t channel,
                                     const std::string& body);
  std::string HandlePutDigitalReference(std::uint8_t channel,
                                        types::IO::Direction direction,
                                        const std::string& body);
  std::string HandlePutSerialTx(const std::string& body);
  std::string HandlePutSerialConfig(const std::string& body);

  interfaces::IWebServer* web_server_;
  const core::SystemContext& ctx_;
};

}  // namespace logic
}  // namespace hardware_pruefadapter

#endif  // HARDWARE_PRUEFADAPTER_LOGIC_API_CONTROLLER_H_