/******************************************************************************
    FILE NAME   : GUI_Comm.H
    AUTHOR      : SJeong
    DATE        : 2016.xx.xx ~ 2016.xx.xx
    REVISION    : Ver0.10
    DESCRIPTION : GUI Comm. Header.

 ******************************************************************************
    HISTORY     :


 ******************************************************************************/
 
#ifndef _GUI_COMM_H_
#define _GUI_COMM_H_



/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/
 
#define START	                0x7E

#define GUI_DATADIV_MAX         100


#define GUI_MAX_BUFFERSIZE      100
#define GUI_COMM16_CRCSIZE        1
#define GUI_DATA_MAX_WAIT       0xFFFF


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/
 
// Ctrl code
#define PACKET_FIRST		0
#define PACKET_ING			1
#define PACKET_END			2
#define PACKET_ONE_TIME		3

// ACK code
#define ACK_CMD				0
#define ACK_OK				1
#define ACK_FAIL			2


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

// CMD -- module control
#define CMD_STATUS_RD					0x01
#define CMD_CTRL_RD						0x03
#define CMD_CTRL_WR						0x04
#define CMD_CFG_RD						0x0A
#define CMD_CFG_WR						0x0B
#define CMD_CFG_SAVE					0x0F

// CMD -- Comm.
#define INIT_COMM_PORT					0x80
#define RUN_COMMUNICATION				0x81
#define STOP_COMMUNICATION				0x82

#define GUI_CMD_DISCONNECT              0x12


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

typedef enum
{
    GUIDEFS_COMM_STX,
    GUIDEFS_COMM_SEQ,
    GUIDEFS_COMM_LEN1,
    GUIDEFS_COMM_LEN2,
    GUIDEFS_COMM_CMD,
    GUIDEFS_COMM_ACK,
    GUIDEFS_COMM_CTRL,
    GUIDEFS_COMM_REV,

    GUIDEFS_COMM_DATA,
    GUIDEFS_COMM_HEADER_MAX = GUIDEFS_COMM_DATA
    
} GUIDEFS_COMM_HEADER_INX;


typedef enum
{
    GUIDEFS_COMM16_SEQ_STX,
    GUIDEFS_COMM16_LEN,
    GUIDEFS_COMM16_ACK_CMD,
    GUIDEFS_COMM16_REV_CTRL,

    GUIDEFS_COMM16_DATA,
    GUIDEFS_COMM16_HEADER_MAX = GUIDEFS_COMM16_DATA

} GUIDEFS_COMM16_HEADER_INX;



/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

typedef struct
{
	unsigned char u8Rx_DataTemp[GUI_MAX_BUFFERSIZE];
    uint16_t  u16Rx_Data[GUI_MAX_BUFFERSIZE];
    uint16_t  u16Tx_Data[GUI_MAX_BUFFERSIZE];	// char∞° 16bit¿”

    uint16_t u16Front;
    uint16_t u16Rear;
    
    uint16_t u16DataTmpSize;
    uint16_t u16DataSize;
    
    uint16_t u16Dummy;
    
} GUIDEFS_COMM_DAT;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

extern GUIDEFS_COMM_DAT        gdsGUI_Comm_Msg;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void GUI_Rx_Msg();
void GUI_Tx_Msg(unsigned char u8Seq, unsigned char u8Cmd, unsigned char u8Ack, unsigned char u8Ctrl, unsigned char * pbData, uint16_t u16Size, uint16_t u16Type);

void GUI_InitComm(void);
void GUI_Cmd_Process(unsigned char u8Cmd);

bool GUI_CheckCRC16(uint16_t *pbdata, uint32_t u32LenIncludeCRC, uint16_t u16RcvCRC);

void GUI_Rx_STX(void);


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

#endif

/*****************************[ End of Program ]*****************************/


