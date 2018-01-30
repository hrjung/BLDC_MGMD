/******************************************************************************
 **                  Copyright(C) 2016 Nara Control. Ltd.                  **
 **                  --------------------------------------                  **
 ******************************************************************************
    FILE NAME   : Console.H
    AUTHOR      : 
    DATE        : 
    REVISION    : 
    DESCRIPTION : Console Interface Header(CLI)

 ******************************************************************************
    HISTORY     :


 ******************************************************************************/

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

/******************************************************************************
 **                                                                          **
 **  Version Definitions                                                         **
 **                                                                          **
 ******************************************************************************/

#define BOARD_NAME	"IMDB R1.0"
#define BTP_VER0     1
#define BTP_VER1     0

/******************************************************************************
 **                                                                          **
 **  Key Definitions                                                         **
 **                                                                          **
 ******************************************************************************/

#define KEYDEFS_CTRL_C               0x03        
#define KEYDEFS_BS                   0x08        
#define KEYDEFS_TAB                  0x09        
#define KEYDEFS_LF                   0x0A        
#define KEYDEFS_CR                   0x0D        
#define KEYDEFS_ENTER                KEYDEFS_CR  
#define KEYDEFS_CANCEL               0x18        
#define KEYDEFS_CTRL_S               0x13        
#define KEYDEFS_ESC                  0x1B        
#define KEYDEFS_SPACE                0x20        
#define KEYDEFS_DOT                  0x2E

#define KEYDEFS_AMPERSAND            0x26        
#define KEYDEFS_AST                  0x2A        
#define KEYDEFS_SEMICOLON            0x3B        
#define KEYDEFS_STX                  0x7E        


/*****************************************************************************
 **                                                                         **
 **  콘솔관련 소프트 타이머 정의                                            **
 **                                                                         **
 *****************************************************************************/

#define CONSDEFS_DEBUG_MODE_CLEAR_TIME  (MACRO_MINUTES_STIMER(30))
#define CONSDEFS_PORT_REINIT_TIME       (MACRO_DAYS_STIMER(1))
#define CONSDEFS_GUI_IN_USE_TIME        (MACRO_MINUTES_STIMER(1))


/*****************************************************************************
 **                                                                         **
 **  콘솔관련 문자상수 정의                                                 **
 **                                                                         **
 *****************************************************************************/

#define CONSDEFS_MAX_BUFFER             128         

#define CONSDEFS_MAX_ARGMENT            64          
#define CONSDEFS_MAX_PARAMETER          6           
#define CONSDEFS_MAX_USER_FT            128         


/*****************************************************************************
 **                                                                         **
 **  Argment type definitions                                               **
 **                                                                         **
 *****************************************************************************/

#define CONSDEFS_TOKEN_TYPE_EOL      1      /* EOL: End of Line         */
#define CONSDEFS_TOKEN_TYPE_ARG      2      /* ARG: Argument            */


/*****************************************************************************
 **                                                                         **
 **  memory function constant variable definitions                          **
 **                                                                         **
 *****************************************************************************/

#define CONSDEFS_BASE_MSIZE          0x50L


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

typedef enum
{
    CONSDEFS_DATA_TYPE_BYTE,                /* Data type: uint8_t(1Byte)   */
    CONSDEFS_DATA_TYPE_WORD,                /* Data type: uint16_t(2Byte)   */
    CONSDEFS_DATA_TYPE_LONG                 /* Data type: uint32_t(4Byte)   */
}
CONSDEFS_DATA_TYPES;


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

typedef struct
{
	unsigned char      *pu8Command;
	unsigned char      *pu8Message;
    void    (*ft)();
    uint16_t     u16ParameterNo;
    uint16_t     u16Dummy;
}
CONS_USER_FUNCTION;


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

#define PASSWORD_OPERATOR              "IMDX"
#define PASSWORD_SUPERVISOR            "IMDMT"
#define PASSWORD_DEVELOPER             "IMDRND"

#define PASSCODE	2000

/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

typedef enum
{
    CONSDEFS_DBG_LEVEL_USER,
    CONSDEFS_DBG_LEVEL_OPERATOR,
    CONSDEFS_DBG_LEVEL_SUPERVISOR,
    CONSDEFS_DBG_LEVEL_DEVELOPER,
    CONSDEFS_DBG_LEVEL_MAX
}
CONSDEFS_DEBUG_LEVEL;


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

