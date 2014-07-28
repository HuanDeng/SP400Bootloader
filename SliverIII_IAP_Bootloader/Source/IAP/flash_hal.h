
#ifndef _FLASH_HAL_H_
#define _FLASH_HAL_H_
#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include "Parameter.h"

#define FLASH_PAGE_SIZE    (0x800)

 



int Flash_Read(uint32_t iAddress, uint8_t *buf, int32_t iNbrToRead) ;
int Flash_Write(uint32_t iAddress, uint8_t *buf, uint32_t iNbrToWrite);
 

#endif
