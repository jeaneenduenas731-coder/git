/*************************************************************************************
 * @file	sbc_plc64.h
 * @brief	Packet Loss Concealment for SBC frame size = 64 @ 48kHz
 *
 * @author	Zhao Ying (Alfred)
 * @version	v1.6.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 *************************************************************************************/


#ifndef SBC_PLC64_H
#define SBC_PLC64_H
#define FS 64 /* Frame Size */
// #define N 256 /* 16ms - Window Length for pattern matching */
// #define M 64 /* 4ms - Template for matching */
#define N 768 /* 16ms - Window Length for pattern matching */
// #define N 1920 /* 40ms - Window Length for pattern matching */
#define M 192 /* 4ms - Template for matching */
// #define LHIST (N+FS-1) /* Length of history buffer required */
#define LHIST (N + FS + M) /* Length of history buffer required */
// #define SBCRT 36 /* SBC Reconvergence Time (samples) */
// #define OLAL 16 /* OverLap-Add Length (samples) */
#define SBCRT 16 /* SBC Reconvergence Time (samples) */
// #define OLAL 16 /* OverLap-Add Length (samples) */
#define OLAL (FS - SBCRT) /* OverLap-Add Length (samples) */

// #define SBCRT_MIN 30 /* SBC Reconvergence Time (samples) minimum */
// #define SBCRT (FS-M) /* SBC Reconvergence Time (samples) */
// #define OLAL M /* OverLap-Add Length (samples) */

/* PLC State Information */
struct PLC_State64
{
    short hist[LHIST + FS + SBCRT + OLAL];
    short bestlag;
    int nbf;
};

/* Prototypes */
void InitPLC64(struct PLC_State64 *plc_state);
void PLC_bad_frame64(struct PLC_State64 *plc_state, short *ZIRbuf, short *out);
void PLC_good_frame64(struct PLC_State64 *plc_state, short *in, short *out);

#endif /* SBC_PLC64_H */
