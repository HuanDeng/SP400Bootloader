
#include "EMSP_API.h"
#include "EMSP.h"
#include "EMW_hal.h"
#include <string.h>	


extern u8 recvdata[300];
extern u8 recvlen;
extern u8 buffer_bytes;
u32 fw_version;
EMW_mode_TypeDef  EM380C_mode;

u8 EMW_Get_status(EMW_status_TypeDef* EMW_status)
{  
   u32 timeout;
   vs8 ret = EMW_FAIL;
   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_GET_STATUS, 0, NULL, 0);
	}

   switch(*(u16 *)(recvdata+2*sizeof(u8)))
 	 {
        case TCP_stop:
		   EMW_status->TCPstatus = TCP_stop;
		   break;
        case TCP_listening:
		   EMW_status->TCPstatus = TCP_listening;
		   break;
		case TCP_connected:
		   EMW_status->TCPstatus = TCP_connected;
		   break;
		default:
		   EMW_status->TCPstatus = TCP_unknown;
			break;
	 }

   switch(*(u16 *)(recvdata))
 	 {
        case wifi_connected:
		   EMW_status->WiFistatus = wifi_connected;
		   break;
        case wifi_disconnected:
		   EMW_status->WiFistatus = wifi_disconnected;
		   break;
		default:
		   EMW_status->WiFistatus = wifi_unknown;
			break;
	 }

  return ret;
}

u8 EMW_Get_RF_POWER(EMW_RF_POWER_TypeDef* RF_POWER)
{
   u32 timeout;
   vs8 ret = EMW_FAIL;
   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_GET_RF_POWER, 0, NULL, 0);
	}

   RF_POWER->Min_Power=	recvdata[0];
   RF_POWER->Max_Power=	recvdata[1];
   RF_POWER->Cur_Power=	recvdata[2];

  return ret;
}

u8 EMW_Get_Config(EMW_parm_TypeDef* EMW_Parm)
{
   u32 timeout;
   vs8 ret = EMW_FAIL;

#ifdef 	EMW_FIRMWARE_UART
   u8 type = 1;
   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_GET_CONFIG, 0, &type, 1) ;
	}
#elif defined EMW_FIRMWARE_UART_NO_WPA
   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_GET_CONFIG, 0, NULL, 0) ;
	}
#endif
   memcpy(EMW_Parm,recvdata,sizeof(EMW_parm_TypeDef));
   return ret;
}

u8 EMW_Set_Config(EMW_parm_TypeDef* EMW_Parm)
{
   u32 timeout;
   vs8 ret = EMW_FAIL;
   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_SET_CONFIG, 0, EMW_Parm, sizeof(EMW_parm_TypeDef));
	}					  
   return ret;
}


u8 EMW_Get_DNS(EMW_DNS_TypeDef* EMW_dns)
{
   u32 timeout;
   vs8 ret = EMW_FAIL;

   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_GET_DNS, 0, NULL, 0) ;
	}
   memcpy(EMW_dns,recvdata,sizeof(EMW_DNS_TypeDef));
   return ret;
}

u8 EMW_Set_DNS(EMW_DNS_TypeDef* EMW_dns)
{
   u32 timeout;
   vs8 ret = EMW_FAIL;
   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_SET_DNS, 0, EMW_dns, sizeof(EMW_DNS_TypeDef));
	}
   return ret;
}


u8 EMW_Get_Name(u8* name)
{
   u32 timeout;
   vs8 ret = EMW_FAIL;

   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_GET_NAME, 0, NULL, 0) ;
	}
   memcpy(name,recvdata,RECV_BUFFERSIZE);
   return ret;
}

u8 EMW_Get_Mac(u8* mac)
{
   u32 timeout;
   vs8 ret = EMW_FAIL;

   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_GET_MAC_ADDR, 0, NULL, 0) ;
	}
   memcpy(mac,recvdata,RECV_BUFFERSIZE);
   return ret;
}



u8 EMW_Set_Name(u8* name, u8 name_length)
{
   u32 timeout;
   vs8 ret = EMW_FAIL;
   u8 input_name[40]={0};
   memcpy(input_name, name, name_length);

   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_SET_NAME, 0, input_name, 40);
	}
   return ret;
}

u8 EMW_Set_RF_POWER(EMW_RF_POWER_TypeDef* RF_POWER)
{
   u32 timeout;
   vs8 ret = EMW_FAIL;
   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_SET_RF_POWER, 0, &(RF_POWER->Cur_Power), sizeof(u8));
	}

  return ret;
}

