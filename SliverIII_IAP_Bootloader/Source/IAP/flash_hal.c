 #include "flash_hal.h"
 #include "Parameter.h"
 #include "stdlib.h"
 
uint16_t Flash_Write_Without_check(uint32_t iAddress, uint8_t *buf, uint16_t iNumByteToWrite) {
     uint16_t i;
     volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
     i = 0;
     
 //    FLASH_UnlockBank1();
     while((i < iNumByteToWrite) && (FLASHStatus == FLASH_COMPLETE))
     {
       FLASHStatus = FLASH_ProgramHalfWord(iAddress, *(uint16_t*)buf);
       i = i+2;
       iAddress = iAddress + 2;
       buf = buf + 2;
     }
     
     return iNumByteToWrite;
 }
 /**
   * @brief  Programs a half word at a specified Option Byte Data address.
   * @note   This function can be used for all STM32F10x devices.
   * @param  Address: specifies the address to be programmed.
   * @param  buf: specifies the data to be programmed.
   * @param  iNbrToWrite: the number to write into flash
   * @retval if success return the number to write, -1 if error
   *  
   */

 uint8_t tmp[FLASH_PAGE_SIZE]=0;
 int Flash_Write(uint32_t iAddress, uint8_t *buf, uint32_t iNbrToWrite)
  {
	/* Unlock the Flash Bank1 Program Erase controller */
	uint32_t secpos;
	uint32_t iNumByteToWrite = iNbrToWrite;
	uint16_t secoff;
	uint16_t secremain;  
	uint16_t i = 0;    
	//uint8_t tmp[FLASH_PAGE_SIZE];
    //uint8_t *tmp;
	volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
//	tmp=(uint8_t *)malloc(FLASH_PAGE_SIZE);
//  	if(tmp==NULL)
//	{
//		return -1;
//	}
    memset(tmp,FLASH_PAGE_SIZE,0);
	FLASH_Unlock();
	secpos=iAddress & (~(FLASH_PAGE_SIZE -1 )) ;//������ַ 
	secoff=iAddress & (FLASH_PAGE_SIZE -1);     //�������ڵ�ƫ��
	secremain=FLASH_PAGE_SIZE-secoff;           //����ʣ��ռ��С 

	
	if(iNumByteToWrite<=secremain) secremain = iNumByteToWrite;//������4096���ֽ�
	
	while( 1 )
	{
		Flash_Read(secpos, tmp, FLASH_PAGE_SIZE);   //������������
		for(i=0;i<secremain;i++)
		{       //У������
			if(tmp[secoff+i]!=0XFF)break;       //��Ҫ���� 
		}
		if(i<secremain)
		{  //��Ҫ����
			FLASHStatus = FLASH_ErasePage(secpos); //�����������
			if(FLASHStatus != FLASH_COMPLETE)
			return -1;
			for(i=0;i<secremain;i++) {    //����
			tmp[i+secoff]=buf[i];   
			}
			Flash_Write_Without_check(secpos ,tmp ,FLASH_PAGE_SIZE);//д����������  
		} 
		else
		{
			Flash_Write_Without_check(iAddress,buf,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������.
		}
		
		if(iNumByteToWrite==secremain) //д�������
		break;
		else {
		secpos += FLASH_PAGE_SIZE;
		secoff = 0;//ƫ��λ��Ϊ0 
		buf += secremain;  //ָ��ƫ��
		iAddress += secremain;//д��ַƫ��    
		iNumByteToWrite -= secremain;  //�ֽ����ݼ�
		if(iNumByteToWrite>FLASH_PAGE_SIZE) secremain=FLASH_PAGE_SIZE;//��һ����������д����
		else secremain = iNumByteToWrite;  //��һ����������д����
		}
	
	}
	FLASH_Lock();
	return iNbrToWrite; 
 }
 int Flash_Read(uint32_t iAddress, uint8_t *buf, int32_t iNbrToRead)
 {
         int i = 0;
         while(i < iNbrToRead )
		 {
            *(buf + i) = *(__IO uint8_t*) iAddress++;
            i++;
         }
         return i;
 }       