typedef struct
{
    uint32_t     u32StartAddr;

    unsigned char      u8RxBuffer   [CONSDEFS_MAX_BUFFER];
    unsigned char      u8TokenBuffer[CONSDEFS_MAX_BUFFER];
    unsigned char      u8OldBuffer  [CONSDEFS_MAX_BUFFER];

    bool    bOldFlag;

    uint16_t     u16RxCount;
    uint16_t     u16TokenCount;
    uint16_t     u16ArgmentNo;
    uint16_t     u16CommandCount;

    unsigned char      *pu8RxBuffer;
    unsigned char      *pu8TokenBuffer;

    unsigned char      u8DeveloperMode;
    unsigned char      u8LastCommand;
    uint16_t     u16OldCount;

    unsigned char      u8PassChar;
    unsigned char      u8PassCheckCount;
    unsigned char      u8PassKeyInCount;

    unsigned char      u8PassModeBuffer[6];

    unsigned char      u8PassKeyBuffer[CONSDEFS_MAX_BUFFER];

    uint16_t     u16DispAlways;
    uint16_t     u16TargetPage;

    CONS_USER_FUNCTION  Command[ CONSDEFS_MAX_USER_FT ];

}
GLOBAL_HANDLE_CONSOLE_VAR;


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

extern GLOBAL_HANDLE_CONSOLE_VAR    ghCON;


/*****************************************************************************
 **                                                                         **
 **  DSP MODULE INTERFACE 디버그 문자상수 선언                              **
 **                                                                         **
 *****************************************************************************/

 #define VIEW_FLAG_CS_NONE                0x0000
 #define VIEW_FLAG_CS_SET_LV1             0x0001
 #define VIEW_FLAG_CS_SET_LV2             0x0002
 #define VIEW_FLAG_CS_CTRL                0x0004


 #define VIEW_FLAG_CS_EVENT_MASK          0x0007

/*****************************************************************************
 **                                                                         **
 **  ADC MODULE INTERFACE 디버그 문자상수 선언                              **
 **                                                                         **
 *****************************************************************************/

 #define VIEW_FLAG_ADC_NONE                0x0000
 #define VIEW_FLAG_ADC_SET                 0x0001
 #define VIEW_FLAG_ADC_EVENT               0x0002

 #define VIEW_FLAG_ADC_EVENT_MASK          0x0003


/*****************************************************************************
 **                                                                         **
 **  Input Output Module Flag 디버그 문자상수 선언                          **
 **                                                                         **
 *****************************************************************************/

 #define VIEW_FLAG_DIO_NONE                 0x0000
 #define VIEW_FLAG_DIO_SET                	0x0001
 #define VIEW_FLAG_DIO_IN                	0x0002
 #define VIEW_FLAG_DIO_OUT		         	0x0004
 #define VIEW_FLAG_DIO_HOUT		         	0x0008

 #define VIEW_FLAG_DIO_EVENT_MASK           0x000f


/*****************************************************************************
 **                                                                         **
 **  EVT Interface 디버그 문자상수 선언                          **
 **                                                                         **
 *****************************************************************************/

 #define VIEW_FLAG_ALM_NONE                 0x0000
 #define VIEW_FLAG_ALM_EVENT_LV1           	0x0001
 #define VIEW_FLAG_ALM_EVENT_LV2           	0x0002

 #define VIEW_FLAG_ALM_EVENT_MASK           0x0003

/*****************************************************************************
 **                                                                         **
 **  MCU Interface 디버그 문자상수 선언                          **
 **                                                                         **
 *****************************************************************************/

 #define VIEW_FLAG_MCU_NONE                 0x0000
 #define VIEW_FLAG_MCU_RD_EVENT             0x0001
 #define VIEW_FLAG_MCU_WR_EVENT             0x0002

 #define VIEW_FLAG_MCU_EVENT_MASK           0x0003

/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

#define VIEWFLAG_BTP_DEFAULT	0x0000



/*****************************************************************************
 **                                                                         **
 **  GUI용 정의                                                                                                              **
 **                                                                         **
 *****************************************************************************/

#define TERMINAL_IN_USE                     0
#define GUI_IN_USE                          1

/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

 typedef enum
 {
    VIEWFLAG_BTP,
    VIEWFLAG_ADC,
    VIEWFLAG_DIO,

    VIEWFLGA_MAX           /* [03] */
 }
 CONSDEFS_CDF_NO_DEFS;

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/
 typedef struct
 {
	uint16_t     u16GuiFlag;
    uint16_t     u16DbgFlag[5];
 }
 CONSOLE_DEBUG_FLAG;

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

extern CONSOLE_DEBUG_FLAG    ghCDF;
extern uint16_t 			 gu16ModeFlag;
extern bool 				 gbMonitorADC;

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void ViewProcess();

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_Initialize(void);

void CONSOLE_SerialProcess(unsigned char u8Char);

void CONSOLE_RegistUserFunction(void);

