#ifndef __EMSP_API_H
#define __EMSP_API_H

#include "stm32f10x.h"

//#define EMW_FIRMWARE_UART_NO_WPA         0x01040100       
#define EMW_FIRMWARE_UART                  0x01050200
//#define EMW_FIRMWARE_SPI_NO_WPA          0x01040400    
//#define EMW_FIRMWARE_SPI        	       0x01050500

#define EMW_SUCCESS                        0xFF
#define EMW_FAIL                           0x0

#define Ad_Hoc                             0x0
#define AP                                 0x1

#define	TCP_Server                         0x0
#define TCP_Client                         0x1

#define DHCP_Disable                       0x0
#define DHCP_Enable	                       0x1

#define TCP_mode                           0x0
#define UDP_mode	                       0x1

#define BaudRate_9600                      0x0
#define BaudRate_19200                     0x1
#define BaudRate_38400                     0x2 
#define BaudRate_57600                     0x3 
#define BaudRate_115200                    0x4
#define BaudRate_230400                    0x5
#define BaudRate_460800                    0x6
#define BaudRate_921600                    0x7


#define buffer_2bytes                      0x0
#define buffer_16bytes                     0x1
#define buffer_32bytes	                   0x2
#define buffer_64bytes	                   0x3
#define buffer_128bytes	                   0x4
#define buffer_256bytes                    0x5
#define buffer_512bytes                    0x6 
 
#define WordLength_8b                      0x0
#define WordLength_9b                      0x1

#define StopBits_1                         0x0                         
#define StopBits_0_5                       0x1                         
#define StopBits_2                         0x2                         
#define StopBits_1_5                       0x3                         

#define Parity_No                          0x0                         
#define Parity_Even                        0x1                         
#define Parity_Odd                         0x2  

#define HardwareFlowControl_None           0x0                         
#define HardwareFlowControl_RTS_CTS        0x1

#define Secure_WEP           			   0x0                         
#define Secure_WPA_WPA2_PSK                0x1
#define Secure_None                        0x2

#define  IO1_Normal							0x0
#define  IO1_Frame_Control					0x1
#define  IO1_485_TX_Enable					0x2

#define  DNS_Disable						0x0
#define  DNS_Enable							0x1

                         
typedef enum
{	TCP_stop      = 0x0,
    TCP_listening = 0x1,
	TCP_connected = 0x2,
	TCP_unknown   = 0xFF,
} EMW_TCPstatus_TypeDef;

typedef enum
{
  wifi_disconnected = 0x0,
  wifi_connected	= 0x5,
  wifi_unknown      = 0xFF,
} EMW_WiFistatus_TypeDef;

typedef enum
{
  CMD_mode = 0x0,
  DTU_mode = 0x1,
} EMW_mode_TypeDef;

typedef struct
{
	// WIFI
	u8 wifi_mode;		// Wlan802_11IBSS(0), Wlan802_11Infrastructure(1)
	u8 wifi_ssid[32];	// 
	u8 wifi_wepkey[16];	// 40bit and 104 bit
	u8 wifi_wepkeylen;	// 5, 13


	// TCP/IP
	u8 local_ip_addr[16];		// if em380 is server, it is server's IP;	if em380 is client, it is em380's default IP(when DHCP is disable)
	u8 remote_ip_addr[16];		// if em380 is server, it is NOT used;		if em380 is client, it is server's IP
	u8 net_mask[16];	// 255.255.255.0
	u8 gateway_ip_addr[16];        // gateway ip address
	u8 portH;			// High Byte of 16 bit
	u8 portL;			// Low Byte of 16 bit
	u8 connect_mode;	// 0:server  1:client
	u8 use_dhcp;		// 0:disale, 1:enable
	u8 use_udp;			// 0:use TCP,1:use UDP

	// COM
	u8 UART_buadrate;	// 0:9600, 1:19200, 2:38400, 3:57600, 4:115200
	u8 DMA_buffersize;	// 0:2, 1:16, 2:32, 3:64, 4:128, 5:256, 6:512
	u8 use_CTS_RTS;		// 0:disale, 1:enable
	u8 parity;			// 0:none, 1:even parity, 2:odd parity
	u8 data_length;		// 0:8, 1:9
	u8 stop_bits;		// 0:1, 1:0.5, 2:2, 3:1.5

	// DEVICE
	u8 IO_Control;		// 0 - 255
#ifdef 	EMW_FIRMWARE_UART
	u8 sec_mode;        // 0 = wep, 1=wpa psk, 2=none
    u8 wpa_psk[32];
#endif
} EMW_parm_TypeDef;

