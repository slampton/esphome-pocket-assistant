#pragma once
#include "esphome.h"

namespace esphome {
namespace voice_assistant {

class VoiceAssistantAccessor : public VoiceAssistant {
 public:
  void abort_pipeline() {
    ESP_LOGI("va_helper", "Aborting Voice Assistant pipeline and resetting to IDLE");
    // 1. Send cancel request to Home Assistant to immediately abort server pipeline
    this->signal_stop_();
    // 2. Stop hardware speaker and clear buffers
#ifdef USE_SPEAKER
    if (this->speaker_ != nullptr) {
      this->speaker_->stop();
    }
    this->cancel_timeout("speaker-timeout");
    this->cancel_timeout("playing");
    this->speaker_buffer_size_ = 0;
    this->speaker_buffer_index_ = 0;
    this->speaker_bytes_received_ = 0;
    this->wait_for_stream_end_ = false;
    this->stream_ended_ = false;
#endif
    this->clear_buffers_();
    // 3. Reset internal state machine to IDLE so the next voice_assistant.start is accepted immediately
    this->continuous_ = false;
    this->continue_conversation_ = false;
    this->set_state_(State::IDLE, State::IDLE);
  }
};

inline void abort_voice_assistant(VoiceAssistant *va) {
  if (va == nullptr) return;
  static_cast<VoiceAssistantAccessor *>(va)->abort_pipeline();
}

} // namespace voice_assistant
} // namespace esphome
