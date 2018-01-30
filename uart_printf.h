/******************** (C) COPYRIGHT 2016 NARA Controls Co. Ltd. ****************
* File Name          : UART.h
* Author             : °³¹ß2ÆÀ
* Date First Issued  : 
* Description        : 
*                      
********************************************************************************
* History:
*  0.1 : 
*  
*******************************************************************************
  COMMENTS:
*******************************************************************************/
#ifndef _UART_PRINTF_H_
#define _UART_PRINTF_H_

/*******************************************************************************
*                                                                            
* Header file definition                                                     
*                                                                            
*******************************************************************************/

/*******************************************************************************
*
* Local macro definition
*                       
*******************************************************************************/
/* UART , SINGLE UART ST16C2550, EXAR */

#define UART_TXRX_FIFO               1
#define UART_FIFO_TRIGGER            4

#define _PRINTF_BUF_LEN_ 			 126

#define UARTDEFS_MAX_QUEUE_BUFFER_TX   500
#define UARTDEFS_MAX_QUEUE_BUFFER_RX   100

/*******************************************************************************
*                                                                           
* Local struct definition                                                   
*                                                                           
*******************************************************************************/

typedef struct {
	uint16_t wTxFront;
	uint16_t wTxRear;
	unsigned char bTxBuffer[UARTDEFS_MAX_QUEUE_BUFFER_TX];
	uint16_t wRxFront;
	uint16_t wRxRear;
	unsigned char bRxBuffer[UARTDEFS_MAX_QUEUE_BUFFER_RX];
} hUART;

/*******************************************************************************
*
* Local function declaration                                                 
*
*******************************************************************************/

/*******************************************************************************
*
* Local variable declaration                                                
*
*******************************************************************************/
extern hUART ghUART;

/*******************************************************************************
*
* Local const variable defintion                                            
*
*******************************************************************************/

/*******************************************************************************
*
* External function declaration                                             
*
*******************************************************************************/
void UART_buf_init();
void UART_rxprocess(void);
void UART_txprocess(void);

void UART_putch (unsigned char ch);
//void UART_putch_i (unsigned char ch);
unsigned char UART_getch (void);
bool UART_readbyte (unsigned char *ch);
uint16_t UART_readdata (unsigned char *buf);

void UART_puts (unsigned char *str);
int UART_printf( const char *format, ... );
//void UART_printf_i( const char *format, ... );
void UART_flush();

#endif

/****************************************** END OF FILE ***********************************************/

