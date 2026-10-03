#pragma once
#include "esphome.h"

namespace esphome {
namespace voice_assistant {

class VoiceAssistantAccessor : public VoiceAssistant {
 public:
  void abort_pipeline() {
    ESP_LOGI("va_helper", "Aborting Voice Assistant pipeline and resetting to IDLE");
    // Send cancel request to Home Assistant to immediately abort server pipeline
    this->signal_stop_();
    // Flush the internal API audio staging buffer
    this->speaker_buffer_size_ = 0;
    this->speaker_buffer_index_ = 0;
    this->speaker_bytes_received_ = 0;
    this->wait_for_stream_end_ = false;
    this->stream_ended_ = false;
    this->clear_buffers_();
    this->continuous_ = false;
    this->continue_conversation_ = false;
    this->set_state_(State::IDLE, State::IDLE);
  }
};

inline void abort_voice_assistant(VoiceAssistant *va) {
  if (va != nullptr) {
    static_cast<VoiceAssistantAccessor *>(va)->abort_pipeline();
  }
}

} // namespace voice_assistant
} // namespace esphome