u8 EMW_Get_ver(u32* version)
{	 
   u32 timeout;
   vs8 ret = EMW_FAIL;
   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
     ret = send_cmd(EMSP_CMD_GET_VER, 0, NULL, 0);
   *version = *(u32 *)(recvdata);

   return ret;

}

u8 EMW_Get_APList(EMW_APLst_TypeDef* EMW_APLst)
{	 
   u32 timeout,AP_number;
   u8 scan_index=0,AP_name_length;
   u8 ret = EMW_FAIL;
   u8 tmpRssi;
   EMW_APLst_TypeDef* EMW_APLst_scan = EMW_APLst;

   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_SCAN_AP, 0, NULL, 0);
	}

   if(ret==0xFF)
	goto done;

     AP_name_length=strcspn((const char *)recvdata, "\0")+1; 
	  
     for(AP_number = 0;AP_number < ret;AP_number++,EMW_APLst_scan++)
    {  
	  memcpy(EMW_APLst_scan, recvdata+scan_index, AP_name_length);							 //copy AP name
	  tmpRssi = *(recvdata+scan_index+AP_name_length);											 //get RSSI
	  if(tmpRssi < 40)
         tmpRssi = 100;
	  else if(tmpRssi > 95)
	     tmpRssi = 0;
	  else
	    {
	     tmpRssi = (float)(95 - tmpRssi) / (95-40) * 100;
	    }
	  
	  EMW_APLst_scan->AP_signal = tmpRssi;				 
	  scan_index += AP_name_length+2;
	  AP_name_length =  strcspn((const char *)(recvdata+scan_index), "\0")+1;  
	}
   scan_index=0;
done:
	return ret;

}

u8 EMW_Get_Strength(u8* SSID, u8 SSID_length, EMW_APLst_TypeDef* EMW_APLst)
{	 
   u32 timeout,AP_number;
   u8 scan_index=0,AP_name_length;
   u8 ret = EMW_FAIL;
   u8 tmpRssi, SSID_name[32]={0};
   EMW_APLst_TypeDef* EMW_APLst_scan = EMW_APLst;
   memcpy(SSID_name, SSID, SSID_length);

   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_SCAN_CMP, 0, SSID_name, 32);
	}

   if(ret==0xFF)
	goto done;

     AP_name_length=strcspn((const char *)recvdata, "\0")+1; 
	  
     for(AP_number = 0;AP_number < ret;AP_number++,EMW_APLst_scan++)
    {  
	  memcpy(EMW_APLst_scan, recvdata+scan_index, AP_name_length);							 //copy AP name
	  tmpRssi = *(recvdata+scan_index+AP_name_length);											 //get RSSI
	  if(tmpRssi < 40)
         tmpRssi = 100;
	  else if(tmpRssi > 95)
	     tmpRssi = 0;
	  else
	    {
	     tmpRssi = (float)(95 - tmpRssi) / (95-40) * 100;
	    }
	  
	  EMW_APLst_scan->AP_signal = tmpRssi;				 
	  scan_index += AP_name_length+2;
	  AP_name_length =  strcspn((const char *)(recvdata+scan_index), "\0")+1;  
	}
   scan_index=0;
done:
	return ret;
}


u8 EMW_Send_Data(u8* Data,u32 len)
{
   vs8 ret = EMW_FAIL;
   u32 timeout;

   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_SEND_DATA, 0, Data, len);
	}

	return ret;

}

u8 EMW_Open_Socket(EMW_socket_TypeDef* socket)
{
   u32 timeout;
   vs8 ret = EMW_FAIL;
   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_OPEN_SOCKET, 0, socket, sizeof(EMW_socket_TypeDef));
	}

  return ret;
}

u8 EMW_Close_Socket(void)
{
   u32 timeout;
   vs8 ret = EMW_FAIL;
   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_CLOSE_SOCKET, 0, NULL, 0);
	}

  return ret;
}

u8 EMW_Close_Wifi(void)
{
   u32 timeout;
   vs8 ret = EMW_FAIL;
   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_WIFI_STOP, 0, NULL, 0);
	}

  return ret;
}

u8 EMW_Open_Wifi(void)
{
   u32 timeout;
   vs8 ret = EMW_FAIL;
   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_WIFI_CONNECT, 0, NULL, 0);
	}

  return ret;
}

