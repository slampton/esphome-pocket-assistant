#pragma once
#include "esphome.h"
#include "esphome/core/version.h"

#if defined(ESPHOME_VERSION_CODE) && defined(VERSION_CODE)
static_assert(ESPHOME_VERSION_CODE >= VERSION_CODE(2026, 10, 0),
              "va_cancel_helper.h requires ESPHome 2026.10.0 or newer.");
#endif
#ifdef USE_ESP32
#include <esp_sleep.h>
#endif

// ==============================================================================
// Voice Assistant Session, Mixer & Wake Word Helpers (va_cancel_helper.h)
// ==============================================================================
// Provides clean, encapsulated lifecycle helpers for ESPHome's final VoiceAssistant
// component, its PSRAM MixerSpeaker announcement channel, and on-board
// micro_wake_word configuration + NVS flash persistence:
//   1. disarm_va_speaker_timeout(va)
//   2. reset_va_session_state(va)
//   3. prewarm_va_tts_mixer(va, mixer_in)
//   4. abort_voice_assistant(va)
//   5. ensure_wake_words_restored(va) & poll_and_persist_wake_words(va)
//   6. on_api_client_authenticated(va, client_info, client_address)
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

#ifdef USE_SPEAKER
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
#endif

#ifdef USE_MEDIA_PLAYER
struct VA_StartedStreamingTts_Tag {
  typedef bool esphome::voice_assistant::VoiceAssistant::*type;
  friend type get(VA_StartedStreamingTts_Tag);
};
template struct Rob<VA_StartedStreamingTts_Tag, &esphome::voice_assistant::VoiceAssistant::started_streaming_tts_>;
#endif

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

#ifdef USE_MICRO_WAKE_WORD
struct VA_MicroWakeWord_Tag {
  typedef esphome::micro_wake_word::MicroWakeWord *esphome::voice_assistant::VoiceAssistant::*type;
  friend type get(VA_MicroWakeWord_Tag);
};
template struct Rob<VA_MicroWakeWord_Tag, &esphome::voice_assistant::VoiceAssistant::micro_wake_word_>;
#endif

struct VA_ApiClient_Tag {
  typedef esphome::api::APIConnection *esphome::voice_assistant::VoiceAssistant::*type;
  friend type get(VA_ApiClient_Tag);
};
template struct Rob<VA_ApiClient_Tag, &esphome::voice_assistant::VoiceAssistant::api_client_>;

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

// Disarms VoiceAssistant's internal "speaker-timeout" and "playing" timers so responses
// buffered inside announcement_mixing_input's 15s PSRAM ring buffer never prematurely time out.
inline void disarm_va_speaker_timeout(VoiceAssistant *va) {
  ComponentTimeoutAccessor::cancel_va_speaker_timeout(va);
}

