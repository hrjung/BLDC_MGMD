/******************************************************************************
    FILE NAME   : GUI_Comm.C
    AUTHOR      : SJeong
    DATE        : 2016.xx.xx ~ 2016.xx.xx
    REVISION    : Ver0.10
    DESCRIPTION : GUI Comm. Function.

 ******************************************************************************
    HISTORY     :


 ******************************************************************************/


#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "SoftTimer.h"
#include "uart_printf.h"
#include "GUI_Comm.h"
#include "GUI_Cmd.h"
#include "PCInterface.h"
#include "Console.h"


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/
 
GUIDEFS_COMM_DAT        gdsGUI_Comm_Msg;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/
#if(1)
const unsigned char CRCHiTemp[256] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

const unsigned char CRCLoTemp[256] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};
#else
static unsigned char CRCHiTemp[16];
static unsigned char CRCLoTemp[16];
#endif


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

uint32_t GUI_MakeCRC16(uint16_t *src, uint32_t dataLength)
{
	unsigned char uchCRCHi = 0xFF;
	unsigned char uchCRCLo = 0xFF;
	unsigned char uIndex;

	while(dataLength--)
	{
		// 하위 src (c2000은 char가 16bit)
		uIndex = (uchCRCHi ^ ((*src)&0xff));
		uchCRCHi = (uchCRCLo ^ CRCHiTemp[uIndex]);
		uchCRCLo = CRCLoTemp[uIndex];

		// 상위 src (c2000은 char가 16bit)
		uIndex = (uchCRCHi ^ ((*src++)>>8));
		uchCRCHi = (uchCRCLo ^ CRCHiTemp[uIndex]);
		uchCRCLo = CRCLoTemp[uIndex];
	}
	return (uchCRCHi << 8 | uchCRCLo);
}

