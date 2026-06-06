/*******************************************************
 *         MVSilicon Audio Effects Process
 *
 *                All Right Reserved
 *******************************************************/

#ifndef __AUDIO_EFFECT_H__
#define __AUDIO_EFFECT_H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

#include "audio_core_api.h"

//音效Apply抽象函数指针
typedef void (*AudioEffectApplyFunc)(void *effectUint, int16_t *pcm_in, int16_t *pcm_out, uint32_t n);

void AudioMusicProcess(AudioCoreContext *pAudioCore);
void AudioBypassProcess(AudioCoreContext *pAudioCore);
void AudioEffectProcessBTHF(AudioCoreContext *pAudioCore);
void AudioNoAppProcess(AudioCoreContext *pAudioCore);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__AUDIO_EFFECT_H__
