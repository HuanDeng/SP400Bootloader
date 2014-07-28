
#ifndef _XMODEM_H_
#define _XMODEM_H_

//����Լ����������Ͷ���
#include "type.h"
#include "stm32f10x.h"
#include "Process.h"


#define XM_DLY_1S	200		//��1S��xm_timer�����õĴ�����С��65536
#define XM_RETRY	16		//retry����

#define XM_SOH  	0x01	//Xmodem����ͷ
#define XM_STX  	0x02	//1K-xmodem����ͷ
#define XM_EOT  	0x04	//���ͽ���
#define XM_ACK  	0x06	//�Ͽ�Ӧ��
#define XM_NAK  	0x15	//���Ͽ�Ӧ��
#define XM_CAN  	0x18	//�Ի���������
#define XM_EOF 		0x1A	//���ݰ����

#define XM_OK		0
#define XM_ERR		-1

#define MAXRETRANS 	25

extern uint16 xmodem_timeout;

uint16 xm_crc16_ccitt(const uint8 *buf, sint32 len);
void xm_port_write(uint8 *ch);
sint8 xm_port_read(uint8 *ch);
void xm_timer(void);
void xm_disp(sint8 *str);
//����ʵ�ʽ��յ����ݳ���
sint32 xm_receive(uint8 *dest, sint32 destsz);
sint32 xm_transmit(uint8 *src, sint32 srcsz);







#endif /* _CRC16_H_ */