void CONSOLE_SetUserFunction(char *pu8Command, void (*ft)(), char *pu8Message, uint16_t u16ParameterNo);

void CONSOLE_SplitUserFunction(void);

void CONSOLE_CommandExecute(void);

void MENU_CommandExecute(void);


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

unsigned char   CONSOLE_GetToken(unsigned char **ppu8RxBuffer);
bool CONSOLE_IsArgment(char cChar);
void CONSOLE_CallUserFunction(unsigned char **ppu8Command, uint16_t u16ArgmentNo);
int32_t  CONSOLE_GetData( unsigned char *pu8Char );
float  CONSOLE_GetfData( unsigned char *pu8Char );
unsigned char   CONSOLE_KeyInputData(uint32_t *pu32Value, unsigned char u8DataType);
void CONSOLE_HelpMsgDisplay( uint32_t u32No );

/*****************************************************************************
 **                                                                         **
 **  VT100 command prototype definitions                                    **
 **                                                                         **
 *****************************************************************************/

void CONSOLE_Prompt(void);
void CONSOLE_DispAccessCodeErrMsg(void);
void CONSOLE_Logo(void);

void CONSOLE_LineFeed(void);
void CONSOLE_SpacePrint( unsigned char u8Space );
void CONSOLE_CharPrint( unsigned char u8Space, char cCh, unsigned char u8Number, unsigned char u8LineFeedMode );

void CONSOLE_Gotoxy(unsigned char u8X, unsigned char u8Y);
void CONSOLE_ClearScreen(void);


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/
void CONSOLE_VersionDisp(uint32_t u32Flag);
void CONSOLE_ParameterDisp(void);
void CONSOLE_SetViewFlag( uint32_t u32Mode, uint32_t u32Value );
void Display_MonitorMask(void);
void Dbg_printf(  uint32_t u32Mode, uint32_t u32Value, const char *format, ... );

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_DisplayMemoryMap(void);

unsigned char CONSOLE_IsReadMemory(uint32_t u32Address);
unsigned char CONSOLE_IsWriteMemory(uint32_t u32Address);
unsigned char CONSOLE_IsFlashMemory(uint32_t u32Address);

void CONSOLE_MemoryDump(uint32_t u32StartAddress, uint32_t u32Length);
void CONSOLE_MemoryFill(uint32_t u32StartAddress, uint32_t u32Length, uint32_t u32Value);
void CONSOLE_MemoryCheck(uint32_t u32StartAddress, uint32_t u32Length);
void CONSOLE_MemoryEdit(uint32_t u32StartAddress);

void CONSOLE_MemDump(uint32_t u32StartAddress, uint32_t u32Length);
void CONSOLE_MemFill(uint32_t u32StartAddress, uint32_t u32EndAddress, unsigned char u32Value);
unsigned char CONSOLE_MemCheck(uint32_t u32StartAddress, uint32_t u32Length);
void CONSOLE_MemEdit( int32_t u32StartAddress );

void CONSOLE_MemoryMapDisp(void);
void CONSOLE_FlashTest(void);
void CONSOLE_SaveCfg(void);
void CONSOLE_LoadCfg(uint32_t u32Dflt );
void CONSOLE_Factory( uint32_t BoardType, uint32_t MotorType );
void CONSOLE_Reset();
void CONSOLE_SCIboot();

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_SetDebugMode(void);


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/
void CONSOLE_CtrlFlag (char* sbuf0, char* sbuf1);
void CONSOLE_SpeedPid(char* sbuf0, char* sbuf1, char* sbuf2);
void CONSOLE_CurrentPid(char* sbuf0, char* sbuf1, char* sbuf2);
void CONSOLE_PoleFreqCurrent(char* sbuf0, char* sbuf1, char* sbuf2);
void CONSOLE_Slop(uint32_t u32AccTime, uint32_t u32DecTime);
void CONSOLE_SpeedRef(char* sbuf);
void CONSOLE_CurrentRef(char* sbuf);
void CONSOLE_Status(void);
void CONSOLE_LogPrint(void);

void CONSOLE_DoCtrl(uint32_t u32Ch, uint32_t u32Ctrl);
void CONSOLE_DiCtrl(void);

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

int32_t GetDataPosition(unsigned char **data);
int32_t GetNextDataPosition(unsigned char **data);
int32_t GetDigit(unsigned char *data, int *val1, int *val2);


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

unsigned char   CONSOLE_DataInput(unsigned char *u32Value, uint16_t u16Size);


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

//void CONSOLE_GpioCtrl(uint32_t u32Ch, uint32_t u32Ctrl);

#endif

/*****************************[ End of Program ]*****************************/

