
#include "Process.h"
#include "stdint.h"
#include "Ymodel.h"

unsigned char file_name[FILE_NAME_LENGTH];
char UserBuf[PACKET_1K_SIZE];  
int PackLen=0;                 

uint16_t UpdateCRC16(uint16_t crcIn, uint8_t byte)
{
    uint32_t crc = crcIn;
    uint32_t in = byte|0x100;
    do
    {
        crc <<= 1;
        in <<= 1;
        if (in&0x100)
            ++crc;
        if (crc&0x10000)
            crc ^= 0x1021;
    }
    while (!(in&0x10000));
    return crc&0xffffu;
}


uint16_t Cal_CRC16(const uint8_t* data, uint32_t size)
{
    uint32_t crc = 0;
    const uint8_t* dataEnd = data+size;
    while (data<dataEnd)
        crc = UpdateCRC16(crc,*data++);

    crc = UpdateCRC16(crc,0);
    crc = UpdateCRC16(crc,0);
    return crc&0xffffu;
}


uint8_t CalChecksum(const uint8_t* data, uint32_t size)
{
    uint32_t sum = 0;
    const uint8_t* dataEnd = data+size;
    while (data < dataEnd )
        sum += *data++;
    return sum&0xffu;
}

u8 isFactory=0;
int YmodemRecvData(void)
{
    int i=0,j=0; u8 FileTemp[512]={0};
    char ErrorNum=0;                                                //�������
    char YmodemState=0;
    unsigned char TempChar=0;                                       //��������
    int crcvalue=0;    
    int len=0;    
    int count=0;u32 saveLenth;
    ErrorNum=0;
    while(1)
    {
        switch(YmodemState)
        {
          case 0:                                                   //ͨ����ʼ�׶�
                         SerialPutChar(CRC16);                      //����ʼ�ź�
                        if(GetKeyC(1000000,&TempChar)==0)           //ÿ�εȴ�0.2s�ӣ�������ʱ�ط���C��
                        {
                            if(TempChar==SOH )
                            {
                                GetKey(&TempChar);
                                if(TempChar!=0x00)
                                    return ;          		 //����00��š�
                                GetKey(&TempChar);
                                if ( TempChar != 0xFF )
                                    return ;     			 //����00��Ų��롣
                                for ( i=0; i<128; i++ )
                                {                                   //�������ݰ�0����128�ֽ�
                                    GetKey((UserBuf+i));
                                }
                                GetKey(&TempChar);
                                GetKey(&TempChar);                  
                                
                                SerialPutChar(ACK);                 //����ȷ���źš�
                                YmodemState=1;                      //״̬�л������ݴ���״̬
                                for ( i=0; i<128; i++ )
                                {                                   //�������ݰ�0����128�ֽ�
                                    file_name[i]=0;
                                } 
                                for ( i=0; (i<128)&&(UserBuf[i]); i++ )
                                {                                   //�������ݰ�0����128�ֽ�
                                    file_name[i]=UserBuf[i];
                                }
                              Frimware=f_open(&f_bin,"new_temp.bin",FA_OPEN_EXISTING);
                              f_close(&f_bin);
                              if(Frimware==FR_NO_FILE)
                              {
                                    Frimware=f_open(&f_bin,"new_temp.bin",FA_CREATE_NEW);
                                    f_close(&f_bin);
                              }
                              else if(Frimware==FR_OK)
                              {
//                                    Frimware=f_open(&f_bin,"old.bin",FA_OPEN_EXISTING);
//                                    f_close(&f_bin);
//                                    if(Frimware==FR_OK)
//                                    {
                                          Frimware=f_unlink("new_temp.bin");
                                          Frimware=f_open(&f_bin,"new_temp.bin",FA_CREATE_NEW);
                                          f_close(&f_bin);
//                                    }
                                    //Frimware=f_rename("new.bin","old.bin");
                                    //Frimware=f_open(&f_bin,"new.bin",FA_CREATE_NEW);
                                    //f_close(&f_bin);
                              }                              
                              //f_close(&f_bin);
                              Frimware=f_open(&f_bin,"new_temp.bin",FA_WRITE);
                              
                                Str2Int((UserBuf+i+1),&PackLen);
                                SerialPutChar (CRC16);              //�ٷ�һ��C����ʽ�������ݴ���                         
                            }
                        }
                         break;
          case 1:        //���ݴ���׶�
test:                    GetKey(&TempChar);          
                         switch(TempChar)
                         {                                                        
                              case EOT: YmodemState = EOT;
                                        SerialPutChar ( ACK );
                                        continue;
                              case SOH:
                                           len = 128;
                                           break; 
                              case STX:
                                           len = 1024;                                      
                                           break; 
                              case ABORT1:
							 Serial_PutString("CANCEL\r\n");
                                           return PackLen;
                              case ABORT2:
                                           Serial_PutString("CANCEL\r\n");
                                           return PackLen;
                              default :
                                    return PackLen;                                                    
                        }  //end of switch (StartChar)
                        GetKey(&TempChar); 
                        GetKey(&TempChar); 
                        for(i=0;i<len;i++)
                        {      //�����������ݰ�
                            GetKey((UserBuf+i));
                        }
                        //CRC����
                        GetKey(&TempChar);
                        crcvalue=TempChar; 
                        crcvalue<<=8;
                        GetKey(&TempChar);
                        crcvalue|=TempChar; 

                        if(Cal_CRC16(UserBuf,len)!=crcvalue)
                        {
                            ErrorNum+=1;
                        }    
                                 
                        if(ErrorNum==0)
                        {
														
                              if(len==1024)
                              {
                                    for(j=0;j<512;j++)
                                    {
                                          FileTemp[j]=UserBuf[j];
                                    }
                                    Frimware=f_write(&f_bin,FileTemp,512,&saveLenth);
                                    for(;j<1024;j++)
                                    {
                                          FileTemp[j-512]=UserBuf[j];
                                    }
                                    Frimware=f_write(&f_bin,FileTemp,512,&saveLenth);
                              }
                              else
                                    Frimware=f_write(&f_bin,UserBuf,len,&saveLenth);
                            SerialPutChar(ACK);  
                            count+=len;
                        }                              
                        else SerialPutChar(NAK);                         //���շ��ִ���Ҫ���ط���                   
                        break;
          case EOT:   //��������׶�
                         SerialPutChar(CRC16);
                        GetKey(&TempChar);                               //������ʼ�ַ���
                        if(TempChar==SOH)
                        {
                            GetKey(&TempChar); 
                            if(TempChar!=0x00)
                              {
                                    return PackLen;            //����00��š�
                              }
                            GetKey(&TempChar); 
                            if(TempChar!=0xFF)
                              {
                                    return PackLen;            //����00��Ų��롣
                              }
                            for(i=0;i<128;i++)
                            {
                                   GetKey((UserBuf+i));
                            }
                            //CRC����
                            GetKey(&TempChar);
                            crcvalue=TempChar; 
                            crcvalue<<=8;
                            GetKey(&TempChar);
                            crcvalue|=TempChar; 

                            if(Cal_CRC16(UserBuf,128)!=crcvalue)
                            {
                                ErrorNum+=1;
                            }    
                            if(ErrorNum==0)
                            {
                                SerialPutChar(ACK);  
                                return PackLen;
                            }                              
                            else SerialPutChar(NAK);                     //���շ��ִ���Ҫ���ط��� 
                            break;
                        }                 
          default:     
                     return PackLen;
          } 
       } 
    return PackLen;
}