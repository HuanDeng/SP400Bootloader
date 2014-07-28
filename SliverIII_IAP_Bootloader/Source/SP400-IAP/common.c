
#include "common.h"
#include "ymodel.h"
#include "Process.h"
#include "stdint.h"

/*******************************************************************************
  * @函数名称	Int2Str
  * @函数说明   整形数据转到字符串
  * @输入参数   intnum:数据
  * @输出参数   str：转换为的字符串
  * @返回参数   无
*******************************************************************************/
void Int2Str(uint8_t* str, int32_t intnum)
{
    uint32_t i, Div = 1000000000, j = 0, Status = 0;

    for (i = 0; i < 10; i++)
    {
        str[j++] = (intnum / Div) + 48;

        intnum = intnum % Div;
        Div /= 10;
        if ((str[j-1] == '0') & (Status == 0))
        {
            j = 0;
        }
        else
        {
            Status++;
        }
    }
}

/*******************************************************************************
  * @函数名称	Int2Str
  * @函数说明   字符串转到数据
  * @输入参数   inputstr:需转换的字符串
  * @输出参数   intnum：转好的数据
  * @返回参数   转换结果
                1：正确
                0：错误
*******************************************************************************/
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
    uint32_t i = 0, res = 0;
    uint32_t val = 0;

    if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
    {
        if (inputstr[2] == '\0')
        {
            return 0;
        }
        for (i = 2; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                //返回1
                res = 1;
                break;
            }
            if (ISVALIDHEX(inputstr[i]))
            {
                val = (val << 4) + CONVERTHEX(inputstr[i]);
            }
            else
            {
                //无效输入返回0
                res = 0;
                break;
            }
        }

        if (i >= 11)
        {
            res = 0;
        }
    }
    else//最多12位输入
    {
        for (i = 0; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                //返回1
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
            {
                val = val << 10;
                *intnum = val;
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
            {
                val = val << 20;
                *intnum = val;
                res = 1;
                break;
            }
            else if (ISVALIDDEC(inputstr[i]))
            {
                val = val * 10 + CONVERTDEC(inputstr[i]);
            }
            else
            {
                //无效输入返回0
                res = 0;
                break;
            }
        }
        //超过10位无效，返回0
        if (i >= 11)
        {
            res = 0;
        }
    }

    return res;
}

/*******************************************************************************
  * @函数名称	Int2Str
  * @函数说明   字符串转到数据
  * @输入参数   inputstr:需转换的字符串
  * @输出参数   intnum：转好的数据
  * @返回参数   转换结果
                1：正确
                0：错误
*******************************************************************************/
/**
  * @brief  Get an integer from the HyperTerminal
  * @param  num: The inetger
  * @retval 1: Correct
  *         0: Error
  */
uint32_t GetIntegerInput(int32_t * num)
{
    uint8_t inputstr[16];

    while (1)
    {
        GetInputString(inputstr);
        if (inputstr[0] == '\0') continue;
        if ((inputstr[0] == 'a' || inputstr[0] == 'A') && inputstr[1] == '\0')
        {
            SerialPutString("User Cancelled \r\n");
            return 0;
        }

        if (Str2Int(inputstr, num) == 0)
        {
            SerialPutString("Error, Input again: \r\n");
        }
        else
        {
            return 1;
        }
    }
}
extern u8 rxBuf;
extern u8 isRecv;
uint32_t SerialKeyPressed(uint8_t *key)
{	
    if(isRecv)
		{  
		
        *key = rxBuf; 
	  isRecv=0;			
        return 1;
    }
    else
    {
        return 0;
    }

}
uint8_t GetKey(uint8_t *key)
{
    //等待按键按下
    while (1)
    {
        if (SerialKeyPressed(key)) break;
    }
    return 0;

}
uint8_t GetKeyC(uint32_t count,uint8_t *key)
{
    //等待按键按下
    while (count)
    {
        if (SerialKeyPressed(key)) break;
				count--;
    }
    if(count)return 0;
	else return 1;
}

