/**
 *************************************************************************************
 * @file	blue_plc.h
 * @brief	Packet Loss Concealment (PLC) module
 *
 * @author	ZHAO Ying (Alfred)
 * @version	v0.3.1
 *
 * &copy; Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 *************************************************************************************
 */

#ifndef __BLUE_PLC_H__
#define __BLUE_PLC_H__

#include <stdint.h>

 /** error code for PLC */
typedef enum _BLUEPLC_ERROR_CODE
{
	BLUEPLC_ERROR_UNSUPPORTED_SAMPLE_RATE = -256,
	BLUEPLC_ERROR_UNSUPPORTED_FRAME_SIZE,

	// No Error
	BLUEPLC_ERROR_OK = 0,					/**< no error              */
} BLUEPLC_ERROR_CODE;

#ifdef __cplusplus
extern "C" {
#endif


/**
 * 
 * @brief Estimate the memory usage of PLC module before actual initialization.
 * @param[in]  sample_rate Sample rate in Hz. Supported values: 8000 ~ 48000.
 * @param[in]  frame_size Frame size in samples. Supported values: 20 ~ 960.
 * @param[out] persistent_size  size of persistent memory in bytes. This memory is used to store the context of the PLC module and should not be modified or cleared between calls to blue_plc_bad_frame() or blue_plc_good_frame().
 * @param[out] scratch_size  size of scratch memory in bytes. This memory is used for temporary calculations and can be modified or cleared between calls to blue_plc_bad_frame() or blue_plc_good_frame().
 * @return error code. BLUEPLC_ERROR_OK means successful, other codes indicate error.
 * @note This function is usually called before module initialization to estimate the memory required for the context object and scratch buffer.
 * To achieve decent PLC performance, the frame duration is recommended to be within the range from 2.5ms to 20ms (frame duration in ms = frame_size / sample_rate * 1000).
 */
int32_t blue_plc_estimate_memory_usage(int32_t sample_rate, int32_t frame_size, uint32_t* persistent_size, uint32_t* scratch_size);


/**
 * @brief Initialize the PLC module.
 * @param[in]  ct Pointer to the context object allocated in persistent memory. The size of this memory should be at least the value returned by blue_plc_estimate_memory_usage().
 * @param[in]  scratch Pointer to the scratch memory allocated for temporary calculations. The size of this memory should be at least the value returned by blue_plc_estimate_memory_usage().
 * @param[in]  sample_rate Sample rate in Hz. Supported values: 8000 ~ 48000.
 * @param[in]  frame_size Frame size in samples. Supported values: 20 ~ 960.
 * @return error code. BLUEPLC_ERROR_OK means successful, other codes indicate error.
 * @note This function should be called before using the PLC module. It initializes the context and prepares the module for processing frames.
 */
int32_t blue_plc_init(uint8_t *ct, uint8_t* scratch, int32_t sample_rate, int32_t frame_size);


/**
 * @brief Apply Packet Loss Concealment (PLC) when the frame is broken or lost.
 * @param[in]  ct Pointer to the context object initialized by blue_plc_init().
 * @param[out] s Pointer to the output buffer where the processed samples will be stored.
 * @return error code. BLUEPLC_ERROR_OK means successful, other codes indicate error.
 * @note This function processes a frame of samples when packet loss occurs. It applies the concealment algorithm and fills the output buffer with the processed samples.
 */
int32_t blue_plc_bad_frame(uint8_t *ct, int16_t *s);


/**
 * @brief Add a good frame to the history buffer and output the processed samples.
 * @param[in]  ct Pointer to the context object initialized by blue_plc_init().
 * @param[in,out]  s Pointer to the input/output buffer containing the good frame samples.
 * @return error code. BLUEPLC_ERROR_OK means successful, other codes indicate error.
 * @note This function is called when a good frame is received. 
 * 		 It adds the input frame to the history buffer for future reference and processing. 
 * 		 It may also perform overlap-add with the previous synthetic signal if it follows an erasure. 
 * 		 The output buffer will contain the processed samples after adding the good frame to the history.
 */
int32_t blue_plc_good_frame(uint8_t *ct, int16_t *s);


/**
 * @brief Get the delay introduced by the PLC module.
 * 
 * @param ct Pointer to the context object initialized by blue_plc_init().
 * @return the delay in samples that the PLC module introduces to the signal. 
 */
int32_t blue_plc_get_delay(uint8_t *ct);


#ifdef __cplusplus
}
#endif

#endif  /* __BLUE_PLC_H__ */
