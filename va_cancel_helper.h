#pragma once
#include "esphome.h"
#include <esp_wifi.h>

namespace esphome {
namespace power {

inline void set_wifi_power_save(bool enabled) {
  esp_wifi_set_ps(enabled ? WIFI_PS_MIN_MODEM : WIFI_PS_NONE);
}

} // namespace power

namespace voice_assistant {

inline void abort_voice_assistant(VoiceAssistant *va) {
  // Voice Assistant pipeline cancellation is natively executed via voice_assistant.stop
}

} // namespace voice_assistant
} // namespace esphome
