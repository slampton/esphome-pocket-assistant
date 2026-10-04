#pragma once
#include "esphome.h"
#include "esphome/components/api/api_server.h"
#include "esphome/components/api/api_pb2.h"
#include "esphome/components/voice_assistant/voice_assistant.h"

namespace esphome {
namespace voice_assistant {

inline void abort_voice_assistant(VoiceAssistant *va) {
  if (va != nullptr) {
    va->request_stop();
  }
#if defined(USE_API)
  if (api::global_api_server != nullptr) {
    for (auto &client : api::global_api_server->active_clients()) {
      if (client != nullptr) {
        api::VoiceAssistantRequest msg;
        msg.start = false;
        client->send_message(msg);
      }
    }
  }
#endif
}

} // namespace voice_assistant
} // namespace esphome
