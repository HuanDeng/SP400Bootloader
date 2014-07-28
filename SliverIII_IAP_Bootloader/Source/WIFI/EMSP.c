#include "EMSP.h"
#include "EMW_hal.h"
#include "EMSP_API.h"

 
__packed struct emsp_header {
	u16 cmdcode;	// Command code
	u16 size;		// size of the packet, including the header and footer
	u16 result;		// Result code, set by module.
					// This field is used only the response packet. Set 0 in the request packet
	u16 checksum;	// check sum of the header
};

__packed struct emsp_footer {
	u16 checksum;	// check sum of the packet body( not include the header)
};


u8 sendbuf[EMSP_MAX_PACKET_SIZE];
u8 recvbuf[EMSP_MAX_PACKET_SIZE];

u8 senddata[SEND_BUFFERSIZE];
u8 recvdata[RECV_BUFFERSIZE];
u8 recvlen;
u8 buffer_bytes; 

u16 calc_sum(void *data, u32 len)
{
	u32	cksum=0;
	__packed u16 *p=data;

	while (len > 1)
	{
		cksum += *p++;
		len -=2;
	}
	if (len)
	{
		cksum += *(u8 *)p;
	}
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >>16);

	return ~cksum;
}

int check_sum(void *data, u32 len)	
{
	__packed u16 *sum;
	u8 *p = (u8 *)data;
	
	p += len - 2;

	sum = (u16 *)p;

	if (calc_sum(data, len - 2) != *sum) {	// check sum error		
 		return 0;
	}
	return 1;	// ckeck sum ok
}

vs8 recv_cmd(void) 
{
	int datalen;
	u8 ret = EMW_FAIL;
	struct emsp_header *hdr = (struct emsp_header *)recvbuf;
	u8 *data = &recvbuf[sizeof(struct emsp_header)];

    recvlen = UART_receive_buf(recvbuf);


	if (!check_sum(hdr, sizeof(struct emsp_header)) || recvlen != hdr->size) {	// check sum error or length error
		ret = EMW_FAIL;
		goto done;
	}
	
	datalen = hdr->size - sizeof(struct emsp_header);

	if (!check_sum(data, datalen)) {	// check sum error
		ret = EMW_FAIL;
		goto done;
	}

	switch (hdr->cmdcode) {
		case EMSP_CMD_RESET:			
		case EMSP_CMD_SET_CONFIG:			
		case EMSP_CMD_SET_NAME:			
		case EMSP_CMD_START:				
		case EMSP_CMD_CLOSE_SOCKET:		
		case EMSP_CMD_OPEN_SOCKET:		
		case EMSP_CMD_SET_DNS:		
		case EMSP_CMD_SET_IP:			
		case EMSP_CMD_SET_RF_POWER:			
		case EMSP_CMD_WIFI_STOP:			
		case EMSP_CMD_WIFI_CONNECT:
		case EMSP_CMD_SET_PINGPONG_MODE:
		case EMSP_CMD_SET_RESET_DELAY:			
			if (hdr->result == 1) {
			ret = EMW_SUCCESS;
			}
			break;
		case EMSP_CMD_GET_DNS:			
		case EMSP_CMD_GET_CONFIG:			
		case EMSP_CMD_GET_NAME:			
		case EMSP_CMD_GET_STATUS:		
		case EMSP_CMD_GET_VER:
		case EMSP_CMD_GET_RF_POWER:		
		case EMSP_CMD_GET_IP:
		case EMSP_CMD_GET_MAC_ADDR:			
			if (hdr->result == 1) {
			memcpy(recvdata, data, datalen - 2);
			recvlen = datalen - 2;
			ret = EMW_SUCCESS;
			}
			break;
		case EMSP_CMD_SCAN_AP:				
		case EMSP_CMD_SCAN_CMP:				
			memcpy(recvdata, data, datalen - 2);
			recvlen = datalen - 2;
			ret= hdr->result;
			if(ret == 0) ret = 0xFF;	   
			break;
		case EMSP_CMD_SEND_DATA:			
			if (hdr->result >= 1) {
			ret = EMW_SUCCESS;
			}
			break;
		case EMSP_CMD_RECV_DATA:		
		   if (hdr->result >= 1) 
		   {
			memcpy(recvdata, data, datalen - 2);
			recvlen = datalen - 2;
			ret = recvlen;
		   }
			break;
		default:
			break;
	}
done:
	return ret;

}

int send_cmd(unsigned short cmdcode, unsigned short result, void *data, unsigned short datalen)
{
	u8 *dt;u8 test[10]={0x02,0x00,0x0a,0x00,0x00,0x00,0xf3,0xff,0xff,0xff};
	struct emsp_header *hdr;
	struct emsp_footer *ft;
	u16 len = sizeof(struct emsp_header) + datalen + sizeof(struct emsp_footer);
	memset(recvbuf, 0x0, 256); 

	if (sendbuf == 0) {
		return -1;
	}
	hdr = (struct emsp_header *)sendbuf;
	dt = (sendbuf + sizeof(struct emsp_header));
	ft = (struct emsp_footer *)(dt + datalen);

	hdr->cmdcode = cmdcode;
	hdr->size = len;
	hdr->result = result;
	hdr->checksum = calc_sum(hdr, sizeof(struct emsp_header) - 2);
	
	memcpy(dt, data, datalen);

	ft->checksum = calc_sum(dt, datalen);
	
	UART_send_buf(sendbuf, len);buffer_bytes=buffer_512bytes;
	switch(buffer_bytes){
      case buffer_2bytes:	
		   Delay(300);
		   break;							 // EM380C's response has a delay
      case buffer_16bytes:	
		   Delay(500);
		   break;							 // EM380C's response has a delay
      case buffer_32bytes:	
		   Delay(500);
		   break;							 // EM380C's response has a delay
      case buffer_64bytes:	
		   Delay(500);
		   break;							 // EM380C's response has a delay
      case buffer_128bytes:	
		   Delay(500);
		   break;							 // EM380C's response has a delay
      case buffer_256bytes:	
		   Delay(500);
		   break;							 // EM380C's response has a delay
      case buffer_512bytes:	
		   Delay(500);
		   break;							 // EM380C's response has a delay
	  default:
		    Delay(500);
			break;
			}
	if(cmdcode == EMSP_CMD_SCAN_AP)
	 Delay(1500);
	if(cmdcode == EMSP_CMD_SCAN_CMP)
	 Delay(1500);
	 if(cmdcode == EMSP_CMD_WIFI_CONNECT)
	 Delay(200);
	 if(cmdcode == EMSP_CMD_GET_STATUS)
	 Delay(200);
	 if(cmdcode == EMSP_CMD_SEND_DATA)
	 Delay(500);
	
	 
	
	return recv_cmd();
}