bool GUI_CheckCRC16(uint16_t *pbdata, uint32_t u32LenIncludeCRC, uint16_t u16RcvCRC)
{
    uint16_t u16CalCRC = 0;

    u16CalCRC = GUI_MakeCRC16(pbdata, u32LenIncludeCRC-GUI_COMM16_CRCSIZE);

//    UART_printf("\n GUI Comm RcvCRC: %04X, CalCRC: %04X", u16RcvCRC, u16CalCRC);
    
	if(u16RcvCRC == u16CalCRC) 
		return true;
 
	return false;
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void GUI_Cmd_Process(unsigned char u8Cmd)
{

    STimer_Start(&ghSTIMER.GuiInUseTimer, CONSDEFS_GUI_IN_USE_TIME);    // GUI 사용 중임을 나타내는 타이머 시작
    
	switch(u8Cmd) {
		// CMD -- module control
		case CMD_STATUS_RD :				GUI_Status_Rd();								break;
		case CMD_CTRL_RD :					GUI_Ctrl_Rd();									break;
		case CMD_CTRL_WR :					GUI_Ctrl_Wr();									break;
		case CMD_CFG_RD :					GUI_Cfg_Rd();									break;
		case CMD_CFG_WR :					GUI_Cfg_Wr();									break;
		case CMD_CFG_SAVE :					GUI_Cfg_Save();									break;
		case GUI_CMD_DISCONNECT      :      GUI_Disconnect();                               break;

#if (0)
		// CMD -- up/download
		case GET_LOGIC_REV_ID :				FCS_GetLogicRevId();							break;
		case SET_LOGIC_REV_ID :				FCS_SetLogicRevId();							break;
		case UPLOAD_SYSTEM_CONF :															break;
		case DOWNLOAD_SYSTEM_CONF :															break;
		case DELETE_SYSTEM_CONF :															break;
		case UPLOAD_IO_CONF :				FCS_UploadIoConfig();							break;
		case DOWNLOAD_IO_CONF :				FCS_DownloadIoConfig();							break;
		case DELETE_IO_CONF :																break;
		case UPLOAD_LOGIC_CONF :			FCS_UploadLogicConfig();						break;
		case DOWNLOAD_LOGIC_CONF :			FCS_DownloadLogicConfig();                      break;
		case DELETE_LOGIC_CONF :															break;
		case UPLOAD_VAR_CONF :				FCS_UploadVarConfig();							break;
		case DOWNLOAD_VAR_CONF :			FCS_DownloadVarConfig();         				break;
		case DELETE_VAR_CONF :																break;

		case UPLOAD_LIF_FILE :				FCS_UploadLifConfig();							break;
		case DOWNLOAD_LIF_FILE :			FCS_DownloadLifConfig();                		break;
		case DELETE_LIF_FILE :																break;
		
		// CMD -- I/O CONTROL
		case RUN_INPUT_UPDATE :																break;
		case STOP_INPUT_UPDATE :															break;
		case RUN_ONCE_INPUT_UPDATE :														break;
		case HOLD_INPUT_VALUE :																break;
		case RUN_OUTPUT_UPDATE :															break;
		case STOP_OUTPUT_UPDATE	:															break;
		case RUN_ONCE_OUTPUT_UPDATE	:														break;
		case HOLD_OUTPUT_VALUE :															break;
		case OUT_LOW_REF_VALUE :															break;
		case OUT_HIGH_REF_VALUE :															break;
		
		// CMD -- LOGIC CONTROL
		case GET_ALL_VAR_VAL :				FCS_Svc_GetAllVarVal();			                break;
		case GET_LOGIC_PAGES_STATUS :		FCS_Svc_GetLogicPagesStatus();	                break;
		case SET_ALL_LOGIC_PAGES_RUN_STOP :	FCS_Svc_SetAllLogicPagesRunStop();              break;
		case SET_A_LOGIC_PAGE_RUN_STOP :	FCS_Svc_SetALogicPageRunStop();	                break;
		case GET_LOGIC_PAGE_BLKS_OUT :		FCS_Svc_GetLogicPageBlksOut();	                break;
		case GET_ALL_IO_VAL :				FCS_Svc_GetAllIoVal();			                break;
		case SET_OUT_VAL :					FCS_Svc_SetOutVal();							break;
		case GET_A_BLK_PARAS_VAL :			FCS_Svc_GetABlkParasVal();		                break;
		case SET_A_BLK_PARAS_VAL :			FCS_Svc_SetABlkParasVal();		                break;
		case CHECK_SCAN_BUFF_OVERLAY :									                    break;
		case GET_CPU_AVAILIBILITY :										                    break;
		case GET_ALL_MONITORING_DATA :		FCS_Svc_GetAllMonitoringData();	                break;

		// CMD -- Comm.
		case INIT_COMM_PORT :																break;
		case RUN_COMMUNICATION :															break;
		case STOP_COMMUNICATION :															break;

	
		/* MJC-101214 : PC 정보 인터페이스를 위한 CMD */
		case GUI_CMD_DISP_VERSION    :      GUI_Disp_Version();                             break;
		case GUI_CMD_DISP_MEASURE    :		GUI_Disp_Measure();                             break;
		case GUI_CMD_DISP_MEASURE2ND :		GUI_Disp_Measure2nd();                          break;
		case GUI_CMD_DISP_MONITOR    :		GUI_Disp_Monitoring();                          break;

        case GUI_CMD_EVENT_TABLE_STS :      GUI_EventTableStatus();                         break;
        case GUI_CMD_EVENT_TABLE_ALM :      GUI_EventTableAlarm();                          break;
        case GUI_CMD_EVENT_TABLE_CTL :      GUI_EventTableControl();                        break;

		case GUI_CMD_SET_SYSTEM_RD   :		GUI_System_Read();                              break;
		case GUI_CMD_SET_SYSTEM_WR   :		GUI_System_Write();                             break;
		case GUI_CMD_SET_IED_RD      :		GUI_IED_Read();                                 break;
		case GUI_CMD_SET_IED_WR      :		GUI_IED_Write();                                break;
		case GUI_CMD_SET_MON_RD      :		GUI_Mon_Read();                                 break;
		case GUI_CMD_SET_MON_WR      :		GUI_Mon_Write();                                break;
		case GUI_CMD_SET_MEASURE_RD  :		                               					break;
		case GUI_CMD_SET_MEASURE_WR  :		                               					break;
		case GUI_CMD_SET_CONTROL_RD  :		GUI_SetControl_Read();                          break;
		case GUI_CMD_SET_CONTROL_WR  :		GUI_SetControl_Write();                         break;

		case GUI_CMD_EVENT_MON_RD    :		GUI_MonitoringEvent();                          break;
		case GUI_CMD_EVENT_DI_RD     :		GUI_DIEvent();                                  break;
		case GUI_CMD_EVENT_CONTROL_RD:		GUI_ControlEvent();                             break;
		case GUI_CMD_EVENT_GOOSE_RD  :		GUI_GOOSEEvent();                               break;
		case GUI_CMD_EVENT_PROT_RD   :		GUI_ProtectionEvent();                          break;
		case GUI_CMD_EVENT_ALM_RD    :		GUI_AlarmEvent();                               break;
		#if( CURRDIFF_RELAY )
		case GUI_CMD_EVENT_PCM_RD    :		GUI_PCMEvent();                                 break;
        #endif
        
		case GUI_CMD_CTRL_SLD_RD     :		GUI_Ctrl_SLD_Read(GUI_CMD_CTRL_SLD_RD, ACK_OK); break;
		case GUI_CMD_CTRL_SLD_WR     :		GUI_Ctrl_SLD_Write();                           break;
		case GUI_CMD_CTRL_RD         :		GUI_Ctrl_Dev_Read(GUI_CMD_CTRL_RD, ACK_OK);     break;
		case GUI_CMD_CTRL_WR         :		GUI_Ctrl_Dev_Write();                           break;

        #if( CURRDIFF_RELAY )
		case GUI_CMD_CTRL_43SW_RD    :		GUI_Ctrl_43SW_Read(GUI_CMD_CTRL_43SW_RD, ACK_OK); break;
		case GUI_CMD_CTRL_43SW_WR    :		GUI_Ctrl_43SW_Write();                          break;
		#endif

		case GUI_CMD_MONITORING      :      GUI_Monitoring();                               break;
		case GUI_CMD_COMTRADE_HDR_RD :      GUI_Comtrade_HdrRead();                         break;
		case GUI_CMD_COMTRADE_CFG_RD :      GUI_Comtrade_CfgRead();                         break;
		case GUI_CMD_COMTRADE_DAT_RD :      GUI_Comtrade_DatRead();                         break;
		case GUI_CMD_COMTRADE_NUM_RD : 		GUI_Comtrade_NumRead();							break;
		#endif

		default:	break;
	}
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

bool GUI_SendComm(uint16_t *pbMsg, uint16_t u16Len)
{
	int i=0;


    for( i=0; i<u16Len; i++ )
    {
    	// char가 16비트임, sizeof는 16비트의 배수임
        UART_putch(*pbMsg&0xff);
        UART_putch(*pbMsg>>8);
        pbMsg++;
    }
    

	return true; 
}


void GUI_Tx_Msg(unsigned char u8Seq, unsigned char u8Cmd, unsigned char u8Ack, unsigned char u8Ctrl, unsigned char * pbData, uint16_t u16Size, uint16_t u16Type)
{
	uint16_t u16CalCRC = 0;
    uint16_t u16TxSize = 0;
    uint16_t u16Idx    = 0;

    memset( &gdsGUI_Comm_Msg.u16Tx_Data[0], 0,  GUI_MAX_BUFFERSIZE);
    u16TxSize = u16Size + GUIDEFS_COMM16_HEADER_MAX+GUI_COMM16_CRCSIZE;

    gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM16_SEQ_STX ] = (u8Seq<<8)|0x7E;
    gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM16_LEN] = u16TxSize<<1;			// C2000은 sizeof가 16bit단위 크기임 (8bit단위 크기로 변환)
    gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM16_ACK_CMD ] = (u8Ack<<8)|u8Cmd;
    gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM16_REV_CTRL] = u8Ctrl;

    u16Idx += (GUIDEFS_COMM16_REV_CTRL+1);
    if( u16Type != 0 )
    {
        gdsGUI_Comm_Msg.u16Tx_Data[u16Idx++] = u16Type;
        ++u16TxSize;	// type이 다르면 인덱스가 추가된다.

        gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM16_LEN] = (u16TxSize<<1);	// C2000은 sizeof가 16bit단위 크기임 (8bit단위 크기로 변환)

    }

    memcpy( &gdsGUI_Comm_Msg.u16Tx_Data[u16Idx], pbData, u16Size);
    
    u16CalCRC = GUI_MakeCRC16(&gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM16_SEQ_STX], u16TxSize-1);
    
    gdsGUI_Comm_Msg.u16Tx_Data[u16TxSize-1] = u16CalCRC;

    GUI_SendComm( &gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM16_SEQ_STX], u16TxSize );

}

