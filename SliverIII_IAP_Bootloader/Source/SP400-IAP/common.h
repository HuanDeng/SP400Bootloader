 /*******************************************************************************
** �ļ���: 		common.h
** �汾��  		1.0
** ��������: 	RealView MDK-ARM 4.21
** ����: 		�˽�
** ��������: 	2012-02-29
** ����:		common�ļ���ͷ�ļ�����
** ����ļ�:	string.h��stdio.h��stm32f10x.h��ymodem.h
** �޸���־��	2012-02-29   �����ĵ�
*******************************************************************************/

/* ��ֹ�ض��� ----------------------------------------------------------------*/
#ifndef _COMMON_H
#define _COMMON_H

/* ����ͷ�ļ� *****************************************************************/
#include "stdio.h"
#include "string.h"

#include "ymodel.h"


/* �������� -----------------------------------------------------------------*/
typedef  void (*pFunction)(void);

//* �� ------------------------------------------------------------------------*/
#define CMD_STRING_SIZE       128


 #define PAGE_SIZE                         (0x400)    /* 1 Kbyte */		
 #define FLASH_SIZE                        (0x20000)  /* 128 KBytes */


//�����ϴ��ļ���С 
#define FLASH_IMAGE_SIZE                   (unsigned int) (FLASH_SIZE - (ApplicationAddress - 0x08000000))

#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

#define SerialPutString(x) Serial_PutString((unsigned char*)(x))


/* �������� ------------------------------------------------------------------*/
void Int2Str(unsigned char* str,int intnum);
unsigned int Str2Int(unsigned char *inputstr,int *intnum);
unsigned int GetIntegerInput(int * num);
unsigned int SerialKeyPressed(unsigned char *key);
unsigned char GetKey(unsigned char *key);
unsigned char GetKeyC(unsigned int count,unsigned char *key);
void SerialPutChar(unsigned char c);
void Serial_PutString(unsigned char *s);
void GetInputString(unsigned char * buffP);
unsigned int FLASH_PagesMask(unsigned int Size);
void FLASH_DisableWriteProtectionPages(void);
u8 ReciveFile2SD(u8 *fname);
u32 SerialDownload(u8 *fname);
void SerialUpload(void);

#endif  /* _COMMON_H */

/*******************************�ļ�����***************************************/