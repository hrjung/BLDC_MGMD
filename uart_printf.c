/******************** (C) COPYRIGHT 2016 NaraControls Co. Ltd. ********************
* File Name          : UART_printf.c
* Author             : ����2��
* Date First Issued  : 
* Description        : 
*                      
********************************************************************************
* History:
*  0.1 : 
*  
*  
*******************************************************************************
  COMMENTS:
*******************************************************************************/


/*******************************************************************************
*                                                                            
* Header file definition                                                     
*                                                                            
*******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "PeripheralHeaderIncludes.h"
//#include <xdc/runtime/System.h>

#include "uart_printf.h"

/*******************************************************************************
*
* Local macro definition
*                       
*******************************************************************************/

/*****************************************************************************
 **  UART PRAM : Receive Buffer Descriptor Control for Async. Definitions    **
 *****************************************************************************/

#define  RBD_EMPTY                 0x8000  /* Buffer empty                        */
#define  RBD_EXT_BUFF              0x4000  /* External Buffer                     */
#define  RBD_WRAP_BIT              0x2000  /* Wrap Bit                            */
#define  RBD_INT_RECEPT            0x1000  /* Interrupt on Reception              */
#define  RBD_BC_CONT_CHAR          0x0800  /* Buffer contains Control Character   */
#define  RBD_BC_ADDR_CHAR          0x0400  /* Buffer contains Address Character   */
#define  RBD_BC_ADDR_MATCH         0x0200  /* Buffer contains Address Match       */
#define  RBD_IDLES                 0x0100  /* Buffer closed due to Idles          */
#define  RBD_BREAK_RX              0x0020  /* Break received                      */
#define  RBD_FRAME_ERR             0x0010  /* Framing Error : UART & DDCMP        */
#define  RBD_PARITY_ERR            0x0008  /* Parity Error : UART & DDCMP         */
#define  RBD_OVERRUN               0x0002  /* Overrun  : UART & DDCMP             */
#define  RBD_CARRIER_DECTET_LOST   0x0001  /* Carrier Detect lost  : UART & DDCMP */


/*****************************************************************************
 **  UART PRAM : Transmit Buffer Descriptor Control for Async. Definitions   **
 *****************************************************************************/

#define  TBD_READY             0x8000  /* Ready for transmit or busy          */
#define  TBD_EXT_BUFF          0x4000  /* External Buffer                     */
#define  TBD_WRAP_BIT          0x2000  /* Wrap Bit                            */
#define  TBD_INT_COMP          0x1000  /* Interrupt on Completion             */
#define  TBD_CTS_REPORT        0x0800  /* Clear-to-Send Report                */
#define  TBD_ADDR_CHAR         0x0400  /* Address Characters in Buffer        */
#define  TBD_UART_SPS          0x0200  /* UART sends Preamble Sequence        */
#define  TBD_CTS_LOST          0x0001  /* CTS lost                            */


#define ASC_CR 0x0D		/* keyboard = Enter */
#define ASC_LF 0x0A
#define ASC_BS 0x08

#define __DBG_SCC_MSG__

/*******************************************************************************
*                                                                           
* Local struct definition                                                   
*                                                                           
*******************************************************************************/

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

/*******************************************************************************
*
* External Variable declaration                                             
*
*******************************************************************************/

/*******************************************************************************
*
* Global Variable declaration                                             
*
*******************************************************************************/

hUART ghUART;

void UART_buf_init ()
{
	ghUART.wTxFront = 0;
	ghUART.wTxRear = 0;
	ghUART.wRxFront = 0;
	ghUART.wRxRear = 0;
}

/****************************************************/
/*  UART TX �۽� ó�� ���μ���  (Idle ISR���� ȣ���)        */
void UART_txprocess ()
{
	// Uart_write�Լ��� �����Ѵ�.
	uint16_t i;

	if (SciaRegs.SCIFFTX.bit.TXFFST == 0){	// FIFO�� ���� �ѹ��� ����
		for ( i = 0; i < 4; ++i ) {      // FIFO���� ���ɰ��� ��ŭ ó��
			if( ghUART.wTxFront != ghUART.wTxRear ) { // ť�� �����Ͱ� ������
				SciaRegs.SCITXBUF = ghUART.bTxBuffer[ ghUART.wTxRear];
				ghUART.wTxRear = ((ghUART.wTxRear+1) % UARTDEFS_MAX_QUEUE_BUFFER_TX);
			} else {
				break;
			}
		}
	}
}

#if (1)
/************************************************/
/*  UART RX ���� ó�� ���μ���                   */
void UART_rxprocess ()
{
    uint16_t i;

    /* Polling method for UART testing*/
    for (i = 0; i < 4; ++i ){
        if (SciaRegs.SCIFFRX.bit.RXFFST) {
            ghUART.bRxBuffer[ ghUART.wRxFront ] = SciaRegs.SCIRXBUF.all;
            ghUART.wRxFront = ((ghUART.wRxFront+1) % UARTDEFS_MAX_QUEUE_BUFFER_RX);
    	} else {
    	    break;
    	}
	}

}

/********************************/
/*   UART getch                 */
/*   �������� ���� �� null�� ������        */
unsigned char UART_getch (void)
{
	unsigned char cCh;

	if( ghUART.wRxFront != ghUART.wRxRear ) {
	   cCh = (unsigned char) ghUART.bRxBuffer[ ghUART.wRxRear ];
	   ghUART.wRxRear = ((ghUART.wRxRear+1) % UARTDEFS_MAX_QUEUE_BUFFER_RX);
	} else {
	   cCh = ('\0');
	}

	return( cCh );

}