u8 EMW_GET_IP(EMW_IP_TypeDef* IP)
{
   u32 timeout;
   vs8 ret = EMW_FAIL;
   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_GET_IP, 0, NULL, 0);
	}
   memcpy(IP, recvdata, sizeof(EMW_IP_TypeDef));


  return ret;

}
u8 EMW_SET_IP(EMW_IP_TypeDef* IP)
{
   u32 timeout;
   vs8 ret = EMW_FAIL;
   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_SET_IP, 0, IP, sizeof(EMW_IP_TypeDef));
	}

  return ret;
}


u8 EMW_Data_transceive_mode(u8 data_mode)
{
   u32 timeout;
   vs8 ret = EMW_FAIL;
   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_SET_PINGPONG_MODE, 0, &data_mode, 1);
	}

  return ret;
}

u8 EMW_Set_Mode(EMW_mode_TypeDef mode)
{
  vs8 ret = EMW_FAIL;
  switch(mode)
 {
    case DTU_mode:{
	   EM380C_mode = DTU_mode;
       SET_STATUS_PIN();
	   ret = EMW_SUCCESS;
	   break;
	   }
    case CMD_mode:{
	   EM380C_mode = CMD_mode;
       RESET_STATUS_PIN();
	   ret = EMW_SUCCESS;
	   break;
	   }
	default:
	   break;
 }
  return ret;

}

u8 EMW_Sleep(void)
{
  RESET_WAKEUP_PIN();
  Delay(100);
  return EMW_SUCCESS;
}

u8 EMW_Wakeup(void)
{
  vs8 ret = EMW_FAIL;
  SET_WAKEUP_PIN();
  Delay(50);
  while(READ_INT_PIN());
  ret = EMW_SUCCESS;
  return ret;

}
u8 EMW_Startup(void)
 {	 
   u32 timeout;
   s8 ret = EMW_FAIL;

   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_START, 0, NULL, 0);
	}
   return ret;
}



u8 EMW_Init(uint32_t BaudRate,uint16_t WordLength,uint16_t StopBits,uint16_t Parity,uint16_t CTSRTS,uint16_t buffer)
{  	
   u32 timeout,timeout2;
   u8 ret = EMW_FAIL;
   buffer_bytes = buffer;

   for(timeout=2;timeout>0&&ret==EMW_FAIL;timeout--)
   {
      if(EMW_HAL_Init(BaudRate,WordLength,StopBits,Parity,CTSRTS) == EMW_FAIL)
	     break;
	  for(timeout2=1;timeout2>0&&ret==EMW_FAIL;timeout2--)
        {
		 ret = send_cmd(EMSP_CMD_GET_VER, 0, NULL, 0);
		}
   }
	return ret;
}

u8 EMW_InitWithDefault(void)
{
   u32 timeout,timeout2;
   u8 ret = EMW_FAIL;
   buffer_bytes = buffer_2bytes;


   for(timeout=2;timeout>0&&ret==EMW_FAIL;timeout--)
   {
      if(EMW_HAL_InitWithDefault()==EMW_FAIL)
	     break;
	  for(timeout2=1;timeout2>0&&ret==EMW_FAIL;timeout2--)
        {
		 ret = send_cmd(EMSP_CMD_GET_VER, 0, NULL, 0);
		}
   }
	return ret;
}

u8 EMW_Reset(void)
 {	 
   u32 timeout;
   s8 ret = EMW_FAIL;

   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_RESET, 0, NULL, 0);
	}
   return ret;
}

u8 EMW_Reset_Delay(u16 second)
 {	 
   u32 timeout;
   s8 ret = EMW_FAIL;

   for(timeout=5;timeout>0&&ret==EMW_FAIL;timeout--)
   {
     ret = send_cmd(EMSP_CMD_SET_RESET_DELAY, 0, &second, 2);
	}
   return ret;
}

