

#include "lcd.h"
#include "nAsciidot.h"


//Base Addr for LCD_FSMC
#define Bank1_LCD_D    ((u32)0x60020000)    //Disp Data ADDR
#define Bank1_LCD_C    ((u32)0x60000000)	   //Disp Reg ADDR

#define DefaultBackLight 999 								// 500/1000*100%=PWM_Duty
void lcdConfiguration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  p; 
		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//Enable TIM3 RCC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);  //Enable PORTB RCC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//Enable TIM3 AF RCC

	//lcd BackLight  PB0   TIM3_ch3 PWM 
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	//TIM3_ch3 Setup
	// Time base configuration 		 
	TIM_TimeBaseStructure.TIM_Period = 999;       			//0-1000
	TIM_TimeBaseStructure.TIM_Prescaler = 4;	    
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	//PWM1 Mode configuration: Channel3 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; ;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = DefaultBackLight;		// LCD BackLight PWM_Duty
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);	 			//Init TIM3_Ch3
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);		//Enable TIM3_Ch3
	TIM_ARRPreloadConfig(TIM3, ENABLE);			 			// Enable ARR
	TIM_Cmd(TIM3, ENABLE);                   				//Enable TIM3

	//FSMC Setup
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    //config lcd gpio clock base on FSMC 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //config tft rst gpio 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 	 
    GPIO_Init(GPIOE, &GPIO_InitStructure);
	//config tft data lines base on FSMC
	//data lines,FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
    //config tft control lines base on FSMC
	//PD4-FSMC_NOE  :LCD-RD
   	//PD5-FSMC_NWE  :LCD-WR
	// PD7-FSMC_NE1  :LCD-CS
	// PD11-FSMC_A16 :LCD-DC

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  

    GPIO_ResetBits(GPIOE, GPIO_Pin_1);	 					//	RST = 1   
    GPIO_SetBits(GPIOD, GPIO_Pin_4);		 				// RD = 1  
    GPIO_SetBits(GPIOD, GPIO_Pin_5);		 				// WR = 1 
    GPIO_SetBits(GPIOD, GPIO_Pin_7);		 				//	CS = 1

    p.FSMC_AddressSetupTime = 0x01;	 
    p.FSMC_AddressHoldTime = 0x00;	 
    p.FSMC_DataSetupTime = 0x02;		 
    p.FSMC_BusTurnAroundDuration = 0x00;
    p.FSMC_CLKDivision = 0x00;
    p.FSMC_DataLatency = 0x00;
    p.FSMC_AccessMode = FSMC_AccessMode_B;	 				// Mode_B for LCD
    
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;  
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
    //Enable FSMC Bank1_SRAM Bank 
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
		
}

static void Delay(__IO u32 nCount)
{
    for(; nCount != 0; nCount--);
}
static void LCD_Rst(void)
{			
    GPIO_ResetBits(GPIOE, GPIO_Pin_1);
    Delay(0xFFFFf);					   
    GPIO_SetBits(GPIOE, GPIO_Pin_1 );		 	 
    Delay(0xFFFFf);	
}
static __inline void LCD_WR_REG(u16 index)
{
    *(__IO u16 *) (Bank1_LCD_C) = index;
}
static __inline void LCD_WR_CMD(u16 index, u16 val)
{	
    *(__IO u16 *) (Bank1_LCD_C) = index;	
    *(__IO u16 *) (Bank1_LCD_D) = val;
}
static __inline void LCD_WR_Data(unsigned int val)
{   
    *(__IO u16 *) (Bank1_LCD_D) = val; 	
}
static __inline u16 LCD_RD_data(void)
{
    u16 a = 0;
    a = (*(__IO u16 *) (Bank1_LCD_D)); 	//Dummy	
    a = *(__IO u16 *) (Bank1_LCD_D);     //L
    
    return(a);	
}
static u16 RGB(u8 R,u8 G,u8 B)
{	
    return ( ( (u8)R<<11 ) | ( (u8)G<<5 & 0X07E0 ) | ( (u8)(B&0X1F) ) );
}

