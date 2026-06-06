/**
 *************************************************************************************
 * @file	blue_ns_adv.h
 * @brief	Noise Suppression for Mono Signals with Advanced Control
 *
 * @author	ZHAO Ying (Alfred)
 * @version	v4.1.0
 *
 * &copy; Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 *************************************************************************************
 */

#ifndef __BLUE_NS_ADV_H__
#define __BLUE_NS_ADV_H__

#include <stdint.h>


 /** error code for noise suppression */
typedef enum _BLUENSADV_ERROR_CODE
{
	BLUENSADV_ERROR_ILLEGAL_NS_LEVEL = -256,
	BLUENSADV_ERROR_ILLEGAL_BLOCK_LENGTH,
	BLUENSADV_ERROR_ILLEGAL_BIT_DEPTH,
	BLUENSADV_ERROR_ILLEGAL_SAMPLE_RATE,
	BLUENSADV_ERROR_ILLEGAL_SMOOTH_TIME,
	BLUENSADV_ERROR_ILLEGAL_INITIAL_ENV_TIME,

	// No Error
	BLUENSADV_ERROR_OK = 0,					/**< no error              */
} BLUENSADV_ERROR_CODE;


#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


/**
 * 
 * @brief Estimate the memory usage of noise suppression module before actual initialization.
 * @param[in]  blk_len Block length in samples. 4 supported values: 64 / 128 / 256 / 512.
 * @param[in]  bit_depth Bit depth of the PCM samples. Choose either 16 or 24 bits.
 * @param[out] persistent_size  size of persistent memory in bytes. The contents of this memory cannot be modified or cleared between calls to blue_ns_adv_apply() or blue_ns_adv_apply24().
 * @param[out] scratch_size  size of scratch memory in bytes. The contents of this memory can be modified or cleared for other use between calls to blue_ns_adv_apply() or blue_ns_adv_apply24().
 * @return error code. BLUENSADV_ERROR_OK means successful, other codes indicate error.
 * @note This function is usually called before module initialization to estimate the memory required for the context object.
 * Typical memory usage is as follows:
 * ----------------------------------------------------
 * | block_length | bit_depth | persistent |  scratch |
 * |              |           |   (bytes)  |  (bytes) |
 * ----------------------------------------------------
 * |      64      |    16     |    1584    |    512   |
 * |     128      |    16     |    3120    |   1024   |
 * |     256      |    16     |    6192    |   2048   |
 * |     512      |    16     |   12336    |   4096   |
 * |---------------------------------------------------
 * |      64      |    24     |    1840    |    512   |
 * |     128      |    24     |    3632    |   1024   |
 * |     256      |    24     |    7216    |   2048   |
 * |     512      |    24     |   14384    |   4096   |
 * ----------------------------------------------------
 * The values above are for reference only. The actual values returned by the function should prevail in case of any difference.
 */
int32_t blue_ns_adv_estimate_memory_usage(int32_t blk_len, int32_t bit_depth, uint32_t* persistent_size, uint32_t* scratch_size);


/**
 * @brief Initialize the noise suppression module.
 * @param ct Pointer to the noise suppression context object (persistent_size in bytes).
 * @param scratch Pointer to the scratch area (scratch_size in bytes).
 * @param sample_rate Sample rate.
 * @param initial_env_time Initial environment time in milliseconds. Range: 10~5000.
 * @param smooth_time Smoothing time for noise estimation in milliseconds. Range: 1~32767. 
 *	      Longer time results in smoother noise estimation but slower response to noise level change.
 * @param blk_len Block length in samples. 4 supported values: 64 / 128 / 256 / 512. 
 *        The blk_len selected not only affects the CPU usage but also the effect. 
 *	      Longer block usually has better effect but with longer delay.
 * @param bit_depth Bit depth of the PCM samples. Choose either 16 or 24 bits.
 * @return error code. BLUENSADV_ERROR_OK means successful, other codes indicate error.
 * @note Only mono signals are supported.
 */
int32_t blue_ns_adv_init(uint8_t* ct, uint8_t* scratch, int32_t sample_rate, int32_t initial_env_time, int32_t smooth_time, int32_t blk_len, int32_t bit_depth);


/**
 * @brief Run noise suppression to a block of PCM data (16-bit).
 * @param ct Pointer to the noise suppression context object.
 * @param xin Input PCM data. The size of xin is equal to blk_len set in blue_ns_adv_init().
 * @param xout Output PCM data. The size of xout is equal to blk_len set in blue_ns_adv_init().
 *        xout can be the same as xin. In this case, the PCM is changed in-place.
 * @param ns_level Noise suppression level. Valid range: 0 ~ 30. Use 0 to disable noise suppression while 30 to apply maximum suppression.
 * @return error code. BLUENSADV_ERROR_OK means successful, other codes indicate error.
 * @note Only mono signals are supported.
 */
int32_t blue_ns_adv_apply(uint8_t* ct, int16_t* xin, int16_t* xout, int32_t ns_level);


/**
 * @brief Run noise suppression to a block of PCM data (24-bit).
 * @param ct Pointer to the noise suppression context object.
 * @param xin Input PCM data. The size of xin is equal to blk_len set in blue_ns_adv_init().
 * @param xout Output PCM data. The size of xout is equal to blk_len set in blue_ns_adv_init().
 *        xout can be the same as xin. In this case, the PCM is changed in-place.
 * @param ns_level Noise suppression level. Valid range: 0 ~ 30. Use 0 to disable noise suppression while 30 to apply maximum suppression.
 * @return error code. BLUENSADV_ERROR_OK means successful, other codes indicate error.
 * @note Only mono signals are supported.
 */
int32_t blue_ns_adv_apply24(uint8_t* ct, int32_t* xin, int32_t* xout, int32_t ns_level);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//__BLUE_NS_ADV_H__
