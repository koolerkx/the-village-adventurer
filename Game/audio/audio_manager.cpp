module;
#include <__msvc_ostream.hpp>

#include "cri/cri_le_xpt.h"
#include "cri/cri_le_atom_ex.h"
#include "cri/cri_le_atom_wasapi.h"

module game.audio.audio_manager;

import std;

namespace {
  static void user_error_callback_func(const CriChar8* errid, CriUint32 p1, CriUint32 p2, CriUint32* parray) {
    /* エラー文字列の表示 */
    const CriChar8* error_message = criErr_ConvertIdToMessage(errid, p1, p2);
    std::cout << error_message << std::endl;
  }

  void* user_alloc_func(void* obj, const CriUint32 size) {
    void* ptr = malloc(size);
    return ptr;
  }

  void user_free_func(void* obj, void* ptr) {
    free(ptr);
  }
}

AudioManager::AudioManager() {
  criErr_SetCallback(user_error_callback_func);
  criAtomEx_SetUserAllocator(user_alloc_func, user_free_func, nullptr);

  CriAtomExConfig_WASAPI ex_cfg{};
  criAtomEx_SetDefaultConfig_WASAPI(&ex_cfg);
  criAtomEx_Initialize_WASAPI(&ex_cfg, nullptr, 0);

  criAtomEx_RegisterAcfFile(nullptr, ACF_FILE, nullptr, 0);
  acb_hn_ = criAtomExAcb_LoadAcbFile(nullptr, ACB_FILE, nullptr, nullptr, nullptr, 0);

  CriAtomExStandardVoicePoolConfig vp_cfg{};
  criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&vp_cfg);
  pool_ = criAtomExVoicePool_AllocateStandardVoicePool(&vp_cfg, nullptr, 0);

  // Create Player
  for (auto& player : audio_clip_players_) {
    player = criAtomExPlayer_Create(nullptr, nullptr, 0);
  }

  static constexpr int crossfade_ms = 3000;
  bgm_player_.current = criAtomExPlayer_Create(nullptr, nullptr, 0);
  criAtomExPlayer_AttachFader(bgm_player_.current, nullptr, nullptr, 0);
  criAtomExPlayer_SetFadeInTime(bgm_player_.current, crossfade_ms);
  criAtomExPlayer_SetFadeOutTime(bgm_player_.current, crossfade_ms);

  bgm_player_.other = criAtomExPlayer_Create(nullptr, nullptr, 0);
  criAtomExPlayer_AttachFader(bgm_player_.other, nullptr, nullptr, 0);
  criAtomExPlayer_SetFadeInTime(bgm_player_.other, crossfade_ms);
  criAtomExPlayer_SetFadeOutTime(bgm_player_.other, crossfade_ms);
}

AudioManager::~AudioManager() {
  for (auto& player : audio_clip_players_) {
    criAtomExPlayer_Destroy(player);
  }
  criAtomExPlayer_Destroy(bgm_player_.current);
  criAtomExPlayer_Destroy(bgm_player_.other);

  criAtomExVoicePool_Free(pool_);
  criAtomExAcb_Release(acb_hn_);
  criAtomEx_UnregisterAcf();
  criAtomEx_Finalize_WASAPI();
}

void AudioManager::OnUpdate() {}

void AudioManager::OnFixedUpdate() {
  criAtomEx_ExecuteMain();

  CriAtomExPlayerStatus s = criAtomExPlayer_GetStatus(bgm_player_.current);

  if (criAtomExPlayer_GetStatus(bgm_player_.current) == CRIATOMEXPLAYER_STATUS_PLAYEND) {
    criAtomExPlayer_SetCueId(bgm_player_.current, acb_hn_, bgm_player_.current_playback_cue_id);
    bgm_player_.current_playback_id = criAtomExPlayer_Start(bgm_player_.current);
  }
}

CriAtomExPlaybackId AudioManager::PlayAudioClip(audio_clip clip) {
  CriAtomExCueId cue_sheet_id = static_cast<CriAtomExCueId>(clip);

  CriAtomExPlaybackId playback_id = CRIATOMEX_INVALID_PLAYBACK_ID;
  for (auto& player : audio_clip_players_) {
    if (criAtomExPlayer_GetStatus(player) == CRIATOMEXPLAYER_STATUS_STOP) {
      criAtomExPlayer_SetCueId(player, acb_hn_, cue_sheet_id);
      playback_id = criAtomExPlayer_Start(player);
      break;
    }
  }

  return playback_id;
}

void AudioManager::PlayBGM(audio_clip clip) {
  std::swap(bgm_player_.current, bgm_player_.other);

  CriAtomExCueId cue_sheet_id = static_cast<CriAtomExCueId>(clip);
  criAtomExPlayer_SetCueId(bgm_player_.current, acb_hn_, cue_sheet_id);

  bgm_player_.current_playback_cue_id = cue_sheet_id;
  bgm_player_.current_playback_id = criAtomExPlayer_Start(bgm_player_.current);
  criAtomExPlayer_Stop(bgm_player_.other);
}
