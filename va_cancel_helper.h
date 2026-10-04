#pragma once
#include "esphome.h"

// Template accessor technique to safely access protected members of final VoiceAssistant
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

inline void abort_voice_assistant(VoiceAssistant *va) {
  if (va != nullptr) {
    va->request_stop();
    va->reset_conversation_id();

    // Directly reset buffers and flags via template accessor
    va->*get(VA_BufSize_Tag()) = 0;
    va->*get(VA_BufIndex_Tag()) = 0;
    va->*get(VA_BytesReceived_Tag()) = 0;
    va->*get(VA_WaitForStreamEnd_Tag()) = false;
    va->*get(VA_StreamEnded_Tag()) = false;
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

} // namespace voice_assistant
} // namespace esphome