/*********************************/
/*   UART readch                 */
/*   ���� ������ ������                          */
bool UART_readbyte (unsigned char *ch)
{

	if( ghUART.wRxFront != ghUART.wRxRear ) {
		*ch = (unsigned char) ghUART.bRxBuffer[ ghUART.wRxRear ];
		ghUART.wRxRear = ((ghUART.wRxRear+1) % UARTDEFS_MAX_QUEUE_BUFFER_RX);
		return (true);
	} else {
		*ch = ('\0');
		return (false);		/* return NULL */
	}

}

/*******************************/
/*   UART readch                 */
uint16_t UART_readdata (unsigned char *buf )
{

	uint16_t u16Cnt = 0;
	uint16_t loop;

	if( ghUART.wRxFront == ghUART.wRxRear )
	{
		return( 0 );
	}

	for(loop=0; (ghUART.wRxFront != ghUART.wRxRear) && (loop < UARTDEFS_MAX_QUEUE_BUFFER_RX); loop++)
	{
		buf[ u16Cnt++ ] =  ghUART.bRxBuffer[ ghUART.wRxRear++];
		ghUART.wRxRear = ((ghUART.wRxRear) % UARTDEFS_MAX_QUEUE_BUFFER_RX );
	}

	return( u16Cnt );

}

#else

/********************************/
/*   UART getch                 */
/*   �������� ���� �� null�� ������        */
unsigned char UART_getch (void)
{
    unsigned char ch;

    //if ( SciaRegs.SCIRXST.bit.RXRDY ) {
    if (SciaRegs.SCIFFRX.bit.RXFFST) {
    	ch = SciaRegs.SCIRXBUF.all;
    	return (ch);
	}else {
		return (unsigned char)0;		/* return NULL */
	}

}

/*********************************/
/*   UART readch                 */
/*   ���� ������ ������                          */
bool UART_readbyte (unsigned char *ch)
{
	//if (SciaRegs.SCIRXST.bit.RXRDY) {
    if (SciaRegs.SCIFFRX.bit.RXFFST) {
		*ch = SciaRegs.SCIRXBUF.all;
		return (true);
	} else {
		*ch = 0;
		return (false);		/* return NULL */
	}
}

/*******************************/
/*   UART readch                 */
uint16_t UART_readdata (unsigned char *buf, uint16_t cnt)
{
	uint16_t rd_cnt = 0;

//	while ((SciaRegs.SCIRXST.bit.RXRDY) && (rd_cnt < cnt)) {
	while ((SciaRegs.SCIFFRX.bit.RXFFST) && (rd_cnt < cnt)) {
			buf[rd_cnt++] = SciaRegs.SCIRXBUF.all;
	}
	return (rd_cnt);
}

#endif

// #pragma CODE_SECTION (UART_readdata, ".text_far") /* ���ֽ��� �ǹǷ� ���� �޸𸮷� */

/********************************/
/*   UART putch                  */
/*   ����� �����͸� ť�� ����  */
void UART_putch (unsigned char ch)
{
    /*uint16_t i;*/

    ghUART.bTxBuffer[ ghUART.wTxFront ] = ch;

    ghUART.wTxFront = ((ghUART.wTxFront+1) % UARTDEFS_MAX_QUEUE_BUFFER_TX);

}

#if (0)
/****************************************/
/*   UART putch_i                        */
/*   ��� ���(������ ���� ��� ���)   */
void UART_putch_i (unsigned char ch)
{
    uint16_t i;

    // �۽Ű��ɱ��� ���
    for( i=0; i<10000; i++)
    {
        if( CSL_FEXT(uart1Regs->LSR, UART_LSR_THRE) ) {  // regLSR & LSR_THR_EMPTY
            // �����Ϳ� �ִ´�
        	uart1Regs->THR = (unsigned char) (ch & 0xFF);
            return;
        }
    }

}
#endif

/*************************************/
/*   UART puts                        */
/*   ��Ʈ�� ���                     */
void UART_puts (unsigned char *pbuf)
{
	while (*pbuf != 0) {
		if (*pbuf == ASC_CR) UART_putch(ASC_LF);
		UART_putch(*pbuf++);
	}
}


/*************************************/
/*   UART printf                      */
/*   ��Ʈ�� ���� ���� ���           */

char imsi[_PRINTF_BUF_LEN_];

int UART_printf( const char *format, ... )
{
     va_list args;
     uint16_t  i;

     va_start( args, format);
     vsprintf (imsi, format, args);
     va_end( args );

     for(i=0;imsi[i]!=NULL && i< _PRINTF_BUF_LEN_ ;i++)
     {
 		if(imsi[i]=='\n')
             UART_putch('\r');

 		UART_putch(imsi[i]);
     }
     return(true);
}

#if (0)
 /*************************************/
 /*   UART printf_i                    */
 /*   ��Ʈ�� ���� ���� ��� ���      */
 void UART_printf_i( const char *format, ... )
 {
     va_list args;
     uint16_t  i;
     char imsi[_PRINTF_BUF_LEN_];

     va_start( args, format);
     vsprintf( imsi, format, args );
     va_end( args );

     for(i=0;imsi[i]!=NULL && i< _PRINTF_BUF_LEN_ ;i++)
     {
 		if(imsi[i]=='\n')
             UART_putch_i('\r');

 		UART_putch_i(imsi[i]);
     }
 }
#endif

 void UART_flush() {
	 while(ghUART.wTxFront != ghUART.wTxRear);
 }
/****************************************** END OF FILE ***********************************************/

