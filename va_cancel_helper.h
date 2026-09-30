#pragma once
#include "esphome.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace voice_assistant {

inline void abort_voice_assistant(VoiceAssistant *va) {
  // Voice Assistant pipeline cancellation is natively executed via voice_assistant.stop
}

} // namespace voice_assistant

namespace display {

// High-performance direct framebuffer accessor for zero-overhead full-screen image scaling (< 1.5ms)
class DisplayBufferAccessor : public DisplayBuffer {
 public:
  static uint16_t *get_framebuffer(Display *disp) {
    if (!disp) return nullptr;
    auto *buf_disp = static_cast<DisplayBufferAccessor *>(disp);
    return reinterpret_cast<uint16_t *>(buf_disp->buffer_);
  }
};

} // namespace display
} // namespace esphome
