#pragma once
#include "esphome.h"
#include "esphome/components/api/api_server.h"
#include "esphome/components/api/api_pb2.h"
#include "esphome/components/voice_assistant/voice_assistant.h"

namespace esphome {
namespace voice_assistant {

class VoiceAssistantAccessor : public VoiceAssistant {
 public:
  void force_abort_pipeline() {
    ESP_LOGI("va_helper", "Aborting Voice Assistant pipeline and resetting state to IDLE");
    this->signal_stop_();
    this->clear_buffers_();
    this->set_state_(State::IDLE, State::IDLE);
  }
};

inline void abort_voice_assistant(VoiceAssistant *va) {
  if (va != nullptr) {
    static_cast<VoiceAssistantAccessor *>(va)->force_abort_pipeline();
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
