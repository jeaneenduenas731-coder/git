/*************************************************************************************
 * @file	sbc_plc96.h
 * @brief	Packet Loss Concealment for SBC frame size = 96 @ 48kHz
 *
 * @author	Zhao Ying (Alfred)
 * @version	v1.8.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 *************************************************************************************/

#ifndef SBC_PLC96_H
#define SBC_PLC96_H
#define FS 96 /* Frame Size */
#define N 768 /* 16ms - Window Length for pattern matching */
#define M 192 /* 4ms - Template for matching */
//#define LHIST (N+FS-1) /* Length of history buffer required */
#define LHIST (N+FS+M) /* Length of history buffer required */
//#define SBCRT 36 /* SBC Reconvergence Time (samples) */
//#define OLAL 16 /* OverLap-Add Length (samples) */
#define SBCRT 24 /* SBC Reconvergence Time (samples) */
#define OLAL (FS-SBCRT) /* OverLap-Add Length (samples) */

//#define SBCRT_MIN 30 /* SBC Reconvergence Time (samples) minimum */
//#define SBCRT (FS-M) /* SBC Reconvergence Time (samples) */
//#define OLAL M /* OverLap-Add Length (samples) */
/* PLC State Information */
struct PLC_State96
{
short hist[LHIST+FS+SBCRT+OLAL];
short bestlag;
int nbf;
};

/* Prototypes */
void InitPLC96(struct PLC_State96 *plc_state);
void PLC_bad_frame96(struct PLC_State96 *plc_state, short *ZIRbuf, short *out);
void PLC_good_frame96(struct PLC_State96 *plc_state, short *in, short *out);
#endif /* SBC_PLC96_H */
