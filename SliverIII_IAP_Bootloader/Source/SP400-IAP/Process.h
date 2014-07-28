#ifndef _PORCESS_H_
#define _PORCESS_H_
#include "DELAY.h"
#include "stdio.h"
#include <string.h>

#include "EMSP_API.h"
#include "EMSP.h"
#include "EMW_hal.h"

#include "sdio_sdcard.h"
#include "integer.h"
#include "ff.h"
#include "ffconf.h"
#include "diskio.h"

#include "lcd.h"
#include "flash_hal.h"

#include "Parameter.h"
#include "crc16.h"

typedef enum
{
	KEY_none=0,
	KEY_left,
	KEY_right,
	KEY_ok	
}KEY_Press;


extern u8 isFactory;
extern u8 isAppExist;
extern void _main_Process(void);
extern uint8_t JumpToApplication(uint32_t Addr);
extern BootPrarmeter Bp;
extern SD_Error Status;
extern FATFS fs;
extern FRESULT Frimware;
extern FIL f_bin;
#endif