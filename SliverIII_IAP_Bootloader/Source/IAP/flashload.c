#include "flash_hal.h"
#include "ff.h"
#include "Parameter.h"
#include "DELAY.h"
#include "stdlib.h"
#include "Process.h"

#define readByteCount 512
FRESULT Frimware;
FIL f_bin;
int numOfread=0;
unsigned char BUFF[readByteCount]=0;
long WriteFlashFromFile(char *Path)
{

	long byteOfload=0;
	uint32_t addr=AppProgramAddr;
	memset(BUFF,readByteCount,0);
	Frimware = f_open( &f_bin , (char *)Path, FA_OPEN_EXISTING | FA_READ);
	if(Frimware==FR_OK)
	{
		while(1)
		{
		   	
		   Frimware=f_read(&f_bin ,BUFF,readByteCount,&numOfread);

		   if(Frimware!=FR_OK)
		   {
		   		return -1;
		   }
				byteOfload+=Flash_Write(addr,BUFF,numOfread);
		   addr+=numOfread;
		   if(numOfread<readByteCount)
		   {
		   		break;
		   }
		}
		
	}
	else return -1;

	return byteOfload;
	
}