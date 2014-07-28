#include "stm32f10x.h"
#include "Process.h"


#define _DF1S	0x81
SD_CardInfo SDCardInfo;
uint32_t Buffer_Block_Tx[512], Buffer_Block_Rx[512]; 
SD_Error Status = SD_OK;
FATFS fs;                      // �߼��������ı�־

void RCC_Configuration(void);
void keypadInit(void);
void NVIC_Configuration(void);	  
void wifiGpioConfiguration_Sleep(void);
uint8_t JumpToApplication(uint32_t Addr);

char filePath[20];
char fileName[6];

typedef void (*pFunction)(void);
uint8_t JumpToApplication(uint32_t Addr)
{
	pFunction Jump_To_Application;
	__IO uint32_t JumpAddress; 
	u16 i=65535;
	__disable_irq();
	while(i--);
	
	
	/* Test if user code is programmed starting from address "ApplicationAddress" */
	if (((*(__IO uint32_t*)Addr) & 0x2FFE0000 ) == 0x20000000)
	{ 
	  /* Jump to user application */
	  JumpAddress = *(__IO uint32_t*) (Addr + 4);
	  Jump_To_Application = (pFunction) JumpAddress; 
	  /* Initialize user application's Stack Pointer */
	  __set_MSP(*(__IO uint32_t*) Addr);
	  #ifdef USE_USB_PORT
	  USB_Cable_Config(DISABLE);
	  #endif
	  Jump_To_Application();
	}
//	TimeOutFlag=0xFF;
	return 1;
}
__asm void SystemReset(void) 
{ 
 MOV R0, #1            
 MSR FAULTMASK, R0    
 LDR R0, =0xE000ED0C  
 LDR R1, =0x05FA0004   
 STR R1, [R0]             

deadloop 
    B deadloop        
} 


u8 defaul_BootVersion[14]={"v0.0.1"};
BootPrarmeter Bp;
u8 isAppExist=0;
u8 isFSok=1;
int main(void)
{
	int i=0;u8 *p;u16 CRCR;
	SystemInit();
	RCC_Configuration();
	keypadInit();
	wifiGpioConfiguration_Sleep();
	NVIC_Configuration();
	delayByTIM_Init(72);
	
	
	Bp = *(BootPrarmeter *)((u32)BootPrarmeterAddr);
	CRCR=crc16(Bp.bootVersion,14);
	if(CRCR!=((Bp.bootVersion[14]<<8)+Bp.bootVersion[15]))
	{
		memset((void *)&Bp,0,sizeof(Bp));
		for(i=0;i<14;i++)
		{
			Bp.bootVersion[i]=defaul_BootVersion[i];
		}
		CRCR=crc16(defaul_BootVersion,14);
		Bp.bootVersion[14]=(u8)(CRCR>>8);
		Bp.bootVersion[15]=(u8)(CRCR);
		Flash_Write(BootPrarmeterAddr,(u8*)&Bp,sizeof(Bp));
	}
	else if(Bp.isEnterToBootmenu==0&&Bp.isEnterBootLoadToSD==0&&Bp.isEnterBootloadToWIFI==0)
	{
			CRCR=crc16(Bp.appVersion,14);
			if(CRCR==((Bp.appVersion[14]<<8)+Bp.appVersion[15]))//�Ϸ�Ӧ�ó�����
			{
					isAppExist=1;
					if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13))//�������
					{
						JumpToApplication(AppProgramAddr);
					}
			}
	}
	lcdInit();
	printf("******************************");
  printf("SP400 Serial:%d\n",Bp.serialNum);
	printf("SP400 Bootloader:%s\n",Bp.bootVersion);
	printf("SP400 Application:");
	if(isAppExist)
		printf("%s\n",Bp.appVersion);
	else 
		printf("Not Exist\n");	
	printf("Pyxis Lab,Inc.\n");
	printf("******************************");
	while(!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13));
	while(!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12));
	while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2));
	
	Status = SD_Init();
	Status = SD_GetCardInfo(&SDCardInfo);
	Status = SD_SelectDeselect((uint32_t) (SDCardInfo.RCA << 16));
	Status = SD_EnableWideBusOperation(SDIO_BusWide_1b);
	Status = SD_SetDeviceMode(SD_DMA_MODE);  
	if (Status == SD_OK)
	{
			Status = SD_ReadBlock(Buffer_Block_Rx, 0x00,  512); 
	}
	if (Status != SD_OK)
	{
			printf("FATFS ERROR\n");
			while(1);
	}
	else
	{
			f_mount(0, &fs);
	}
	while(1)
	{
		_main_Process();
	}
 }


