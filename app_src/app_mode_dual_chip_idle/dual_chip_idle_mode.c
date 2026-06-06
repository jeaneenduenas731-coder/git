#include <string.h>
#include "main_task.h"
#include "dma.h"
#include "audio_adc.h"
#include "adc_interface.h"
#include "remind_sound.h"
#include "audio_effect.h"
#include "audio_vol.h"
#include "ctrlvars.h"
#include "reset.h"
#include "uart_cmd.h"

static const uint8_t sDmaChannelMap[6] = {
	//DMA默认配置不要删除，DMA数量不足6个的时候作为填充用
	DMA_CFG_TABLE_DEFAULT_INIT

#ifdef CFG_RES_AUDIO_DAC0_EN
	PERIPHERAL_ID_AUDIO_DAC0_TX,
#endif

#if CFG_RES_MIC_SELECT
	PERIPHERAL_ID_AUDIO_ADC1_RX,
#endif

#ifdef CFG_RES_AUDIO_SPDIFOUT_EN
	SPDIF_OUT_DMA_ID,
#endif

#if (I2S_ALL_DMA_CH_CFG & I2S0_TX_NEED_ENABLE)
	PERIPHERAL_ID_I2S0_TX,
#endif
#if (I2S_ALL_DMA_CH_CFG & I2S1_TX_NEED_ENABLE)
	PERIPHERAL_ID_I2S1_TX,
#endif
#if (I2S_ALL_DMA_CH_CFG & I2S0_RX_NEED_ENABLE)
	PERIPHERAL_ID_I2S0_RX,
#endif
#if (I2S_ALL_DMA_CH_CFG & I2S1_RX_NEED_ENABLE)
	PERIPHERAL_ID_I2S1_RX,
#endif

#ifdef CFG_COMMUNICATION_BY_UART
	CFG_FUNC_COMMUNICATION_TX_DMA_PORT,
	CFG_FUNC_COMMUNICATION_RX_DMA_PORT,
#endif

#ifdef CFG_DUMP_DEBUG_EN
	CFG_DUMP_UART_TX_DMA_CHANNEL,
#endif

	PERIPHERAL_ID_AUDIO_ADC0_RX,
	PERIPHERAL_ID_SDIO_RX,
};


void DualChipIdleRun(uint16_t msgId)
{
	switch(msgId)
	{
		case MSG_PLAY_PAUSE:
			//HardWareMuteOrUnMute();
			SendPlayPauseKeyMsgToSlave();
			break;
		case MSG_PRE:
			APP_DBG("PRE Song\n");
			SendPreKeyMsgToSlave();
			break;

		case MSG_NEXT:
			APP_DBG("next Song\n");
			SendNextKeyMsgToSlave();
			break;

		default:
			CommonMsgProccess(msgId);
			break;
	}
}

bool DualChipIdleInit(void)
{
	bool ret = FALSE;

	APP_DBG("DualChipIdle Init\n");

	DMA_AllChannelClose();
	DMA_ChannelAllocTableSet((uint8_t *)sDmaChannelMap);

	if(!ModeCommonInit())
	{
		ModeCommonDeinit();
		return FALSE;
	}

	//Core Process
#ifdef CFG_FUNC_AUDIO_EFFECT_EN
	AudioCoreProcessConfig((void*)AudioMusicProcess);
#else
	AudioCoreProcessConfig((void*)AudioBypassProcess);
#endif


#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(GetSystemMode() == ModeLeAudioLinein)
		ret = RemindSoundServiceItemRequest(SOUND_REMIND_XIANLUMO, REMIND_ATTR_NEED_MUTE_APP_SOURCE);
	if(GetSystemMode() == ModeLeAudioBT)
		ret = RemindSoundServiceItemRequest(SOUND_REMIND_BTMODE, REMIND_ATTR_NEED_MUTE_APP_SOURCE);
#endif
	if(ret == FALSE)
	{
		if(IsAudioPlayerMute() == TRUE)
		{
			HardWareMuteOrUnMute();
		}
	}
	AudioCoreSourceDisable(APP_SOURCE_NUM);
	Audio_BisDACSwtichChannal(1);
	return TRUE;
}


bool DualChipIdleDeinit(void)
{
	APP_DBG("DualChipIdle Deinit\n");

	PauseAuidoCore();

	//注意：AudioCore父任务调整到mainApp下，此处只关闭AudioCore通道，不关闭任务
	AudioCoreProcessConfig((void*)AudioNoAppProcess);

	ModeCommonDeinit();//通路全部释放
	return TRUE;
}


