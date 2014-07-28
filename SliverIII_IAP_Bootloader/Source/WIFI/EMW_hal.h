#ifndef __EM380C_HAL_H
#define __EM380C_HAL_H

#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>	


#define RCC_GPIO            RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA
#define RCC_UART            RCC_APB2Periph_USART1
#define status_pin          GPIO_Pin_7
#define status_port         GPIOC
#define wakeup_pin          GPIO_Pin_6  
#define wakeup_port         GPIOC

//#define int_pin             GPIO_Pin_5
//#define int_port            GPIOE

#define reset_pin           GPIO_Pin_8
#define reset_port          GPIOA

//#define data_ready_pin      GPIO_Pin_11
//#define data_ready_port     GPIOE

#define UART_rx_pin         GPIO_Pin_10
#define UART_rx_port        GPIOA

#define UART_tx_pin         GPIO_Pin_9
#define UART_tx_port        GPIOA

//#define UART_cts_pin        GPIO_Pin_11
//#define UART_cts_port       GPIOA

//#define UART_rts_pin        GPIO_Pin_12
//#define UART_rts_port       GPIOA

#define uart_irq			USART1_IRQn
#define uartx			    USART1
#define USART_IRQHandler    USART1_IRQHandler



/*	
#define RCC_SPI             RCC_APB2Periph_SPI1
#define SPI_sel_pin         GPIO_Pin_4
#define SPI_sel_port        GPIOA
#define SPI_sclk_pin        GPIO_Pin_5
#define SPI_sclk_port       GPIOA
#define SPI_miso_pin        GPIO_Pin_6
#define SPI_miso_port       GPIOA
#define SPI_mosi_pin        GPIO_Pin_7
#define SPI_mosi_port       GPIOA
#define spix			    SPI1
*/

u8 EMW_HAL_Init(uint32_t BaudRate,uint16_t WordLength,uint16_t StopBits,uint16_t Parity,uint16_t CTSRTS);
u8 EMW_HAL_InitWithDefault(void);
void Delay(__IO uint32_t nTime);
void SET_STATUS_PIN(void);
void RESET_STATUS_PIN(void);
void SET_WAKEUP_PIN(void);
void RESET_WAKEUP_PIN(void);
vs8 READ_INT_PIN(void);
void UART_send_buf(u8 *buf, int len);
int UART_receive_buf(u8 *buf);


#endif