void SerialPutChar(uint8_t c)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);   
	USART1->DR =c;
}

void Serial_PutString(uint8_t *s)
{
    while (*s != '\0')
    {
        SerialPutChar(*s);
        s++;
    }
}

void GetInputString (uint8_t * buffP)
{
    uint32_t bytes_read = 0;
    uint8_t c = 0;
    do
    {
        GetKey(&c);
        if (c == '\r')
            break;
        if (c == '\b')// Backspace 按键  
        {
            if (bytes_read > 0)
            {
                SerialPutString("\b \b");
                bytes_read --;
            }
            continue;
        }
        if (bytes_read >= CMD_STRING_SIZE )
        {
            SerialPutString("Command string size overflow\r\n");
            bytes_read = 0;
            continue;
        }
        if (c >= 0x20 && c <= 0x7E)
        {
            buffP[bytes_read++] = c;
            SerialPutChar(c);
        }
    }
    while (1);
    SerialPutString(("\n\r"));
    buffP[bytes_read] = '\0';
	bytes_read=0;
}


uint32_t FLASH_PagesMask(uint32_t Size)
{
    uint32_t pagenumber = 0x0;
    uint32_t size = Size;

    if ((size % PAGE_SIZE) != 0)
    {
        pagenumber = (size / PAGE_SIZE) + 1;
    }
    else
    {
        pagenumber = size / PAGE_SIZE;
    }
    return pagenumber;

}


u8 ReciveFile2SD(u8* fname)
{
    uint8_t key = 0;
    u32 size;
    u8 i=0; 
    u8 t1t2Buf[14]; 
    float tempt1=0,tempt2=0; 
    u32 tempserial=0;      
      BootPrarmeter BpsetP;
loop:		printf("******************************");	
		printf("Waiting CMD\n");

	
	
		GetKey(&key);

			Serial_PutString("\r\n*************************\n\r");
					Serial_PutString("\r\n*SP400 BootLoader       *\n\r");
                              Serial_PutString("\r\n*Download Factory------0*\n\r");
					Serial_PutString("\r\n*Download User---------1*\n\r");
					Serial_PutString("\r\n*Cancle----------------2*\n\r");
                              Serial_PutString("\r\n*DownLoad Parameter----3*\n\r");
					Serial_PutString("\r\n*************************\n\r");
					GetKey(&key);
            if(key == 0x30)
            {
                        isFactory=1;
                        //printf("******************************");
                        printf("Update the Factory Ver...\n");
                  	size=SerialDownload(fname);
				if(size>0)
				{
					Serial_PutString("\n\rDownload Done!\n\r");
                              
					return 1;
				}
				else return size;
            }
		else if (key == 0x31)
		{
                        isFactory=0;
                        printf("Update the new Ver...\n");
				size=SerialDownload(fname);
				if(size>0)
				{
					Serial_PutString("\n\rDownload Done!\n\r");
					return 1;
				}
				else return size;

		}
		else if(key==0x32)return 0;
            else if(key==0x33)
            {
                  printf("Update the Par...\n");
                  for(i=0;i<14;i++)
                  {
                        GetKey((t1t2Buf+i));
                  }
                  tempt1=*((float*)t1t2Buf);
                  tempt2=*((float*)(t1t2Buf+4));
                  tempserial=*((u32*)(t1t2Buf+8));
                  printf("t1=%.2f t2=%.2f serial:%d\n",tempt1,tempt2,tempserial);
                  BpsetP=*(BootPrarmeter *)((u32)BootPrarmeterAddr);
                  BpsetP.calibp.ptsaf1=tempt1;
                  BpsetP.calibp.ptsaf2=tempt2;
                   BpsetP.serialNum=tempserial;
                  Flash_Write(BootPrarmeterAddr,(u8*)&BpsetP,sizeof(BpsetP));
                  goto loop;
                  
            }
		else
		{
				printf("Error Cmd\n");
				Serial_PutString("Error Cmd\n\r");
				goto loop;
		}
   
}