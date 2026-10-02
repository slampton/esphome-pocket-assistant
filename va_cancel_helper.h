#pragma once
#include "esphome.h"

namespace esphome {
namespace voice_assistant {

inline void abort_voice_assistant(VoiceAssistant *va) {
  // Voice Assistant pipeline cancellation is natively executed via voice_assistant.stop
}

} // namespace voice_assistant
} // namespace esphome
