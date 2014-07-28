
#include "stm32f10x.h"
#include "xmodem.h"


//��ֲʱ����Ҫ�޸ĸú���
//�����շ���ʵ�ò�ѯ��ʽ��
void xm_port_write(uint8 *ch)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);   
	USART1->DR = *ch;

}

//���ڽ��պ�������Ҫ��ֲ
extern u8 rxBuf;
extern u8 isRecv;
sint8 xm_port_read(uint8 *ch)
{
	if(USART_GetFlagStatus(USART1, USART_IT_RXNE) != RESET)
	{  
		*ch = USART_ReceiveData(USART1);
		
		return 1;
	}
//	if(isRecv)
//	{
//			*ch=rxBuf;
//			isRecv=0;
//			return 1;
//	}

	return 0;
}

//�ڶ�ʱ�ж�����øú���
//��ʱʱ��5ms
void xm_timer(void)
{
	xmodem_timeout++;
}