// Resets per-utterance stream/buffer tracking flags and disarms any leftover speaker timeouts.
inline void reset_va_session_state(VoiceAssistant *va) {
  if (va != nullptr) {
#ifdef USE_SPEAKER
    va->*get(VA_BufSize_Tag()) = 0;
    va->*get(VA_BufIndex_Tag()) = 0;
    va->*get(VA_BytesReceived_Tag()) = 0;
    va->*get(VA_WaitForStreamEnd_Tag()) = false;
    va->*get(VA_StreamEnded_Tag()) = false;
#endif
#ifdef USE_MEDIA_PLAYER
    va->*get(VA_StartedStreamingTts_Tag()) = false;
#endif
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

// ==============================================================================
// Wake Word NVS Persistence, Model State Sync & API Client Handoff
// ==============================================================================
struct SavedWakeWordsNVS {
  uint32_t magic;
  char ww1[28];
  char ww2[28];
};

static constexpr uint32_t WAKE_WORDS_NVS_KEY = 0x50415757UL;
static constexpr uint32_t WAKE_WORDS_NVS_MAGIC = 0x50415733UL;

inline bool &wake_words_restored_flag() {
  static bool restored = false;
  return restored;
}

inline std::string &last_persisted_ww1() {
  static std::string s;
  return s;
}

inline void save_wake_words_to_nvs(const std::string &w1) {
  last_persisted_ww1() = w1;
  if (global_preferences == nullptr) return;
  SavedWakeWordsNVS data{};
  data.magic = WAKE_WORDS_NVS_MAGIC;
  std::snprintf(data.ww1, sizeof(data.ww1), "%s", w1.c_str());
  data.ww2[0] = '\0';
  auto pref = global_preferences->make_preference<SavedWakeWordsNVS>(WAKE_WORDS_NVS_KEY);
  pref.save(&data);
  global_preferences->sync();
}

// Restores the selected wake word from NVS on boot (defaulting to only "okay_nabu"
// on first boot instead of leaving all 5 compiled models simultaneously enabled).
inline void ensure_wake_words_restored(VoiceAssistant *va) {
  if (wake_words_restored_flag() || va == nullptr) return;
#ifdef USE_MICRO_WAKE_WORD
  auto *mww = va->*get(VA_MicroWakeWord_Tag());
  if (mww == nullptr) return;
  wake_words_restored_flag() = true;

  std::string w1 = "okay_nabu";
  if (global_preferences != nullptr) {
    SavedWakeWordsNVS data{};
    auto pref = global_preferences->make_preference<SavedWakeWordsNVS>(WAKE_WORDS_NVS_KEY);
    if (pref.load(&data) && data.magic == WAKE_WORDS_NVS_MAGIC) {
      data.ww1[sizeof(data.ww1) - 1] = '\0';
      w1 = data.ww1;
    } else {
      save_wake_words_to_nvs(w1);
    }
  }
  last_persisted_ww1() = w1;

  for (auto *model : mww->get_wake_words()) {
    model->disable();
  }
  bool matched = false;
  for (auto *model : mww->get_wake_words()) {
    if (!w1.empty() && model->get_id() == w1) {
      model->enable();
      matched = true;
      break;
    }
  }
  if (!matched && w1 != "no_wake_word" && !mww->get_wake_words().empty()) {
    mww->get_wake_words().front()->enable();
    last_persisted_ww1() = mww->get_wake_words().front()->get_id();
  }
#endif
}

// Called synchronously from `api: on_client_connected:` inside APIConnection::complete_authentication_()
// during HelloRequest — BEFORE SubscribeVoiceAssistantRequest and VoiceAssistantConfigurationRequest
// are sent by Home Assistant. Ensures wake word models are initialized from NVS and releases any
// stale api_client_ pointer from a previous socket so VoiceAssistant::client_subscription() always
// binds the new Home Assistant connection and returns all 5 wake word models.
inline void on_api_client_authenticated(VoiceAssistant *va,
                                        const std::string &client_info,
                                        const std::string &client_address) {
  (void)client_address;
  if (va == nullptr) return;
  ensure_wake_words_restored(va);
  if (client_info.find("Home Assistant") != std::string::npos) {
    if (va->get_api_connection() != nullptr) {
      ESP_LOGI("va_api", "Releasing stale VoiceAssistant API client for new Home Assistant connection (%s)",
               client_address.c_str());
      va->*get(VA_ApiClient_Tag()) = nullptr;
    }
  }
}

// Polls mww->get_wake_words() once per second to detect when Home Assistant updates
// the active wake word via on_set_configuration, persisting the new selection to NVS
// and restarting mww so only the selected TFLite model is loaded in PSRAM.
inline void poll_and_persist_wake_words(VoiceAssistant *va) {
  if (va == nullptr) return;
  ensure_wake_words_restored(va);
#ifdef USE_MICRO_WAKE_WORD
  auto *mww = va->*get(VA_MicroWakeWord_Tag());
  if (mww == nullptr) return;

  std::string cur_w1 = "no_wake_word";
  for (auto *model : mww->get_wake_words()) {
    if (model->is_enabled()) {
      if (cur_w1 == "no_wake_word") {
        cur_w1 = model->get_id();
      } else {
        // Keep at most 1 active wake word to match ESPHome's max_active_wake_words = 1
        model->disable();
      }
    }
  }

  if (cur_w1 != last_persisted_ww1()) {
    save_wake_words_to_nvs(cur_w1);
    if (mww->is_running()) {
      mww->stop();
    }
  }
#endif
}

// Cleanly sends ESPHome DisconnectRequest + TCP FIN to all connected API clients
// before the AXP2101 cuts the 3.3V power rail in Hibernate (enter_deep_sleep).
// Prevents Home Assistant from staying stuck on a half-open ("zombie") TCP socket
// for 60-90s after power-off and ignoring mDNS wake announcements on the next boot.
inline void clean_disconnect_api_before_hibernate(VoiceAssistant *va) {
  if (va != nullptr) {
    va->*get(VA_ApiClient_Tag()) = nullptr;
  }
  if (api::global_api_server != nullptr) {
    ESP_LOGI("power", "Sending clean API DisconnectRequest to Home Assistant before Hibernate rail cut");
    api::global_api_server->on_shutdown();
  }
}

}  // namespace voice_assistant
}  // namespace esphome
