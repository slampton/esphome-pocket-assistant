#pragma once
#include "esphome.h"

// ==============================================================================
// Voice Assistant Session & Mixer Lifecycle Helpers (va_cancel_helper.h)
// ==============================================================================
// Provides clean, encapsulated lifecycle helpers for ESPHome's final VoiceAssistant
// component and its PSRAM MixerSpeaker announcement channel:
//   1. disarm_va_speaker_timeout(va)
//   2. reset_va_session_state(va)
//   3. prewarm_va_tts_mixer(va, mixer_in)
//   4. abort_voice_assistant(va)
// ==============================================================================

template<typename Tag, typename Tag::type M>
struct Rob {
  friend typename Tag::type get(Tag) { return M; }
};

struct VA_ClearBuffers_Tag {
  typedef void (esphome::voice_assistant::VoiceAssistant::*type)();
  friend type get(VA_ClearBuffers_Tag);
};
template struct Rob<VA_ClearBuffers_Tag, &esphome::voice_assistant::VoiceAssistant::clear_buffers_>;

struct VA_SetState_Tag {
  typedef void (esphome::voice_assistant::VoiceAssistant::*type)(esphome::voice_assistant::State, esphome::voice_assistant::State);
  friend type get(VA_SetState_Tag);
};
template struct Rob<VA_SetState_Tag, &esphome::voice_assistant::VoiceAssistant::set_state_>;

struct VA_BufSize_Tag {
  typedef size_t esphome::voice_assistant::VoiceAssistant::*type;
  friend type get(VA_BufSize_Tag);
};
template struct Rob<VA_BufSize_Tag, &esphome::voice_assistant::VoiceAssistant::speaker_buffer_size_>;

struct VA_BufIndex_Tag {
  typedef size_t esphome::voice_assistant::VoiceAssistant::*type;
  friend type get(VA_BufIndex_Tag);
};
template struct Rob<VA_BufIndex_Tag, &esphome::voice_assistant::VoiceAssistant::speaker_buffer_index_>;

struct VA_BytesReceived_Tag {
  typedef size_t esphome::voice_assistant::VoiceAssistant::*type;
  friend type get(VA_BytesReceived_Tag);
};
template struct Rob<VA_BytesReceived_Tag, &esphome::voice_assistant::VoiceAssistant::speaker_bytes_received_>;

struct VA_WaitForStreamEnd_Tag {
  typedef bool esphome::voice_assistant::VoiceAssistant::*type;
  friend type get(VA_WaitForStreamEnd_Tag);
};
template struct Rob<VA_WaitForStreamEnd_Tag, &esphome::voice_assistant::VoiceAssistant::wait_for_stream_end_>;

struct VA_StreamEnded_Tag {
  typedef bool esphome::voice_assistant::VoiceAssistant::*type;
  friend type get(VA_StreamEnded_Tag);
};
template struct Rob<VA_StreamEnded_Tag, &esphome::voice_assistant::VoiceAssistant::stream_ended_>;

struct VA_Continuous_Tag {
  typedef bool esphome::voice_assistant::VoiceAssistant::*type;
  friend type get(VA_Continuous_Tag);
};
template struct Rob<VA_Continuous_Tag, &esphome::voice_assistant::VoiceAssistant::continuous_>;

struct VA_ContinueConversation_Tag {
  typedef bool esphome::voice_assistant::VoiceAssistant::*type;
  friend type get(VA_ContinueConversation_Tag);
};
template struct Rob<VA_ContinueConversation_Tag, &esphome::voice_assistant::VoiceAssistant::continue_conversation_>;

namespace esphome {
namespace voice_assistant {

struct ComponentTimeoutAccessor : public esphome::Component {
  static void cancel_va_speaker_timeout(esphome::Component *comp) {
    if (comp != nullptr) {
      static_cast<ComponentTimeoutAccessor *>(comp)->cancel_timeout("speaker-timeout");
      static_cast<ComponentTimeoutAccessor *>(comp)->cancel_timeout("playing");
    }
  }
};

// Disarms VoiceAssistant's internal 5000ms "speaker-timeout" so responses buffered inside
// announcement_mixing_input's 15s PSRAM ring buffer never prematurely time out at 5.0s.
inline void disarm_va_speaker_timeout(VoiceAssistant *va) {
  ComponentTimeoutAccessor::cancel_va_speaker_timeout(va);
}

// Resets per-utterance stream/buffer tracking flags and disarms any leftover speaker timeouts.
inline void reset_va_session_state(VoiceAssistant *va) {
  if (va != nullptr) {
    va->*get(VA_BufSize_Tag()) = 0;
    va->*get(VA_BufIndex_Tag()) = 0;
    va->*get(VA_BytesReceived_Tag()) = 0;
    va->*get(VA_WaitForStreamEnd_Tag()) = false;
    va->*get(VA_StreamEnded_Tag()) = false;
    disarm_va_speaker_timeout(va);
  }
}

// Pre-warms the announcement SourceSpeaker into SPEAKER_STATE_RUNNING during Thinking
// (after microphone capture ends and while the ES8311 DAC is hardware-muted) so its
// 15s (480 KB) PSRAM ring_buffer_ is open before sub-20ms local-intent TTS bursts arrive.
template<typename SpeakerT>
inline void prewarm_va_tts_mixer(VoiceAssistant *va, SpeakerT *mixer_in) {
  disarm_va_speaker_timeout(va);
  if (mixer_in != nullptr) {
    if (!mixer_in->is_running()) {
      mixer_in->start();
    } else {
      const int16_t silence_pcm = 0;
      mixer_in->play(reinterpret_cast<const uint8_t *>(&silence_pcm), sizeof(silence_pcm));
    }
  }
}

// Performs a complete, synchronous client + server VoiceAssistant session abort.
inline void abort_voice_assistant(VoiceAssistant *va) {
  if (va != nullptr) {
    va->request_stop();
    va->reset_conversation_id();

    reset_va_session_state(va);
    va->*get(VA_Continuous_Tag()) = false;
    va->*get(VA_ContinueConversation_Tag()) = false;

    (va->*get(VA_ClearBuffers_Tag()))();
    (va->*get(VA_SetState_Tag()))(State::IDLE, State::IDLE);
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

}  // namespace voice_assistant
}  // namespace esphome