void EMW_printf_config(void)
{   
   EMW_parm_TypeDef parm;

   while(EMW_Get_Config(&parm)==EMW_FAIL);									 //EMSP API, get EM380C status

   switch(parm.wifi_mode)
 	 {
        case AP:
 	      printf("Mode:AP/");
		   break;
        case Ad_Hoc:
 	      printf("Mode:Ad-Hoc/");
		   break;
		default:
	       printf("Mode ERROR/");
		break;
	 }

	 printf("ssid:%s/",parm.wifi_ssid);

#ifdef 	EMW_FIRMWARE_UART
	 switch(parm.sec_mode)
 	 {
        case Secure_WEP:
 	      printf("WEP/");
		  printf("key:%s/",parm.wifi_wepkey);
		  break;
        case Secure_WPA_WPA2_PSK:
 	      printf("WPA PSK/");
		  printf("key:%s/",parm.wpa_psk);
		  break;
        case Secure_None:
 	      printf("No secure/");
		  break;
		default:
	      printf("ERROR/");
		break;
	 }
#elif defined EMW_FIRMWARE_UART_NO_WPA
 	 printf("WEP/");
	 printf("key:%s/",parm.wifi_wepkey);
#endif

   printf("IP:%s/",parm.local_ip_addr);
   printf("Server:%s/",parm.remote_ip_addr);
   printf("mask:%s/",parm.net_mask);
   printf("GW:%s/",parm.gateway_ip_addr);
   printf("port:%d/",parm.portH<<8|parm.portL);


      switch(parm.use_dhcp)
 	 {
        case DHCP_Disable:
 	      printf("NO DHCP/");
		   break;
        case DHCP_Enable:
 	      printf("DHCP/");
		   break;
		default:
	       printf("DHCP mode ERROR/");
		break;
	 }

      switch(parm.use_udp)
 	 {
        case TCP_mode:
		  switch(parm.connect_mode)
 	 		{
        	case TCP_Server:
 	      		printf("TCP Server/");
		    	break;
            case TCP_Client:
 	        	printf("TCP Client/");
		    	break;
		    default:
	        	printf("connect mode ERROR/");
				break;
	 		}
		  break;
        case UDP_mode:
		  switch(parm.connect_mode)
 	 		{
        	case TCP_Server:
 	      		printf("UDP Multicast/");
		    	break;
            case TCP_Client:
 	        	printf("UDP Unicast/");
		    	break;
		    default:
	        	printf("connect mode ERROR/");
				break;
	 		}
		  break;
		default:
	       printf("mode ERROR/");
		break;
	 }

	 	  switch(parm.UART_buadrate)
 	 {
        case BaudRate_9600:
 	      printf("UART:9600/");
		   break;
        case BaudRate_19200:
 	      printf("UART:19200/");
		   break;
        case BaudRate_38400:
 	      printf("UART:38400/");
		   break;
        case BaudRate_57600:
 	      printf("UART:57600/");
		   break;
		case BaudRate_115200:
 	      printf("UART:115200/");
		  break;
		case BaudRate_230400:
 	      printf("UART:230400/");
		   break;
		case BaudRate_460800:
 	      printf("UART:460800/");
		  break;
		case BaudRate_921600:
 	      printf("UART:921600/");
		  break;
		default:
	       printf("UART:ERROR/");
		break;
	 }

	  switch(parm.data_length)
 	 {
        case WordLength_8b:
 	      printf("8/");
		   break;
        case WordLength_9b:
 	      printf("9/");
		   break;
		default:
	       printf("ERROR/");
		break;
	 }

	  switch(parm.parity)
 	 {
        case Parity_No:
 	      printf("n/");
		   break;
        case Parity_Even:
 	      printf("E/");
		   break;
        case Parity_Odd:
 	      printf("O/");
		   break;
		default:
	       printf("ERROR/");
		break;
	 }

	  switch(parm.stop_bits)
 	 {
        case StopBits_1:
 	      printf("1/");
		   break;
        case StopBits_0_5:
 	      printf("0.5/");
		   break;
        case StopBits_2:
 	      printf("2/");
		   break;
        case StopBits_1_5:
 	      printf("1.5/");
		   break;
		default:
	       printf("ERROR/");
		break;
	 }

	 switch(parm.use_CTS_RTS)
 	 {
        case HardwareFlowControl_None:
 	      printf("NoCTSRTS/");
		   break;
        case HardwareFlowControl_RTS_CTS:
 	      printf("CTSRTS/");
		   break;
		default:
	       printf("ERROR/");
		break;
	 }

	  switch(parm.DMA_buffersize)
 	 {
        case buffer_2bytes:
 	      printf("Buffer:2bytes/");
		   break;
        case buffer_16bytes:
 	      printf("Buffer:16bytes/");
		   break;
        case buffer_32bytes:
 	      printf("Buffer:32bytes/");
		   break;
        case buffer_64bytes:
 	      printf("Buffer:64bytes/");
		   break;
		case buffer_128bytes:
 	      printf("Buffer:128bytes/");
		   break;
		case buffer_256bytes:
 	      printf("Buffer:256bytes/");
		   break;
		case buffer_512bytes:
 	      printf("Buffer:512bytes/");
		   break;
		default:
	       printf("Buffer:ERROR/");
		break;
	 }


   printf("ID:%d/",parm.IO_Control);
}
