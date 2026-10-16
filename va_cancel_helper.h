#pragma once
#include "esphome.h"
#ifdef USE_ESP32
#include <esp_sleep.h>
#endif

// ==============================================================================
// Voice Assistant Session, Mixer & Dual Wake Word Helpers (va_cancel_helper.h)
// ==============================================================================
// Provides clean, encapsulated lifecycle helpers for ESPHome's final VoiceAssistant
// component, its PSRAM MixerSpeaker announcement channel, and dual on-board
// micro_wake_word configuration + NVS flash persistence:
//   1. disarm_va_speaker_timeout(va)
//   2. reset_va_session_state(va)
//   3. prewarm_va_tts_mixer(va, mixer_in)
//   4. abort_voice_assistant(va)
//   5. ensure_wake_words_restored(va) & poll_and_persist_wake_words(va)
//   6. Linker --wrap hooks for max_active_wake_words = 2 & API client binding
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

struct VA_Config_Tag {
  typedef esphome::voice_assistant::Configuration esphome::voice_assistant::VoiceAssistant::*type;
  friend type get(VA_Config_Tag);
};
template struct Rob<VA_Config_Tag, &esphome::voice_assistant::VoiceAssistant::config_>;

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

// ==============================================================================
// Dual Wake Word NVS Persistence & Model State Synchronization
// ==============================================================================
struct SavedWakeWordsNVS {
  uint32_t magic;
  char ww1[28];
  char ww2[28];
};

static constexpr uint32_t WAKE_WORDS_NVS_KEY = 0x50415757UL;
static constexpr uint32_t WAKE_WORDS_NVS_MAGIC = 0x50415732UL;

inline bool &wake_words_restored_flag() {
  static bool restored = false;
  return restored;
}

inline std::string &last_persisted_ww1() {
  static std::string s;
  return s;
}

inline std::string &last_persisted_ww2() {
  static std::string s;
  return s;
}

inline void save_wake_words_to_nvs(const std::string &w1, const std::string &w2) {
  last_persisted_ww1() = w1;
  last_persisted_ww2() = w2;
  if (global_preferences == nullptr) return;
  SavedWakeWordsNVS data{};
  data.magic = WAKE_WORDS_NVS_MAGIC;
  std::snprintf(data.ww1, sizeof(data.ww1), "%s", w1.c_str());
  std::snprintf(data.ww2, sizeof(data.ww2), "%s", w2.c_str());
  auto pref = global_preferences->make_preference<SavedWakeWordsNVS>(WAKE_WORDS_NVS_KEY);
  pref.save(&data);
  global_preferences->sync();
}

// Restores the 1 or 2 selected wake words from NVS on boot (defaulting to only "okay_nabu"
// on first boot instead of leaving all 5 compiled models simultaneously enabled).
inline void ensure_wake_words_restored(VoiceAssistant *va) {
  if (wake_words_restored_flag() || va == nullptr) return;
#ifdef USE_MICRO_WAKE_WORD
  auto *mww = va->*get(VA_MicroWakeWord_Tag());
  if (mww == nullptr) return;
  wake_words_restored_flag() = true;

  std::string w1 = "okay_nabu";
  std::string w2 = "";
  if (global_preferences != nullptr) {
    SavedWakeWordsNVS data{};
    auto pref = global_preferences->make_preference<SavedWakeWordsNVS>(WAKE_WORDS_NVS_KEY);
    if (pref.load(&data) && data.magic == WAKE_WORDS_NVS_MAGIC) {
      data.ww1[sizeof(data.ww1) - 1] = '\0';
      data.ww2[sizeof(data.ww2) - 1] = '\0';
      w1 = data.ww1;
      w2 = data.ww2;
    } else {
      save_wake_words_to_nvs(w1, w2);
    }
  }
  last_persisted_ww1() = w1;
  last_persisted_ww2() = w2;

  for (auto *model : mww->get_wake_words()) {
    model->disable();
  }
  for (auto *model : mww->get_wake_words()) {
    const std::string id_str = model->get_id();
    if ((!w1.empty() && id_str == w1) || (!w2.empty() && id_str == w2)) {
      model->enable();
    }
  }
#endif
}