#if (0)
void GUI_Tx_MsgComtrade(unsigned char u8Seq, unsigned char u8Cmd, unsigned char u8Ack, unsigned char u8Ctrl, unsigned char * pbData, uint16_t u16Size, uint16_t u16Flag, dsTimeInfo Time)
{
    uint16_t u16CalCRC = 0;
    uint16_t u16TxSize = 0;
    uint16_t u16Idx    = 0, i;

    memset( &gdsGUI_Comm_Msg.u16Tx_Data[0], 0,  GUI_MAX_BUFFERSIZE);
    u16TxSize = u16Size + GUIDEFS_COMM_HEADER_MAX+GUI_COMM_CRCSIZE;

    gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM_STX ] = 0x7E;
    gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM_SEQ ] = u8Seq;
    gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM_LEN1] = (u16TxSize&0xFF);
    gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM_LEN2] = ((u16TxSize>>8)&0xFF);;
    gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM_CMD ] = u8Cmd;
    gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM_ACK ] = u8Ack;
    gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM_CTRL] = u8Ctrl;
    gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM_REV ] = 0x00;

    u16Idx += (GUIDEFS_COMM_REV+1);
    if( u16Flag != 0 )
    {
        memcpy( &gdsGUI_Comm_Msg.u16Tx_Data[u16Idx], &Time, sizeof(dsTimeInfo) );
        u16Idx    = u16Idx+sizeof(dsTimeInfo);
        u16TxSize = u16TxSize+sizeof(dsTimeInfo);;

        gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM_LEN1] = (u16TxSize&0xFF);
        gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM_LEN2] = ((u16TxSize>>8)&0xFF);;

    }

    memcpy( &gdsGUI_Comm_Msg.u16Tx_Data[u16Idx], pbData, u16Size);
    
    u16CalCRC = FCS_MakeCRC16(&gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM_STX], u16TxSize-2);
    
    gdsGUI_Comm_Msg.u16Tx_Data[u16TxSize-2] = (u16CalCRC&0xFF);
    gdsGUI_Comm_Msg.u16Tx_Data[u16TxSize-1] = ((u16CalCRC>>8)&0xFF);

  	if( ghCDF.u16CommFlag & VIEW_FLAG_COMM_ETH_TX_EVENT  ) 
  	{
  	        UART_printf("\n Eth TX :");
  	    for( i=0; i<u16TxSize; i++)
  	    {
  	        if( i%16==0)
  	        {
  	            UART_printf("\n");
  	            //ClearWatchdog();
  	        }
  	        UART_printf(" %02X", gdsGUI_Comm_Msg.u16Tx_Data[i]);
  	    }
  	}

    GUI_SendComm( &gdsGUI_Comm_Msg.u16Tx_Data[GUIDEFS_COMM_STX], u16TxSize );

}
#endif

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/
enum {RX_IDLE, RX_STX, RX_LEN, RX_WAIT};
uint16_t gRx_state = RX_IDLE;
uint16_t gRx_TryCnt = 0;


