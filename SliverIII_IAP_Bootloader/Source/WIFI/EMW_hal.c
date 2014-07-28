#include "EMW_hal.h"
#include "EMSP_API.h"
#include "xmodem.h"


int buff_pos;
u8 EM380RxBuffer[256] = {0};
bool use_hardware_flow_control;
vu32 pulse;
uint32_t rxCount = 0; 
extern EMW_mode_TypeDef  EM380C_mode;

uint32_t UART_BaudRate[]={9600,19200,38400,57600,115200,230400,460800,921600};
uint16_t UART_WordLength[]={USART_WordLength_8b,USART_WordLength_9b};
uint16_t UART_StopBits[]={USART_StopBits_1,USART_StopBits_0_5,USART_StopBits_2,USART_StopBits_1_5};
uint16_t UART_Parity[]={USART_Parity_No,USART_Parity_Even,USART_Parity_Odd};
uint16_t UART_CTSRTS[]={USART_HardwareFlowControl_None,USART_HardwareFlowControl_RTS_CTS};

static __IO uint32_t TimingDelay;
void Delay(__IO uint32_t nTime);

void SET_STATUS_PIN(void)
{
  GPIO_SetBits(status_port, status_pin);
}
void RESET_STATUS_PIN(void)
{
  GPIO_ResetBits(status_port, status_pin);
}		   

void SET_WAKEUP_PIN(void)
{
  GPIO_SetBits(wakeup_port, wakeup_pin);
}
void RESET_WAKEUP_PIN(void)
{
  GPIO_ResetBits(wakeup_port, wakeup_pin);
}

void SET_RESET_PIN(void)
{
  GPIO_SetBits(reset_port, reset_pin);
}
void RESET_RESET_PIN(void)
{
  GPIO_ResetBits(reset_port, reset_pin);
}

vs8 READ_INT_PIN(void)
{
   //return(GPIO_ReadInputDataBit(int_port, int_pin));
	return 0;
}

u8 EMW_HAL_Init(uint32_t BaudRate,uint16_t WordLength,uint16_t StopBits,uint16_t Parity,uint16_t CTSRTS)
{  	
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;
   vs8 ret = -1;
   buff_pos = 0;
   EM380C_mode = CMD_mode;

   RCC_APB2PeriphClockCmd(RCC_GPIO, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_UART , ENABLE);


  /* Configure USART */
  GPIO_InitStructure.GPIO_Pin = UART_rx_pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(UART_rx_port, &GPIO_InitStructure);		  /* Configure USART RX PIN*/
  
  GPIO_InitStructure.GPIO_Pin = UART_tx_pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(UART_tx_port, &GPIO_InitStructure);		 /* Configure USART TX PIN*/

  if(CTSRTS)
  {
//	  GPIO_InitStructure.GPIO_Pin = UART_cts_pin;
//	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	  GPIO_Init(UART_cts_port, &GPIO_InitStructure);		  /* Configure USART CTS PIN*/
//	  
//	  GPIO_InitStructure.GPIO_Pin = UART_rts_pin;
//	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	  GPIO_Init(UART_rts_port, &GPIO_InitStructure);		 /* Configure USART RTS PIN*/
  }

  /* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  /* Enable the USARTy Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = uart_irq;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  /* Configure Status PIN*/
  GPIO_InitStructure.GPIO_Pin = status_pin ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(status_port, &GPIO_InitStructure);		
  /* Configure Wakeup PIN*/
  GPIO_InitStructure.GPIO_Pin = wakeup_pin ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(wakeup_port, &GPIO_InitStructure);		 
  /* Configure /RESET(PE9) PIN*/
  GPIO_InitStructure.GPIO_Pin = reset_pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(reset_port, &GPIO_InitStructure);		 
  /* Configure /INT PIN*/
//  GPIO_InitStructure.GPIO_Pin = int_pin;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  GPIO_Init(int_port, &GPIO_InitStructure);		 
  /* Run in configuration mode*/
  RESET_STATUS_PIN();
  /* WAKEUP pin should be high*/		    
  SET_WAKEUP_PIN();				
  /* Reset*/  
  RESET_RESET_PIN();
  Delay(1);
  SET_RESET_PIN();
  Delay(1500);
//  Delay(20);
//  while(READ_INT_PIN());

  /* Configure the UART interface*/
  USART_InitStructure.USART_BaudRate = UART_BaudRate[BaudRate];
  USART_InitStructure.USART_WordLength = UART_WordLength[WordLength];
  USART_InitStructure.USART_StopBits = UART_StopBits[StopBits];
  USART_InitStructure.USART_Parity = UART_Parity[Parity];
  USART_InitStructure.USART_HardwareFlowControl = UART_CTSRTS[CTSRTS];
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		 
  USART_Init(uartx, &USART_InitStructure);
  /* ENABLE USART interrupt*/
  USART_ITConfig(uartx, USART_IT_RXNE, ENABLE);
  /* ENABLE USART*/
  USART_Cmd(uartx, ENABLE);											 

  return EMW_SUCCESS;
}

