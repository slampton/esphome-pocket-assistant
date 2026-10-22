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
//   3. abort_voice_assistant(va)
//   4. ensure_wake_words_restored(va) & poll_and_persist_wake_words(va)
//   5. on_api_client_authenticated(va, client_info, client_address)
//   6. clean_disconnect_api_before_hibernate(va)
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
    }
  }
};

// Disarms VoiceAssistant's 10s "speaker-timeout" watchdog and clears continue_conversation_
// so LLM responses buffered in announcement_mixing_input's 15s PSRAM ring buffer never
// re-open the microphone mid-sentence, while letting VoiceAssistant's native "playing"
// completion timer transition cleanly to IDLE and fire on_end.
inline void disarm_va_speaker_timeout(VoiceAssistant *va) {
  if (va != nullptr) {
    va->*get(VA_ContinueConversation_Tag()) = false;
  }
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
// Wake Word NVS Persistence (Dual-Slot ww1 & ww2), Model State Sync & API Handoff
// ==============================================================================
struct SavedWakeWordsNVS {
  uint32_t magic;
  char ww1[28];
  char ww2[28];
};

static constexpr uint32_t WAKE_WORDS_NVS_KEY = 0x50415757UL;
static constexpr uint32_t WAKE_WORDS_NVS_MAGIC = 0x50415734UL;
static constexpr uint32_t WAKE_WORDS_NVS_MAGIC_LEGACY = 0x50415733UL;

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

inline void save_wake_words_to_nvs(const std::string &w1, const std::string &w2 = "") {
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
  ESP_LOGI("va_nvs", "Saved wake words to NVS: ww1='%s', ww2='%s'", w1.c_str(), w2.c_str());
}

// Restores both Wake Word 1 and Wake Word 2 from NVS on boot
inline void ensure_wake_words_restored(VoiceAssistant *va, bool force = false) {
  if ((wake_words_restored_flag() && !force) || va == nullptr) return;
#ifdef USE_MICRO_WAKE_WORD
  auto *mww = va->*get(VA_MicroWakeWord_Tag());
  if (mww == nullptr) return;
  wake_words_restored_flag() = true;

  std::string w1 = "okay_nabu";
  std::string w2 = "";
  if (global_preferences != nullptr) {
    SavedWakeWordsNVS data{};
    auto pref = global_preferences->make_preference<SavedWakeWordsNVS>(WAKE_WORDS_NVS_KEY);
    if (pref.load(&data) && (data.magic == WAKE_WORDS_NVS_MAGIC || data.magic == WAKE_WORDS_NVS_MAGIC_LEGACY)) {
      data.ww1[sizeof(data.ww1) - 1] = '\0';
      data.ww2[sizeof(data.ww2) - 1] = '\0';
      w1 = data.ww1;
      if (data.magic == WAKE_WORDS_NVS_MAGIC) {
        w2 = data.ww2;
      }
    } else {
      save_wake_words_to_nvs(w1, w2);
    }
  }
  last_persisted_ww1() = w1;
  last_persisted_ww2() = w2;

  for (auto *model : mww->get_wake_words()) {
    model->disable();
  }
  int matched_count = 0;
  for (auto *model : mww->get_wake_words()) {
    const std::string id_str = model->get_id();
    if ((!w1.empty() && w1 != "no_wake_word" && id_str == w1) ||
        (!w2.empty() && w2 != "no_wake_word" && id_str == w2)) {
      model->enable();
      matched_count++;
    }
  }
  if (matched_count == 0 && w1 != "no_wake_word" && !mww->get_wake_words().empty()) {
    mww->get_wake_words().front()->enable();
    last_persisted_ww1() = mww->get_wake_words().front()->get_id();
  }
#endif
}

// Called synchronously from `api: on_client_connected:` inside APIConnection::complete_authentication_()
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
// Wake Word 1 or Wake Word 2 via on_set_configuration, persisting both slots to NVS.
inline bool poll_and_persist_wake_words(VoiceAssistant *va) {
  if (va == nullptr) return false;
  ensure_wake_words_restored(va);
#ifdef USE_MICRO_WAKE_WORD
  auto *mww = va->*get(VA_MicroWakeWord_Tag());
  if (mww == nullptr) return false;

  std::string cur_w1 = "no_wake_word";
  std::string cur_w2 = "";
  for (auto *model : mww->get_wake_words()) {
    if (model->is_enabled()) {
      if (cur_w1 == "no_wake_word") {
        cur_w1 = model->get_id();
      } else if (cur_w2.empty()) {
        cur_w2 = model->get_id();
      } else {
        // Keep at most 2 active wake words to match Home Assistant's 2-slot Assist Satellite UI
        model->disable();
      }
    }
  }

  if (cur_w1 != last_persisted_ww1() || cur_w2 != last_persisted_ww2()) {
    save_wake_words_to_nvs(cur_w1, cur_w2);
    if (mww->is_running()) {
      mww->stop();
    }
    return true;
  }
#endif
  return false;
}
inline const char *wake_word_id_to_label(const std::string &id_str) {
  if (id_str == "hey_luna") return "Hey Luna";
  if (id_str == "hey_jarvis") return "Hey Jarvis";
  if (id_str == "hey_mycroft") return "Hey Mycroft";
  if (id_str == "alexa") return "Alexa";
  return "Okay Nabu";
}

inline const char *wake_word_label_to_id(const std::string &label) {
  if (label == "Hey Luna") return "hey_luna";
  if (label == "Hey Jarvis") return "hey_jarvis";
  if (label == "Hey Mycroft") return "hey_mycroft";
  if (label == "Alexa") return "alexa";
  return "okay_nabu";
}

inline void set_primary_wake_word_by_label(VoiceAssistant *va, const std::string &label) {
  if (va == nullptr) return;
  const std::string target_id = wake_word_label_to_id(label);
#ifdef USE_MICRO_WAKE_WORD
  auto *mww = va->*get(VA_MicroWakeWord_Tag());
  if (mww == nullptr) return;
  std::string cur_w2 = last_persisted_ww2();
  if (cur_w2 == target_id) cur_w2 = "";

  for (auto *model : mww->get_wake_words()) {
    const std::string m_id = model->get_id();
    if (m_id == target_id || (!cur_w2.empty() && m_id == cur_w2)) {
      model->enable();
    } else {
      model->disable();
    }
  }
  save_wake_words_to_nvs(target_id, cur_w2);
  if (mww->is_running()) {
    mww->stop();
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

#ifdef USE_MEDIA_SOURCE
struct MS_Listener_Tag {
  typedef esphome::media_source::MediaSourceListener *esphome::media_source::MediaSource::*type;
  friend type get(MS_Listener_Tag);
};
template struct Rob<MS_Listener_Tag, &esphome::media_source::MediaSource::listener_>;

namespace esphome {
namespace voice_assistant {

class SendspinMonoDownmixListener final : public esphome::media_source::MediaSourceListener {
 public:
  void wrap(esphome::media_source::MediaSource *src) {
    if (src == nullptr) return;
    auto *cur = src->*get(MS_Listener_Tag());
    if (cur != nullptr && cur != this) {
      this->orig_ = cur;
      src->set_listener(this);
      ESP_LOGI("audio", "Installed L+R -> Mono downmixer on SendspinMediaSource");
    }
  }

  size_t write_audio(const uint8_t *data, size_t length, uint32_t timeout_ms,
                     const esphome::audio::AudioStreamInfo &stream_info) override {
    if (this->orig_ == nullptr) return 0;
    if (stream_info.get_channels() == 2 && stream_info.get_bits_per_sample() == 16) {
      size_t remaining_frames = length / 4;
      if (remaining_frames == 0) return 0;
      static constexpr size_t CHUNK_FRAMES = 512;
      int16_t mono_buf[CHUNK_FRAMES];
      const int16_t *stereo_in = reinterpret_cast<const int16_t *>(data);
      esphome::audio::AudioStreamInfo mono_info(16, 1, stream_info.get_sample_rate());
      size_t total_stereo_bytes = 0;

      while (remaining_frames > 0) {
        size_t n = (remaining_frames > CHUNK_FRAMES) ? CHUNK_FRAMES : remaining_frames;
        for (size_t i = 0; i < n; i++) {
          int32_t l = stereo_in[i * 2];
          int32_t r = stereo_in[i * 2 + 1];
          mono_buf[i] = static_cast<int16_t>((l + r) >> 1);
        }
        size_t want_mono_bytes = n * sizeof(int16_t);
        size_t mono_written = this->orig_->write_audio(
            reinterpret_cast<const uint8_t *>(mono_buf),
            want_mono_bytes,
            timeout_ms,
            mono_info);
        total_stereo_bytes += mono_written * 2;
        size_t frames_done = mono_written / sizeof(int16_t);
        stereo_in += frames_done * 2;
        remaining_frames -= frames_done;
        if (mono_written < want_mono_bytes) {
          break;
        }
      }
      return total_stereo_bytes;
    }
    return this->orig_->write_audio(data, length, timeout_ms, stream_info);
  }

  void report_state(esphome::media_source::MediaSourceState state) override {
    if (this->orig_ != nullptr) this->orig_->report_state(state);
  }

  void request_volume(float volume) override {
    if (this->orig_ != nullptr) this->orig_->request_volume(volume);
  }

  void request_mute(bool is_muted) override {
    if (this->orig_ != nullptr) this->orig_->request_mute(is_muted);
  }

  void request_play_uri(const std::string &uri) override {
    if (this->orig_ != nullptr) this->orig_->request_play_uri(uri);
  }

 private:
  esphome::media_source::MediaSourceListener *orig_{nullptr};
};

inline void install_sendspin_mono_downmixer(esphome::media_source::MediaSource *src) {
  static SendspinMonoDownmixListener downmixer;
  downmixer.wrap(src);
}

}  // namespace voice_assistant
}  // namespace esphome
#endif