void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus;
	/* RCC system reset(for debug purpose) */
	RCC_DeInit();												//ʱ�ӿ��ƼĴ���ȫ���ָ�Ĭ��ֵ
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);								//�ⲿ����ʱ��Դ������8M����
	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();				//�ȴ��ⲿʱ�Ӿ���
	if(HSEStartUpStatus == SUCCESS)							//���ʱ�������ɹ�
	{
		/* HCLK = SYSCLK */
		RCC_HCLKConfig(RCC_SYSCLK_Div1);						//����AHB�豸ʱ��Ϊϵͳʱ��1��Ƶ
		/* PCLK2 = HCLK */
		RCC_PCLK2Config(RCC_HCLK_Div1);							//����APB2�豸ʱ��ΪHCLKʱ��1��Ƶ
		/* PCLK1 = HCLK/2 */
		RCC_PCLK1Config(RCC_HCLK_Div2);							//����APB1�豸ʱ��ΪHCLKʱ��2��Ƶ
		/* Flash 2 wait state */
		FLASH_SetLatency(FLASH_Latency_2);						//�趨�ڲ�FLASH�ĵ���ʱ����Ϊ2����
		/* Enable Prefetch Buffer */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);	//ʹ��FLASHԤ��ȡ������
		/* PLLCLK = 8MHz * 9 = 72 MHz */
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);	//����PLLʱ��Ϊ�ⲿ����ʱ�ӵ�9��Ƶ��8MHz * 9 = 72 MHz
		/* Enable PLL */
		RCC_PLLCmd(ENABLE);										//ʹ��PLLʱ��
		/* Wait till PLL is ready */
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)		//�ȴ�PLLʱ���������׼������
		{
		}
		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);				//ʹ��PLLʱ����Ϊϵͳʱ��Դ
		/* Wait till PLL is used as system clock source */
		while(RCC_GetSYSCLKSource() != 0x08)					//����ϵͳ����ʱ��Դȷ��Ϊ�ⲿ���پ���8M����
		{
		}
	}
	/* �豸ʱ�ӿ��� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE |
	RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
 						  	 
}
void keypadInit(void)
{
	GPIO_InitTypeDef gpioStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD |RCC_APB2Periph_AFIO, ENABLE);
	/*************************************
	 * configure GPD pin 12/13,PB2 as input
	 ************************************/
	gpioStruct.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
	gpioStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &gpioStruct);
	gpioStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOB, &gpioStruct);	
}
void wifiGpioConfiguration_Sleep(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOC, ENABLE);
	// PC6 wifi weakup,PC7 wifi status,PA8 wifi nrst;
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);	
}
void NVIC_Configuration(void)				
{
    NVIC_InitTypeDef NVIC_InitStructure;
	/* Set the Vector Table base location at 0x08000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	/* Configure one bit for preemption priority */
  /* ���ȼ���1 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	  
  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;			     			//SDIO�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		 	//�������ȼ�0  ��Χ��0��1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			 			//�����ȼ�0	��Χ��0-7
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

// ��printf(...)�����������LCD
#ifdef __GNUC__ 
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
	static uint16_t x = 0, y = 0;
	if(x > lcdWIDTH - 8)  //X_MAX????, ???240
	{
		x = 0;           //x??
		y += 16;         //y?????
	}
	if(y > lcdHEIGHT - 16)  //Y_MAX????, ???320
	{
		y=0;lcdClear();
	}
	if(ch == '\n')
	{
		x = 0;
		y += 16;
		return ch;
	}
	LCD_Ascii_One(x, y, (int)ch, White);
	x += 8; 
return ch;
}