//********************************************************************************************************************************************************************/

#define RGB2COLOR(r, g, b) 			((((r>>3)<<11) | ((g>>2)<<5) | (b>>3))) 
#define COLOR2RGB(color)    		( ( Nat16) (                                      \
                                             (                                    \
                                                 ( ( color & 0x00F80000 ) >> 8 )  \
                                               | ( ( color & 0x0000FC00 ) >> 5 )  \
                                               | ( ( color & 0x000000F8 ) >> 3 )  \
                                             )                                    \
                                           )                                      \
                                )
																

void MainLCD_Init(void)
{
	long i;
	LCD_Rst();
	LCD_WR_REG(0X0000);
	LCD_WR_REG(0X0000);
	LCD_WR_REG(0X0000);
	LCD_WR_REG(0X0000);
	LCD_WR_CMD(0X00A4,0X0001);
	LCD_WR_CMD(0x0060,0x2700);
	LCD_WR_CMD(0x0008,0x0808); 
	//γ Control
	LCD_WR_CMD(0X0030,0X0500);
	LCD_WR_CMD(0X0031,0X7715);
	LCD_WR_CMD(0X0032,0X0803);
	LCD_WR_CMD(0X0033,0X080a);
	LCD_WR_CMD(0X0034,0X0000);
	LCD_WR_CMD(0X0035,0X0a08);
	LCD_WR_CMD(0X0036,0X7308);
	LCD_WR_CMD(0X0037,0X1507);
	LCD_WR_CMD(0X0038,0X0009);
	LCD_WR_CMD(0X0039,0X3333);

	LCD_WR_CMD(0x0090,0x0017);
	LCD_WR_CMD(0X000a,0X0008);
	//delay_ms(20);
	LCD_WR_CMD(0X0010,0X0430);
	//delay_ms(100);
	LCD_WR_CMD(0X0011,0X0247);
    //delay_ms(100);
	LCD_WR_CMD(0x000E,0x0030);
	//delay_ms(20);
	LCD_WR_CMD(0x0013,0x1600);
	//delay_ms(20);
	LCD_WR_CMD(0x002A,0x006b);
	LCD_WR_CMD(0x0012,0x019a);
	//delay_ms(50);
	LCD_WR_CMD(0x0012,0x01bf);
	//delay_ms(200);
	//Window address control instruction
	LCD_WR_CMD(0x0050,0x0000);
	LCD_WR_CMD(0x0051,0x00EF);
	LCD_WR_CMD(0x0052,0x0000);
	LCD_WR_CMD(0x0053,0x013F);
	//Base image display control instruction
	LCD_WR_CMD(0x0061,0x0001);
	LCD_WR_CMD(0x006A,0x0000);
	//Partial display control instruction
	LCD_WR_CMD(0x0080,0x0000);
	LCD_WR_CMD(0x0081,0x0000);
	LCD_WR_CMD(0x0082,0x0000);
	//Panel interface control instruction 
	LCD_WR_CMD(0x0092,0x0300);
	LCD_WR_CMD(0x0093,0x0503);
	LCD_WR_CMD(0x0091,0x0005);
	LCD_WR_CMD(0x0094,0x0003);
	
	LCD_WR_CMD(0x0001,0x0100);
	//delay_ms(20); 
	LCD_WR_CMD(0x0002,0x0200);
	//delay_ms(20);
	LCD_WR_CMD(0x0003,0X1000); 	//旋转	180
	//delay_ms(20);
	LCD_WR_CMD(0x000C,0x0000);
	LCD_WR_CMD(0x000F,0x0000);
	//RAM access instruction
	LCD_WR_CMD(0x0020,0x0000);
	LCD_WR_CMD(0x0021,0x0000);
	LCD_WR_CMD(0X0007,0X0100);
	LCD_WR_CMD(0x0021,0);        //行首址
	LCD_WR_CMD(0x0020,0);
	LCD_WR_REG(0x0022);
	for(i=0;i<76800;i++)
	{
		LCD_WR_Data(Red);
	}
}
void Set_ramaddr(u16 x,u16 y)
{
	x=239-x;
	y=319-y;
	LCD_WR_CMD(0x0021,y);        //行首址
	LCD_WR_CMD(0x0020,x);        //列首址
}
void DispOneColor(u16 Color)
{   
	int i,j;
	Set_ramaddr(0,0);        //
	LCD_WR_REG(0x22);
	for(j=0;j<320;j++)
		for(i=0;i<240;i++)
			LCD_WR_Data(Color);
}
void LcdDrawDot(u16 x,u16 y,u16 color)
{
	Set_ramaddr(x,y);
	LCD_WR_REG(0x22);
	LCD_WR_Data(color);		
}
void LCD_open_windows(u8 x,u16 y,u8 len,u16 wid)
{   
			x=239-x;
			y=319-y;	
    LCD_WR_CMD(0x0050, x-len+1); 
    LCD_WR_CMD(0x0051, x); 
    LCD_WR_CMD(0x0052, y-wid+1); 
    LCD_WR_CMD(0x0053, y);
	
    LCD_WR_REG(0X22);    
}
void LCD_DrawPic(u16 x,u16 y,u16 width,u16 hight,u8 *pic)
{
	int i;u16 temp;
	Set_ramaddr(x,y);
	LCD_open_windows(x,y,width,hight);
	for(i=0;i<width*hight;i++)
	{	
    temp=(*pic<<8)+(*(pic+1));
		LCD_WR_Data(temp);
		pic+=2;
	}
	LCD_open_windows(0,0,240,320);
}
/**********************************************************/
void lcdFill(u16 x, u16 y, u16 width, u16 hight, u16 color)
{
	int i;
	Set_ramaddr(x,y);
	LCD_open_windows(x,y,width,hight);
	for(i=0;i<width*hight;i++)LCD_WR_Data(color);
	LCD_open_windows(0,0,240,320);
}

