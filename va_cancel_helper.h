#pragma once
#include "esphome.h"

namespace esphome {
namespace voice_assistant {

inline void abort_voice_assistant(VoiceAssistant *va) {
  if (va != nullptr) {
    va->request_stop();
    va->reset_conversation_id();
  }
  if (api::global_api_server != nullptr) {
    for (auto &client : api::global_api_server->active_clients()) {
      api::VoiceAssistantRequest msg;
      msg.start = false;
      bool sent = client->send_message(msg);
      (void)sent;
    }
  }
}

} // namespace voice_assistant
} // namespace esphome
