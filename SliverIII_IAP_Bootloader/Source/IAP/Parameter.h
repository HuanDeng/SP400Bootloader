
#ifndef   _PARAMETER_H_
#define    _PARAMETER_H_
#include "stm32f10x.h"

#define Flash_PAGSIZE 0x800
#define Flash_PAGNUM 0xFF

#define BootPrarmeterPagSite 25
#define AppProgramPagSite 26

#define BootPrarmeterAddr 0x8000000+(0x800*(BootPrarmeterPagSite-1))
#define AppProgramAddr 0x8000000+(0x800*(AppProgramPagSite-1))

#define ApplicationAddress 0x8000000+(0x800*(AppProgramPagSite-1))//0x800C800

typedef struct 
{
	u16 t420di;
	u16 s420di;
	u16 t365di;
	u16 s365di;
	
	u16 ct420;
	u16 cs420;
	u16 ct365;
	u16 cs365;
	
	float ptsaf1;
	float ptsaf2;
	float ptsaf3;
	
	float tempt1;
	float tempt2;
	
	float condt1;
	float condt2;
}CalibParameter;
 
typedef struct
{
	u8 bootVersion[16];
	u8 appVersion[16];//应用程序版本号，也作为应用程序合法性判断
	u8 isEnterToBootmenu;
	u8 isEnterBootloadToWIFI;
	u8 isEnterBootLoadToSD;
	CalibParameter calibp;
	u16 serialNum;
	
}BootPrarmeter;



#endif