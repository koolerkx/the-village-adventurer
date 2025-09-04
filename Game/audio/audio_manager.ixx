module;
#ifndef CI_BUILD
// CRI Header
#include "cri/cri_le_atom_ex.h"
#include "cri/cri_le_atom_wasapi.h"

// #pragma comment(lib, "../cri_ware_pcx64_le_import.lib")
#pragma comment(lib, "cri_ware_pcx64_le_import.lib")
// D:\Projects\roguelike-topdown-game\Game\cri_ware_pcx64_le_import.lib

export module game.audio.audio_manager;

import std;
import game.types;
export import game.audio.audio_clip;

constexpr const char* ACF_FILE = "assets/audio/top-down-game.acf";
constexpr const char* ACB_FILE = "assets/audio/GameCueSheet_0.acb";

export struct BGM_Player {
  CriAtomExPlayerHn current{nullptr};
  CriAtomExPlayerHn other{nullptr};

  CriAtomExPlaybackId current_playback_id{};
  CriAtomExCueId current_playback_cue_id{};
};

export class AudioManager {
private:
  CriAtomExVoicePoolHn pool_;
  CriAtomExAcbHn acb_hn_;
  CriAtomEx3dListenerHn listener_;

  BGM_Player bgm_player_{nullptr, nullptr};
  CriAtomExPlayerHn walking_player_{nullptr};
  std::array<CriAtomExPlayerHn, 32> audio_clip_players_{nullptr};

public:
  AudioManager();
  ~AudioManager();

  void OnUpdate();
  void OnFixedUpdate();
  void UpdateListenerPosition(Vector2 position = {0, 0});

  CriAtomExPlaybackId PlayAudioClip(audio_clip clip, Vector2 position = {0, 0}, float volume = 1.0f);
  void PlayBGM(audio_clip clip);
  void PlayPreviousBGM();

  void PlayWalking(audio_clip clip = audio_clip::walk_grass);
  void StopWalking();
};

#else

export module game.audio.audio_manager;
export import game.audio.audio_clip;

import game.types;

export class AudioManager {
private:

public:
  AudioManager() {};
  ~AudioManager() {};

  void OnUpdate() {};
  void OnFixedUpdate() {};
  void UpdateListenerPosition(Vector2) {};

  int PlayAudioClip(audio_clip, [[maybe_unused]] Vector2 pos = {}, [[maybe_unused]] float v=0) { return 0; };
  void PlayBGM(audio_clip) {};
  void PlayPreviousBGM() {};

  void PlayWalking(audio_clip = audio_clip::walk_grass) {};
  void StopWalking() {};
};

#endif