void GUI_Rx_Msg()
{
    uint16_t  u16Length = 0, u16CRCIndex, i, size = 0;
    uint16_t  u16RcvCRC, Len1_Idx = 0, Len2_Idx = 0;
    unsigned char u8Buf[UARTDEFS_MAX_QUEUE_BUFFER_RX]={0,};
    bool bResult;

    u16RcvCRC = 0;
    bResult   = false;

	switch (gRx_state) {

		case RX_IDLE :
			if (!UART_readbyte(u8Buf))
				return;
			if (u8Buf[0] == START) {
				gdsGUI_Comm_Msg.u8Rx_DataTemp[gdsGUI_Comm_Msg.u16Front] = u8Buf[0];
				gdsGUI_Comm_Msg.u16Front = (gdsGUI_Comm_Msg.u16Front+1) % GUI_MAX_BUFFERSIZE;
				gRx_TryCnt = 0;
				gRx_state = RX_STX;
			}
			break;

		case RX_STX:

			u16Length = UART_readdata( u8Buf);
			if (u16Length) {
				for( i=0; i<u16Length; i++)
				{
					gdsGUI_Comm_Msg.u8Rx_DataTemp[gdsGUI_Comm_Msg.u16Front] = u8Buf[i];
					gdsGUI_Comm_Msg.u16Front = (gdsGUI_Comm_Msg.u16Front+1) % GUI_MAX_BUFFERSIZE;
				}
			}

			if( gdsGUI_Comm_Msg.u16Front > gdsGUI_Comm_Msg.u16Rear )
				size = gdsGUI_Comm_Msg.u16Front - gdsGUI_Comm_Msg.u16Rear;
			else
				size = (gdsGUI_Comm_Msg.u16Front+GUI_MAX_BUFFERSIZE) - gdsGUI_Comm_Msg.u16Rear;

			// 통신 명령어 길이 읽기
			if( size >= (GUIDEFS_COMM_DATA+1) )
			{
				Len1_Idx = (gdsGUI_Comm_Msg.u16Rear+GUIDEFS_COMM_LEN1) %GUI_MAX_BUFFERSIZE;
				Len2_Idx = (gdsGUI_Comm_Msg.u16Rear+GUIDEFS_COMM_LEN2) %GUI_MAX_BUFFERSIZE;

				// 패킷의 길이 필드를 DataSize에 저장
				gdsGUI_Comm_Msg.u16DataTmpSize = (gdsGUI_Comm_Msg.u8Rx_DataTemp[Len1_Idx] | (gdsGUI_Comm_Msg.u8Rx_DataTemp[Len2_Idx]<<8));
				gdsGUI_Comm_Msg.u16DataSize = gdsGUI_Comm_Msg.u16DataTmpSize>>1;// C2000 size로 변환
				gRx_TryCnt = 0;
				gRx_state = RX_LEN;
			} else {
				++gRx_TryCnt;
				// 재시도 횟수 초과하면
				if (gRx_TryCnt > GUI_DATA_MAX_WAIT){
					gRx_state = RX_IDLE;
				}
			}
			break;

		case RX_LEN :

			u16Length = UART_readdata( u8Buf);
			if (u16Length) {
				for( i=0; i<u16Length; i++)
				{
					gdsGUI_Comm_Msg.u8Rx_DataTemp[gdsGUI_Comm_Msg.u16Front] = u8Buf[i];
					gdsGUI_Comm_Msg.u16Front = (gdsGUI_Comm_Msg.u16Front+1) % GUI_MAX_BUFFERSIZE;
				}
			}

			if( gdsGUI_Comm_Msg.u16Front > gdsGUI_Comm_Msg.u16Rear )
				size = gdsGUI_Comm_Msg.u16Front - gdsGUI_Comm_Msg.u16Rear;
			else
				size = (gdsGUI_Comm_Msg.u16Front+GUI_MAX_BUFFERSIZE) - gdsGUI_Comm_Msg.u16Rear;

			// 정해진 길이 만큼 수신을 다 했으면
			if( size >= gdsGUI_Comm_Msg.u16DataTmpSize )
			{
				// tmprxbuf에서 Rxbuf로 복사 (8bit → 16bit변환)
				for( i=0; i<(gdsGUI_Comm_Msg.u16DataSize); i++)
				{
					gdsGUI_Comm_Msg.u16Rx_Data[i] = (gdsGUI_Comm_Msg.u8Rx_DataTemp[gdsGUI_Comm_Msg.u16Rear+1]<<8)|gdsGUI_Comm_Msg.u8Rx_DataTemp[gdsGUI_Comm_Msg.u16Rear];
					gdsGUI_Comm_Msg.u16Rear      = (gdsGUI_Comm_Msg.u16Rear+2) % GUI_MAX_BUFFERSIZE;
				}

				// crc체크
				u16CRCIndex = ( gdsGUI_Comm_Msg.u16DataSize-1 );
				u16RcvCRC   = ( gdsGUI_Comm_Msg.u16Rx_Data[u16CRCIndex]);

				bResult     = GUI_CheckCRC16( &gdsGUI_Comm_Msg.u16Rx_Data[GUIDEFS_COMM16_SEQ_STX], gdsGUI_Comm_Msg.u16DataSize, u16RcvCRC);

				//bResult = true;

				if( bResult )
				{
					gdsGUI_Comm_Msg.u16DataSize = gdsGUI_Comm_Msg.u16DataSize-GUIDEFS_COMM16_HEADER_MAX-GUI_COMM16_CRCSIZE;
					// 명령어 처리
					GUI_Cmd_Process(gdsGUI_Comm_Msg.u16Rx_Data[GUIDEFS_COMM16_ACK_CMD]&0xff);
					gRx_state = RX_IDLE;
				} else {
					gRx_state = RX_IDLE;
				}
			} else {
				++gRx_TryCnt;
				// 재시도 횟수 초과하면
				if (gRx_TryCnt > GUI_DATA_MAX_WAIT){
					gRx_state = RX_IDLE;
				}
			}
			break;
    }
}

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void GUI_Rx_STX(void){
	ghCDF.u16GuiFlag = GUI_IN_USE;
	gdsGUI_Comm_Msg.u8Rx_DataTemp[gdsGUI_Comm_Msg.u16Front] = 0x7e;
	gdsGUI_Comm_Msg.u16Front = (gdsGUI_Comm_Msg.u16Front+1) % GUI_MAX_BUFFERSIZE;
	gRx_TryCnt = 0;
	gRx_state = RX_STX;
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void GUI_InitComm(void)
{

	memset( &gdsGUI_Comm_Msg, 0, sizeof(GUIDEFS_COMM_DAT) );
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/


/*****************************[ End of Program ]******************************/

