#pragma once
#include "esphome.h"

namespace esphome {
namespace voice_assistant {

inline void abort_voice_assistant(VoiceAssistant *va) {
  if (va != nullptr) {
    va->signal_stop();
  }
}

} // namespace voice_assistant
} // namespace esphome