// Polls mww->get_wake_words() once per second to detect when Home Assistant updates
// the active wake words via on_set_configuration, persisting the new selection to NVS
// and restarting mww so only the active TFLite models are loaded in PSRAM.
inline void poll_and_persist_wake_words(VoiceAssistant *va) {
  if (va == nullptr) return;
  ensure_wake_words_restored(va);
#ifdef USE_MICRO_WAKE_WORD
  auto *mww = va->*get(VA_MicroWakeWord_Tag());
  if (mww == nullptr) return;

  std::string cur_w1 = "";
  std::string cur_w2 = "";
  for (auto *model : mww->get_wake_words()) {
    if (model->is_enabled()) {
      if (cur_w1.empty()) {
        cur_w1 = model->get_id();
      } else if (cur_w2.empty()) {
        cur_w2 = model->get_id();
      } else {
        // Never allow more than 2 simultaneous active wake words
        model->disable();
      }
    }
  }

  if (cur_w1 != last_persisted_ww1() || cur_w2 != last_persisted_ww2()) {
    save_wake_words_to_nvs(cur_w1, cur_w2);
    if (mww->is_running()) {
      mww->stop();
    }
  }
#endif
}

}  // namespace voice_assistant
}  // namespace esphome

// ==============================================================================
// Linker --wrap Hooks for Dual Wake Word Support (max_active_wake_words = 2)
// Implemented directly without calling __real_* so linking succeeds under any
// toolchain configuration.
// ==============================================================================
extern "C" {

__attribute__((used))
bool __wrap__ZNK7esphome3api13APIConnection37check_voice_assistant_api_connection_Ev(
    const esphome::api::APIConnection *self) {
  if (esphome::voice_assistant::global_voice_assistant == nullptr) {
    return false;
  }
  if (esphome::voice_assistant::global_voice_assistant->get_api_connection() != self) {
    esphome::voice_assistant::global_voice_assistant->*get(VA_ApiClient_Tag()) =
        const_cast<esphome::api::APIConnection *>(self);
  }
  return true;
}

__attribute__((used))
const esphome::voice_assistant::Configuration &
__wrap__ZN7esphome15voice_assistant14VoiceAssistant17get_configurationEv(
    esphome::voice_assistant::VoiceAssistant *self) {
  esphome::voice_assistant::ensure_wake_words_restored(self);
  auto &cfg = self->*get(VA_Config_Tag());
  cfg.available_wake_words.clear();
  cfg.active_wake_words.clear();
#ifdef USE_MICRO_WAKE_WORD
  auto *mww = self->*get(VA_MicroWakeWord_Tag());
  if (mww != nullptr) {
    cfg.max_active_wake_words = 2;
    for (auto *model : mww->get_wake_words()) {
      if (model->is_enabled() && cfg.active_wake_words.size() < 2) {
        cfg.active_wake_words.push_back(model->get_id());
      }
      esphome::voice_assistant::WakeWord ww;
      ww.id = model->get_id();
      ww.wake_word = model->get_wake_word();
      for (const auto &lang : model->get_trained_languages()) {
        ww.trained_languages.push_back(lang);
      }
      cfg.available_wake_words.push_back(std::move(ww));
    }
  } else {
    cfg.max_active_wake_words = 0;
  }
#else
  cfg.max_active_wake_words = 0;
#endif
  return cfg;
}

__attribute__((used))
void __wrap__ZN7esphome15voice_assistant14VoiceAssistant20on_set_configurationERKSt6vectorINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESaIS7_EE(
    esphome::voice_assistant::VoiceAssistant *self,
    const std::vector<std::string> &active_wake_words) {
  esphome::voice_assistant::wake_words_restored_flag() = true;
#ifdef USE_MICRO_WAKE_WORD
  auto *mww = self->*get(VA_MicroWakeWord_Tag());
  if (mww != nullptr) {
    for (auto *model : mww->get_wake_words()) {
      model->disable();
    }
    std::string w1 = "";
    std::string w2 = "";
    for (const auto &ww_id : active_wake_words) {
      if (ww_id.empty() || ww_id == "no_wake_word") continue;
      for (auto *model : mww->get_wake_words()) {
        if (model->get_id() == ww_id) {
          model->enable();
          if (w1.empty()) w1 = ww_id;
          else if (w2.empty() && ww_id != w1) w2 = ww_id;
        }
      }
    }
    esphome::voice_assistant::save_wake_words_to_nvs(w1, w2);
    if (mww->is_running()) {
      mww->stop();
    }
  }
#else
  (void)self;
  (void)active_wake_words;
#endif
}

}  // extern "C"
