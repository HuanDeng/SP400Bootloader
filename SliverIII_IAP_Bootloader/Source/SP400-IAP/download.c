
#include "common.h"
#include "Process.h"

extern unsigned char file_name[FILE_NAME_LENGTH];

u32 SerialDownload(u8* fname)
{
  unsigned char Number[10] = "          ";
  int Size = 0;
	Size=YmodemRecvData();
	f_close(&f_bin);
	printf("==============================");
    if (Size > 0)
    {
            fname=file_name;
            printf("File Download Done!\n");
            printf("File Name: %s\n",file_name);
            printf("File Size: %d Byte\n",Size);
          
            if(isFactory)
            {
                  Frimware=f_open(&f_bin,"factory.bin",FA_OPEN_EXISTING);
                  f_close(&f_bin);
                  if(Frimware==FR_NO_FILE)
                  {
                  Frimware=f_rename("new_temp.bin","factory.bin");
                  }
                  else if(Frimware==FR_OK)
                  {
                  Frimware=f_unlink("factory.bin");
                  Frimware=f_rename("new_temp.bin","factory.bin");
                  } 
            }
            else 
            {
                  Frimware=f_open(&f_bin,"new.bin",FA_OPEN_EXISTING);
                  f_close(&f_bin);
                  if(Frimware==FR_NO_FILE)
                  {
                  Frimware=f_rename("new_temp.bin","new.bin");
                  }
                  else if(Frimware==FR_OK)
                  {
                  Frimware=f_unlink("new.bin");
                  Frimware=f_rename("new_temp.bin","new.bin");
                  }  
            }


    }
    else if (Size == -1)
    {
        printf("ERROR CODE -1\n");
    }
    else if (Size == -2)
    {
        printf("ERROR CODE -2\n");
    }
    else if (Size == -3)
    {
        printf("User Exit\n");
    }
    else
    {
        printf("Time Out\n");
    }
		return (u32)Size;
}
