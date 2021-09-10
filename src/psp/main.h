//============================================================
//
//	psp_main.h - PSP main
//
//============================================================

#ifndef _PSP_MAIN_H_
#define _PSP_MAIN_H_

#include "ticker.h"
//#include "psp_file.h"
#include "psp_font.h"
#include "sound.h"
#include "psp_video.h"
#include "menu.h"

#ifndef MAX_PATH
#define MAX_PATH 0x108
#endif

extern volatile int psp_loop;
extern volatile int psp_sleep;
extern volatile int psp_rajar;

#define HANKAKU10_OFFS 0x00
extern const unsigned char hankaku_font5x10[];
extern const unsigned short zenkaku_font10[];


/* 1 USE_PSP_SLEEP むむむ、本当は１なんだケド。。。 */
#define USE_PSP_SLEEP 0




#endif /* _PSP_MAIN_H_ */