void lcdClear(u16 color)
{
	lcdFill(0,0,240,320,color);
}
void LCD_Ascii_One(uint8_t x, uint16_t y, char Char, uint16_t Color)
{
	unsigned char i = 0,j=0;
	unsigned int temp = y;
	unsigned int padd= 0;
	padd = (Char-32)*16;
	y=temp;
	for(i=0;i<16;i++)
	{
		Set_ramaddr(x,y);
		LCD_WR_REG(0x22);
		for(j=0;j<8;j++)
		{
			if(ascii_8x16[padd]&(0x80>>j))
			{
				LCD_WR_Data(Color);
			}
			else
			{
				LCD_WR_Data(Black);
			}
		}
		padd++;
		y++;
	}
}
void showstring(unsigned char x, unsigned char y,u16 fcolor,u16 bcolor,char *str)
{
    unsigned char i = 0,j=0;
    unsigned char temp = y;
    unsigned int padd= 0;

    while(*str != '\0')
    {
        
        padd = (*str-32)*16;
				y=temp;
				for(i=0;i<16;i++)
				{
					Set_ramaddr(x,y);
					LCD_WR_REG(0x22);
					for(j=0;j<8;j++)
					{
						if(ascii_8x16[padd]&(0x80>>j))
						{
							LCD_WR_Data(fcolor);
						}
						else
						{
							LCD_WR_Data(bcolor);
						}
					}
					padd++;
					y++;
				}
				x+=8;
				str++;
   }
}
void lcdInit(void)
{
	int i;
  lcdConfiguration();
	MainLCD_Init();
	LCD_open_windows(0,0,240,320);
	lcdClear(Black);
	//showstring(10,10,White,Black,"Acddegg");
}