typedef struct
{
	int dns_enable ;// 1=use dns string, 0=use remote_ip string
	u8 dns[64]; 
	u8 dns_server_addr[16];
} EMW_DNS_TypeDef;


typedef struct
{
	u8 use_udp;                                     // 0:tcp     1:udp
	u8 connect_mode;                                // 0:server  1:client
	u8 portH;			                            // High Byte of 16 bit
	u8 portL;			                            // Low Byte of 16 bit
	u8 remote_ip_addr[64];		                    // if EMW is a server, it is NOT used; if EMW is a client, it is the server's IP address
} EMW_socket_TypeDef;

typedef struct
{
	u8 local_ip_addr[16];		// if em380 is server, it is server's IP;	if em380 is client, it is em380's default IP(when DHCP is disable)
	u8 gateway_ip_addr[16];        // gateway ip address
	u8 net_mask[16];	// 255.255.255.0
} EMW_IP_TypeDef;

typedef struct
{
  EMW_TCPstatus_TypeDef TCPstatus;
  EMW_WiFistatus_TypeDef  WiFistatus;
} EMW_status_TypeDef;

typedef struct 
{
  u8 Min_Power;
  u8 Max_Power;
  u8 Cur_Power;
} EMW_RF_POWER_TypeDef;

typedef struct 
{
  char   AP_NAME[20];
  float     AP_signal;
} EMW_APLst_TypeDef;


typedef enum
{
  TCP_server = 0x0,
  TCP_client = 0x1
} EMW_Mode_TypeDef;


//#define EM380ERROR -1

u8 EMW_Init(uint32_t BaudRate,uint16_t WordLength,uint16_t StopBits,uint16_t Parity,uint16_t CTSRTS,uint16_t buffer);
u8 EMW_InitWithDefault(void);

u8 EMW_Get_ver(u32* version);
u8 EMW_Get_status(EMW_status_TypeDef* EMW_status);
u8 EMW_Get_APList(EMW_APLst_TypeDef* EMW_APLst);
u8 EMW_Get_Strength(u8* SSID, u8 SSID_length, EMW_APLst_TypeDef* EMW_APLst);

u8 EMW_Get_Config(EMW_parm_TypeDef* EMW_Parm);
u8 EMW_Set_Config(EMW_parm_TypeDef* EMW_Parm);
u8 EMW_Get_DNS(EMW_DNS_TypeDef* EMW_dns);
u8 EMW_Set_DNS(EMW_DNS_TypeDef* EMW_dns);

u8 EMW_Get_RF_POWER(EMW_RF_POWER_TypeDef* RF_POWER);
u8 EMW_Set_RF_POWER(EMW_RF_POWER_TypeDef* RF_POWER);

u8 EMW_Open_Socket(EMW_socket_TypeDef* socket);
u8 EMW_Close_Socket(void);
u8 EMW_Open_Wifi(void);
u8 EMW_Close_Wifi(void);

u8 EMW_GET_IP(EMW_IP_TypeDef* IP);
u8 EMW_SET_IP(EMW_IP_TypeDef* IP);
u8 EMW_Set_Name(u8* name, u8 name_length);
u8 EMW_Get_Name(u8* name);
u8 EMW_Get_Mac(u8* mac);

u8 EMW_Data_transceive_mode(u8 data_mode);
u8 EMW_Send_Data(u8* Data,u32 len);
u8 EMW_Recv_Data(u8* Data);
u8 EMW_Set_Mode(EMW_mode_TypeDef mode);
u8 EMW_Startup(void);
u8 EMW_Reset(void);
u8 EMW_Reset_Delay(u16 second);

u8 EMW_Sleep(void);
u8 EMW_Wakeup(void);
void EMW_printf_config(void);



#endif