u8 EMW_HAL_InitWithDefault(void)
{  	
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;
   vs8 ret = -1,i;
   buff_pos = 0;

   EM380C_mode = CMD_mode;
   RCC_APB2PeriphClockCmd(RCC_GPIO, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_UART , ENABLE);


  /* Configure USART */
  GPIO_InitStructure.GPIO_Pin = UART_rx_pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(UART_rx_port, &GPIO_InitStructure);		  /* Configure USART RX PIN*/
  
  GPIO_InitStructure.GPIO_Pin = UART_tx_pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(UART_tx_port, &GPIO_InitStructure);		 /* Configure USART TX PIN*/

  /* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  
  /* Enable the USARTy Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = uart_irq;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		 /* Configure USART parameters*/
  USART_Init(uartx, &USART_InitStructure);

  USART_ITConfig(uartx, USART_IT_RXNE, ENABLE);

  USART_Cmd(uartx, ENABLE);											 /* ENABLE USART*/

  /* Configure Status PIN*/
  GPIO_InitStructure.GPIO_Pin = status_pin ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(status_port, &GPIO_InitStructure);		
  /* Configure Wakeup PIN*/
  GPIO_InitStructure.GPIO_Pin = wakeup_pin ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(wakeup_port, &GPIO_InitStructure);		 
  /* Configure /RESET(PE9) PIN*/
  GPIO_InitStructure.GPIO_Pin = reset_pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  SET_RESET_PIN();
  GPIO_Init(reset_port, &GPIO_InitStructure);		 

  RESET_STATUS_PIN();		    /* EM380C start in configuration mode*/
  SET_WAKEUP_PIN();				/* WAKEUP pin should be high*/ 
	//SET_STATUS_PIN();  

  /* Reset*/  
  RESET_RESET_PIN();
  Delay(1);
  SET_RESET_PIN();
  Delay(800);

  for(i=0;i<4;i++)
  {
  USART_SendData(uartx, 0x20);
    Delay(100); }
  Delay(300);
		//while(1);
  while(READ_INT_PIN());
  return EMW_SUCCESS;
}

void UART_send_buf(u8 *buf, int len)
{
	int i; 
	buff_pos=0;
	memset(EM380RxBuffer, 0x0, 256);   
	for (i = 0; i < len; i++)
	  {
       USART_SendData(uartx,buf[i]);
	   while(!USART_GetFlagStatus(uartx,USART_FLAG_TXE));
	  }	
}

int UART_receive_buf(u8 *buf)
{   memcpy(buf, EM380RxBuffer, buff_pos);
	return buff_pos;
}


u8 rxBuf;
u8 isRecv=0;
void USART_IRQHandler(void){

	if(USART_GetITStatus(uartx, USART_IT_RXNE) == SET)
	{
	  if(EM380C_mode == DTU_mode)
		{
				rxBuf=USART_ReceiveData(uartx);
				isRecv=1;
	  }
	  if(EM380C_mode == CMD_mode)
		{
				/* Read one byte from the receive data register */
				EM380RxBuffer[buff_pos++] = USART_ReceiveData(uartx);
		}
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}


  /* Delay functions */
void timer_init(void)
{
  if (SysTick_Config(SystemFrequency / 1000))
  { 
    /* Capture error */ 
    while (1);
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{ 
  timer_init();
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}


void SysTick_Handler(void)
{
	static char i=0;
	i++;
  TimingDelay_Decrement();
	if(i==5)
	{
		//xm_timer();
		i=0;
	}
	
}

