#include "Process.h"
#include "Ymodel.h"
extern BootPrarmeter Bp;
void clearAppVersion()
{
	memset(Bp.appVersion,0,16);
	Flash_Write(BootPrarmeterAddr,(u8*)&Bp,sizeof(Bp));
}

KEY_Press readKey()
{
	KEY_Press kp=KEY_none;
	while(kp==KEY_none)
	{
		if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2))kp=KEY_ok;
		if(!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13))kp=KEY_left;
		if(!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12))kp=KEY_right;
	}
	return kp;
}
EMW_parm_TypeDef wifi_paraG;
s8 rut=0;
void _main_Process(void)
{
		u8 *fname;
		USART_InitTypeDef USART_InitStruct;
		u8 temp;u16 recvLenth=0;u32 savelenth=0;
		KEY_Press kp=KEY_none;char ssid[30]={"SP400-"};char buf[20]={0};
		if(isAppExist)
                 printf("|OK|--Boot to Application\n");
            else printf("|OK|--Load new ver from SD\n");
		     printf("| <|--Load factory ver from SD");
		     printf("| >|--Update form PC\n");
		     printf("******************************");

		kp=readKey();
		if(kp==KEY_ok)
		{
                  if( isAppExist)JumpToApplication(AppProgramAddr);
                  else 
                  {
                        Frimware=f_open(&f_bin,"new.bin",FA_OPEN_EXISTING);
                        f_close(&f_bin);
                        if(Frimware==FR_OK)
                        {
                              clearAppVersion();
                              printf("Firmware Loading......\n");
                              printf("%d Bytes to load\n",WriteFlashFromFile("new.bin"));
                              printf("Jump To Application\n");
                              JumpToApplication(AppProgramAddr);
                        }
                        else
                        {
                                    printf("Can not found file\n");
                        }   
                  }
		}
		else if(kp==KEY_left)
		{
			Frimware=f_open(&f_bin,"factory.bin",FA_OPEN_EXISTING);
                  f_close(&f_bin);
			if(Frimware==FR_OK)
			{
				clearAppVersion();
				printf("Firmware Loading......\n");
				printf("%d Bytes to load\n",WriteFlashFromFile("factory.bin"));
				printf("Jump To Application\n");
				JumpToApplication(AppProgramAddr);
			}
			else
			{
					printf("Can not found file\n");
			}

		}
		else if(kp==KEY_right)
		{
			printf("Starting Wifi...\n");
			EMW_HAL_InitWithDefault();
			EMW_Get_Config(&wifi_paraG);
			memset(wifi_paraG.wifi_ssid,0,sizeof(wifi_paraG.wifi_ssid));
			sprintf(buf,"%d",Bp.serialNum);
			strcat(ssid,buf);
			strcat(wifi_paraG.wifi_ssid,ssid);
			wifi_paraG.use_dhcp=1;
			EMW_Set_Config(&wifi_paraG);
			EMW_Set_Mode(DTU_mode);
			printf("WIFI Network: %s Created\n",wifi_paraG.wifi_ssid);
			rut=ReciveFile2SD(fname);
			SysTick->CTRL=0;
			if(rut)
			{
				clearAppVersion();
				printf("Firmware Loading......\n");
                        if(isFactory)
                        {
                            printf("%d Bytes to load\n",WriteFlashFromFile("factory.bin"));
                        }
                        else 
                        {
                            printf("%d Bytes to load\n",WriteFlashFromFile("new.bin"));  
                        }
				
				printf("Jump To Application\n");
				JumpToApplication(AppProgramAddr);
			}
			else
			{
				SystemReset();
			}
		}
		while(1);
}