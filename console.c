/******************************************************************************
 **                  Copyright(C) 2016 Nara Controls Co. Ltd.                **
 **                  --------------------------------------                  **
 ******************************************************************************
	FILE NAME   : Console.C
	AUTHOR      : 
	DATE        : 
	REVISION    : 
	DESCRIPTION : Console Interface Function(CLI)

 ******************************************************************************
	HISTORY     :


 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

//#include "Board.h"
//#include "BasicTestPlatform.h"

#include "build_defs.h"
#include "uart_printf.h"
#include "address.h"
#include "console.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "SoftTimer.h"
#include "PeripheralHeaderIncludes.h"
#include "BLDC_IMD-Settings.h"
#include "IQmathLib.h"
#include "speed_pr.h"
#include "f2806xileg_vdc_BLDC.h"
#include "pid_grando.h"
#include "BLDC_IMD_var.h"
#include "pcInterface.h"

/*---- Flash API include file -------------------------------------------------*/
#include "Flash2806x_API_Library.h"

/*---- example include file ---------------------------------------------------*/
//#include "Example_Flash2806x_API.h"

/*---------------------------------------------------------------------------
  Data/Program Buffer used for testing the flash API functions
---------------------------------------------------------------------------*/
#define  WORDS_IN_FLASH_BUFFER 0x100               // Programming data buffer, Words
Uint16  Buffer[WORDS_IN_FLASH_BUFFER];

/*---------------------------------------------------------------------------
  Sector address info
---------------------------------------------------------------------------*/
typedef struct {
     Uint16 *StartAddr;
     Uint16 *EndAddr;
} SECTOR;

#define FLASH_START_ADDR  0x3D8000
SECTOR Sector[8]= {
         (Uint16 *) 0x3F0000,(Uint16 *) 0x3F3FFF,
         (Uint16 *) 0x3F4000,(Uint16 *) 0x3F7FFF,
         (Uint16 *) 0x3EC000,(Uint16 *) 0x3EFFFF,
         (Uint16 *) 0x3E8000,(Uint16 *) 0x3EBFFF,
         (Uint16 *) 0x3E4000,(Uint16 *) 0x3E7FFF,
		 (Uint16 *) 0x3E0000,(Uint16 *) 0x3E3FFF,
         (Uint16 *) 0x3DC000,(Uint16 *) 0x3DFFFF,
         (Uint16 *) 0x3D8000,(Uint16 *) 0x3DBFFF,
};

Uint32 MyCallbackCounter; // Just increment a counter in the callback function

/*------------------------------------------------------------------
  Callback function - must be executed from outside flash/OTP
-----------------------------------------------------------------*/
#pragma CODE_SECTION(MyCallbackFunction,"ramfuncs");
void MyCallbackFunction(void)
{
    // Toggle pin, service external watchdog etc
    MyCallbackCounter++;
    asm("    NOP");
}

/*---------------------------------------------------------------------------
   These key values are used to unlock the CSM by this example
   They are defined in Example_Flash2806x_CsmKeys.asm
--------------------------------------------------------------------------*/
extern Uint16 PRG_key0;        //   CSM Key values
extern Uint16 PRG_key1;
extern Uint16 PRG_key2;
extern Uint16 PRG_key3;
extern Uint16 PRG_key4;
extern Uint16 PRG_key5;
extern Uint16 PRG_key6;
extern Uint16 PRG_key7;

/*-----------------------------------------------------------------*/
Uint16 Example_CsmUnlock()
{
    volatile Uint16 temp;

    // Load the key registers with the current password
    // These are defined in Example_Flash2806x_CsmKeys.asm

    EALLOW;
    CsmRegs.KEY0 = PRG_key0;
    CsmRegs.KEY1 = PRG_key1;
    CsmRegs.KEY2 = PRG_key2;
    CsmRegs.KEY3 = PRG_key3;
    CsmRegs.KEY4 = PRG_key4;
    CsmRegs.KEY5 = PRG_key5;
    CsmRegs.KEY6 = PRG_key6;
    CsmRegs.KEY7 = PRG_key7;
    EDIS;

    // Perform a dummy read of the password locations
    // if they match the key values, the CSM will unlock

    temp = CsmPwl.PSWD0;
    temp = CsmPwl.PSWD1;
    temp = CsmPwl.PSWD2;
    temp = CsmPwl.PSWD3;
    temp = CsmPwl.PSWD4;
    temp = CsmPwl.PSWD5;
    temp = CsmPwl.PSWD6;
    temp = CsmPwl.PSWD7;

    // If the CSM unlocked, return succes, otherwise return
    // failure.
    if ( (CsmRegs.CSMSCR.all & 0x0001) == 0) return STATUS_SUCCESS;
    else return STATUS_FAIL_CSM_LOCKED;

}

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

GLOBAL_HANDLE_CONSOLE_VAR       ghCON;
CONSOLE_DEBUG_FLAG              ghCDF;
//DATETIME                        ghDateTime; 

uint16_t						gu16ModeFlag = 0x0000;

bool 							gbMonitorADC = 0x0000;

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

static inline bool IsDigit (char ch )
{
    return ((ch >= '0') && (ch <= '9'));
}

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

static inline bool IsHexa (char ch )
{
    return ((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <='F')); //16진수를
}

/*****************************************************************************
 **                                                                         **
 **  콘솔관련 변수 초기화 함수                                              **
 **                                                                         **
 *****************************************************************************/

void CONSOLE_Initialize(void)
{
//    unsigned char i;

	/*------------------------------------------------------------------
	  Initalize Flash_CPUScaleFactor.

	   Flash_CPUScaleFactor is a 32-bit global variable that the flash
	   API functions use to scale software delays. This scale factor
	   must be initalized to SCALE_FACTOR by the user's code prior
	   to calling any of the Flash API functions. This initalization
	   is VITAL to the proper operation of the flash API functions.

	   SCALE_FACTOR is defined in Example_2806xFlashProgramming.h as
	     #define SCALE_FACTOR  1048576.0L*( (200L/CPU_RATE) )

	   This value is calculated during the compile based on the CPU
	   rate, in nanoseconds, at which the algorithums will be run.
	------------------------------------------------------------------*/

	   EALLOW;
	   Flash_CPUScaleFactor = SCALE_FACTOR;
	   EDIS;


	/*------------------------------------------------------------------
	  Initalize Flash_CallbackPtr.

	   Flash_CallbackPtr is a pointer to a function.  The API uses
	   this pointer to invoke a callback function during the API operations.
	   If this function is not going to be used, set the pointer to NULL
	   NULL is defined in <stdio.h>.
	------------------------------------------------------------------*/

	   EALLOW;
	   Flash_CallbackPtr = &MyCallbackFunction;
	   EDIS;

	   MyCallbackCounter = 0; // Increment this counter in the callback function
    

	memset( &ghCON, 0, sizeof(GLOBAL_HANDLE_CONSOLE_VAR) );
	memset( &ghCDF, 0, sizeof(CONSOLE_DEBUG_FLAG) );
	ghCDF.u16DbgFlag[VIEWFLAG_BTP] = VIEWFLAG_BTP_DEFAULT;

	gu16ModeFlag = 0;

	ghCON.u32StartAddr     = tb_MEMORY_MAP[MEMORY_TYPE_SRAM].u32Start;

	ghCON.pu8RxBuffer     = (unsigned char *) ghCON.u8RxBuffer[0];
	ghCON.pu8TokenBuffer  = (unsigned char *) ghCON.u8TokenBuffer[0];

	ghCON.u8LastCommand   = false;

	ghCON.u8DeveloperMode = CONSDEFS_DBG_LEVEL_USER ;

	CONSOLE_RegistUserFunction();
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_SerialProcess(unsigned char u8Char)
{

	if( ghCON.u8DeveloperMode != CONSDEFS_DBG_LEVEL_USER )
	{
		STimer_Start( &ghSTIMER.DebugModeClearTimer, CONSDEFS_DEBUG_MODE_CLEAR_TIME);
	}

	STimer_Start( &ghSTIMER.SerialPortReInitTimer, CONSDEFS_PORT_REINIT_TIME);

	if( u8Char == KEYDEFS_ESC )
	{
		memset( (unsigned char*) &ghCON.u8RxBuffer[0], 0, (uint16_t) sizeof( ghCON.u8RxBuffer ) );
	}

	if( ghCON.u16RxCount < CONSDEFS_MAX_BUFFER )
	{
		if( (u8Char != KEYDEFS_CR) && (u8Char != KEYDEFS_LF) )
		{
			if( u8Char != KEYDEFS_BS )
			{
				if(u8Char == KEYDEFS_TAB) 
				    UART_putch(' ');
				else
				{
					if(ghCON.u8PassChar == 0x1F)
					{
						if(u8Char != ' ') 
						    UART_putch('*');
						else
						    UART_putch(u8Char);
					}
					else 
					    UART_putch(u8Char);
				}
			}
		}

		if( (u8Char >= 'a') && (u8Char <= 'z') ) 
		    u8Char &= 0xDF;

		if( u8Char == KEYDEFS_BS )
		{
			if( ghCON.u16RxCount > 0 )
			{
				ghCON.u8RxBuffer[ --ghCON.u16RxCount ] = 0;
				UART_printf("%c %c",KEYDEFS_BS,KEYDEFS_BS);
			}
			if (ghCON.u8PassCheckCount > 5)
			{
				ghCON.u8PassCheckCount--;
				if( ghCON.u8PassKeyInCount > 0 ) ghCON.u8PassKeyInCount--;
			}
			else if (ghCON.u8PassCheckCount > 0)
			{
				ghCON.u8PassModeBuffer[ghCON.u8PassCheckCount--] = 0;
				ghCON.u8PassChar = 0;
			}
		}
		else if( u8Char == KEYDEFS_AST )
		{
			if( ghCON.u8LastCommand ==  true )
			{
				u8Char = KEYDEFS_CR;
				ghCON.bOldFlag = true;
				ghCON.u16RxCount = ghCON.u16OldCount;
				memcpy( (void *)&ghCON.u8RxBuffer[0], (void *)&ghCON.u8OldBuffer[0], CONSDEFS_MAX_BUFFER );
			}
			else
			{
				CONSOLE_Prompt();
			}
		}
		else
		{
			ghCON.u8RxBuffer[ ghCON.u16RxCount++ ] = u8Char;


			if(ghCON.u8PassCheckCount == 0)
			{
				if(u8Char != ' ') ghCON.u8PassModeBuffer[ghCON.u8PassCheckCount++] = u8Char;
			}
			else if(ghCON.u8PassCheckCount <  4)  ghCON.u8PassModeBuffer[ghCON.u8PassCheckCount++] = u8Char;
			else if(ghCON.u8PassCheckCount == 4)
			{
				ghCON.u8PassModeBuffer[ghCON.u8PassCheckCount++] = u8Char;
				if( strncmp( (char *)ghCON.u8PassModeBuffer, "DEBUG", 5) == false) ghCON.u8PassChar = 0x1F;
			}
			else
			{
				ghCON.u8PassCheckCount++;

				if( ghCON.u8PassChar == 0x1F )
				{
					if( u8Char != ' ' ) ghCON.u8PassKeyBuffer[ ghCON.u8PassKeyInCount++ ] = u8Char;
				}
			}
		}
	}

	if(u8Char == KEYDEFS_CR)
	{
		ghCON.pu8RxBuffer = &ghCON.u8RxBuffer[0];
		ghCON.pu8TokenBuffer  = &ghCON.u8TokenBuffer[0];

		if( ghCON.u16RxCount < CONSDEFS_MAX_BUFFER )
		{
			ghCON.u8RxBuffer[ ghCON.u16RxCount ] = 0;

			CONSOLE_CommandExecute();
		}

		if( ghCON.bOldFlag == false )
		{
			ghCON.u16OldCount = ghCON.u16RxCount;
			memcpy( &ghCON.u8OldBuffer[0], &ghCON.u8RxBuffer[0], CONSDEFS_MAX_BUFFER );
		}

		ghCON.bOldFlag = false;

		memset( (unsigned char*) &ghCON.u8RxBuffer[0],    0, (uint16_t) sizeof( ghCON.u8RxBuffer ) );
		memset( (unsigned char*) &ghCON.u8TokenBuffer[0], 0, (uint16_t) sizeof( ghCON.u8TokenBuffer ) );

		ghCON.u16RxCount          = 0;
		ghCON.u16TokenCount       = 0;
		ghCON.u8PassChar         = 0;
		ghCON.u8PassCheckCount   = 0;
		ghCON.u8PassKeyInCount   = 0;

		memset( &ghCON.u8PassKeyBuffer[0],    0, CONSDEFS_MAX_BUFFER );
		memset(  ghCON.u8PassModeBuffer,      0, sizeof( ghCON.u8PassModeBuffer ) );

   		CONSOLE_Prompt();
	}
}

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_RegistUserFunction(void)
{
        CONSOLE_SetUserFunction("?"         , CONSOLE_HelpMsgDisplay    , "HELP"                            , 1);
        CONSOLE_SetUserFunction("HELP"      , CONSOLE_HelpMsgDisplay    , "Help [0..3]"              	, 1);
        CONSOLE_SetUserFunction("DEBUG"     , CONSOLE_SetDebugMode      , "Debug Mode [passcode]"       		, 1);
        CONSOLE_SetUserFunction("VER"       , CONSOLE_VersionDisp       , "Version Display"                 , 1);
        CONSOLE_SplitUserFunction();
        CONSOLE_SetUserFunction("CF"        , CONSOLE_CtrlFlag          , "Ctrl Flag Set"                   , 0x12);
        CONSOLE_SetUserFunction("SPID"      , CONSOLE_SpeedPid          , "Speed PID"                       , 0x13);
        CONSOLE_SetUserFunction("CPID"      , CONSOLE_CurrentPid        , "Current PID"                     , 0x13);
        CONSOLE_SetUserFunction("PFC"       , CONSOLE_PoleFreqCurrent   , "Pole,Bsc Frequency,Bsc Current"  , 0x13);
        CONSOLE_SetUserFunction("SLOP"      , CONSOLE_Slop              , "ACC & DEC Slop"                  , 0x2);
        CONSOLE_SetUserFunction("SPD"       , CONSOLE_SpeedRef          , "Speed Reference"                 , 0x11);
        CONSOLE_SetUserFunction("CR"        , CONSOLE_CurrentRef        , "Current Reference"               , 0x11);
        CONSOLE_SetUserFunction("ST"        , CONSOLE_Status            , "Display Status"                  , 0);
        CONSOLE_SetUserFunction("LOG"       , CONSOLE_LogPrint          , "Log Print"                       , 0);
		#if (0)
        CONSOLE_SplitUserFunction();
        CONSOLE_SetUserFunction("MAP"       , CONSOLE_MemoryMapDisp     , "Memory map disp"                 , 0);
		CONSOLE_SetUserFunction("MD"        , CONSOLE_MemoryDump        , "Memory Dump"                     , 2);
		CONSOLE_SetUserFunction("MF"        , CONSOLE_MemoryFill        , "Memory Fill"                     , 3);
		CONSOLE_SetUserFunction("ME"        , CONSOLE_MemoryEdit        , "Memory Edit"                     , 1);
        CONSOLE_SplitUserFunction();
		#endif
        CONSOLE_SetUserFunction("DO"        , CONSOLE_DoCtrl            , "DO Test Control"                 , 2);
        CONSOLE_SetUserFunction("DI"        , CONSOLE_DiCtrl            , "DI Test Read"                    , 0);

        CONSOLE_SetUserFunction("--LEVEL1"  , NULL                      , NULL                              , 0);


        CONSOLE_SetUserFunction("--LEVEL2"  , NULL                      , NULL                              , 0);
		//CONSOLE_SetUserFunction("FT"        , CONSOLE_FlashTest         , "Flash Write Test"                , 0);
		CONSOLE_SetUserFunction("SAVE"      , CONSOLE_SaveCfg           , "Configuration Save"              , 0);
		CONSOLE_SetUserFunction("LOAD"      , CONSOLE_LoadCfg           , "Configuration Load"              , 1);
        CONSOLE_SplitUserFunction();
		CONSOLE_SetUserFunction("FACTORY"   , CONSOLE_Factory           , "Factory Setting"                 , 2);
		CONSOLE_SetUserFunction("RESET"     , CONSOLE_Reset             , "Cold Reset"                      , 0);
		//CONSOLE_SetUserFunction("SCI"       , CONSOLE_SCIboot           , "SCI Boot"                        , 0);
        CONSOLE_SetUserFunction("--LEVEL3"  , NULL                      , NULL                              , 0);

		#if (1)
		CONSOLE_SetUserFunction("MD"        , CONSOLE_MemoryDump        , "Memory Dump"                     , 2);
		CONSOLE_SetUserFunction("MF"        , CONSOLE_MemoryFill        , "Memory Fill"                     , 3);
		CONSOLE_SetUserFunction("MC"        , CONSOLE_MemoryCheck       , "*Memory Check"                   , 2);
		CONSOLE_SetUserFunction("ME"        , CONSOLE_MemoryEdit        , "*Memory Edit"                    , 1);
		CONSOLE_SplitUserFunction();
		CONSOLE_SetUserFunction("MAP"       , CONSOLE_MemoryMapDisp     , "Memory map disp"                 , 0);
		#endif

        CONSOLE_SplitUserFunction();


}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_SetUserFunction(char *pu8Command, void (*ft)(), char *pu8Message, uint16_t u16ParameterNo)
{
	CONS_USER_FUNCTION *pFunction = (CONS_USER_FUNCTION *) &ghCON.Command[ ghCON.u16CommandCount ];

	pFunction->pu8Command     = (unsigned char*)pu8Command;
	pFunction->pu8Message     = (unsigned char*)pu8Message;
	pFunction->u16ParameterNo = u16ParameterNo;
	pFunction->ft             = ft;

	ghCON.u16CommandCount     = ((ghCON.u16CommandCount + 1) % CONSDEFS_MAX_USER_FT);
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_SplitUserFunction(void)
{
   CONSOLE_SetUserFunction("--", NULL, NULL, NULL);
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_CommandExecute(void)
{
   uint16_t    i;
   unsigned char     u8TokenType;
   uint16_t    u16ArgmentNo = 0;
   unsigned char    *ppu8Argment[CONSDEFS_MAX_ARGMENT+1];

   for( i=0; i<CONSDEFS_MAX_BUFFER; i++ )
   {
	   u8TokenType = CONSOLE_GetToken( &ppu8Argment[u16ArgmentNo] );

	   switch( u8TokenType )
	   {
	   case CONSDEFS_TOKEN_TYPE_ARG:
		   if( u16ArgmentNo < CONSDEFS_MAX_ARGMENT ) u16ArgmentNo++;
		   break;

	   case CONSDEFS_TOKEN_TYPE_EOL:
		   if( u16ArgmentNo != 0 )
		   {
			   ppu8Argment[u16ArgmentNo] = 0;

			   CONSOLE_CallUserFunction(ppu8Argment, u16ArgmentNo);
		   }
		   return;

	   default:
		   UART_printf("toktype error = %d\n", u8TokenType);
		   return;
	   }
   }
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

unsigned char CONSOLE_GetToken(unsigned char **ppu8RxBuffer)
{
   unsigned char u8TokenType;

   *ppu8RxBuffer = ghCON.pu8TokenBuffer;

   for( ; ((*ghCON.pu8RxBuffer == KEYDEFS_SPACE)||\
		   (*ghCON.pu8RxBuffer == KEYDEFS_TAB)) ; ghCON.pu8RxBuffer++ )
   {
	   ghCON.u16TokenCount ++;
   }

   *ghCON.pu8TokenBuffer++   = *ghCON.pu8RxBuffer;

   ghCON.u16TokenCount ++;


   switch( *ghCON.pu8RxBuffer++ )
   {
   case KEYDEFS_CR:
	   u8TokenType = CONSDEFS_TOKEN_TYPE_EOL;
	   break;

   default:
	   u8TokenType = CONSDEFS_TOKEN_TYPE_ARG;

	   while( CONSOLE_IsArgment(*ghCON.pu8RxBuffer) )
	   {
		   *ghCON.pu8TokenBuffer++ = *ghCON.pu8RxBuffer++;
		   if( ++ghCON.u16TokenCount > ghCON.u16RxCount )
		   {
			   *ghCON.pu8TokenBuffer++ = 0;

			   return( CONSDEFS_TOKEN_TYPE_EOL );
		   }
	   }
	   break;
   }
   *ghCON.pu8TokenBuffer++ = 0;

   return( u8TokenType );
}

/*****************************************************************************
 **                                                                         **
 **  compare given character with special character and return 0 or 1       **
 **                                                                         **
 *****************************************************************************/

bool CONSOLE_IsArgment(char cChar)
{
   if(ghCON.u16TokenCount >= ghCON.u16RxCount) return( false );

   switch( cChar )
   {
   case KEYDEFS_AMPERSAND:
   case KEYDEFS_SEMICOLON:
   case KEYDEFS_SPACE:
   case KEYDEFS_TAB:
   case KEYDEFS_CR:
   //case KEYDEFS_DOT:
		return( false );
   }
   return( true );
}

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_CallUserFunction(unsigned char **ppu8Command, uint16_t u16ArgmentNo)
{
   uint16_t      i, j, u16No;
   unsigned char       u8Level,u8ModeNo = 0;
   uint32_t      u32Parameter[ CONSDEFS_MAX_PARAMETER ];

   unsigned char **ppu8Command_buf;

   ppu8Command_buf = ppu8Command;
   CONS_USER_FUNCTION *pFunction = (CONS_USER_FUNCTION *) &ghCON.Command[0];

   switch( ghCON.u8DeveloperMode )
   {
       case CONSDEFS_DBG_LEVEL_OPERATOR:   u8Level = 1; break;
	   case CONSDEFS_DBG_LEVEL_SUPERVISOR: u8Level = 2; break;
	   case CONSDEFS_DBG_LEVEL_DEVELOPER:  u8Level = 3; break;
	   default:                            u8Level = 0;
   }

	if( strncmp( (char*) *ppu8Command, "--", 2) == false) return;

    memset(  (unsigned char *)u32Parameter,      0, sizeof( u32Parameter ) );

    ghCON.u8LastCommand = false;

    for( i=0; i<ghCON.u16CommandCount; i++, pFunction++ )
    {
	   if( strncmp( (char*) pFunction->pu8Command, "--LEVEL", 7) == false) u8ModeNo++;
	   if(( u8ModeNo > u8Level ) && ( u8Level != 4 )) break;

	   if( strcmp( (char*) *ppu8Command, (char*) pFunction->pu8Command ) == false )
	   {
		   u16No = pFunction->u16ParameterNo&0x0f; 

		   for( j=0, ppu8Command++; (j<u16No); j++, ppu8Command++ )
		   {
			   if( (j+1) < u16ArgmentNo ) u32Parameter[j] = CONSOLE_GetData(*ppu8Command);
			   else                     u32Parameter[j] = 0L;
		   }

		   ghCON.u16ArgmentNo   = u16ArgmentNo;
		   ghCON.u8LastCommand = true;

		   switch( pFunction->u16ParameterNo )
		   {
		   case 0 : (*pFunction->ft)(); break;
		   case 1 : (*pFunction->ft)(u32Parameter[0]); break;
		   case 2 : (*pFunction->ft)(u32Parameter[0], u32Parameter[1]); break;
		   case 3 : (*pFunction->ft)(u32Parameter[0], u32Parameter[1], u32Parameter[2]); break;
		   case 4 : (*pFunction->ft)(u32Parameter[0], u32Parameter[1], u32Parameter[2], u32Parameter[3]); break;
		   case 5 : (*pFunction->ft)(u32Parameter[0], u32Parameter[1], u32Parameter[2], u32Parameter[3], u32Parameter[4]); break;
		   case 6 : (*pFunction->ft)(u32Parameter[0], u32Parameter[1], u32Parameter[2], u32Parameter[3], u32Parameter[4], u32Parameter[5]); break;
		   case 0x11 : (*pFunction->ft)(ppu8Command_buf[1]); break;
		   case 0x12 : (*pFunction->ft)(ppu8Command_buf[1], ppu8Command_buf[2]); break;
		   case 0x13 : (*pFunction->ft)(ppu8Command_buf[1], ppu8Command_buf[2],ppu8Command_buf[3]); break;
		   default: UART_printf(" Over parameter\n"); ghCON.u8LastCommand = false; break;
		   }
		   return;
	   }
   }
   UART_printf("\n Wrong Command.\n");
}

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

int32_t CONSOLE_GetData( unsigned char *pu8Char )
{
	int32_t     s32DispValue    = 0;
	unsigned char      u8HexFlag       = false;
	bool    bSignFlag       = false;

   for( ; *pu8Char != 0; pu8Char++ )
   {
	   if( IsDigit( (unsigned char) *pu8Char ) )
	   {
		   if( u8HexFlag == true )
		   {
			   s32DispValue = (s32DispValue * 16) + (long)(*pu8Char - '0');
		   }
		   else
		   {
			   s32DispValue = (s32DispValue * 10L) + (long)(*pu8Char - '0');
		   }
	   }
	   else if( IsHexa( *pu8Char & 0xDF ) )
	   {
		   if( u8HexFlag == true )
		   {
			   s32DispValue = (s32DispValue * 16) + ((*pu8Char & 0xDF) - 'A' + 10 );
		   }
	   }
	   else if( ((*pu8Char == 'x') || (*pu8Char == 'X')) )
	   {
		   u8HexFlag   = true;
		   s32DispValue = 0;
	   }
	   else if( *pu8Char == '-')
	   {
		bSignFlag = true;
	   }
   }

   if( bSignFlag == true)
   {
		s32DispValue *= (-1);
   }
	return( s32DispValue );
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

unsigned char CONSOLE_KeyInputData(uint32_t *pu32Value, unsigned char u8DataType)
{
   unsigned char u8Char;
   uint16_t  u16DataSize;
   uint16_t  u16RxCount = 0;

   *pu32Value = 0L;

   switch( u8DataType )
   {
	   case CONSDEFS_DATA_TYPE_BYTE: u16DataSize = 2; break;
	   case CONSDEFS_DATA_TYPE_WORD: u16DataSize = 4; break;
	   default:                      u16DataSize = 8; break;
   }

	while( (u8Char = (unsigned char)UART_getch() ) != KEYDEFS_CR )

   {
	   //ClearWatchdog();

	   if((u8Char == KEYDEFS_ESC) || (u8Char == KEYDEFS_CTRL_C)) return( u8Char );

	   if( u16RxCount < u16DataSize )
	   {
		   if( u8Char == KEYDEFS_BS )
		   {
			   if( u16RxCount != 0 )
			   {
				   UART_printf("%c %c", KEYDEFS_BS, KEYDEFS_BS);

				   u16RxCount--;
				   *pu32Value /= 16L;
			   }
		   }
		   else if(IsDigit(u8Char))
		   {
			   UART_printf("%c", u8Char);

			   *pu32Value = (*pu32Value * 16L + (u8Char - '0'));
			   u16RxCount++;
		   }
		   else if(IsHexa(u8Char & 0xDF))
		   {
			   UART_printf("%c", u8Char);

			   u8Char     = (u8Char & 0xDF);
			   *pu32Value = (*pu32Value * 16L + (u8Char - 'A' + 10));
			   u16RxCount++;
		   }
	   }
   }
   if( u16RxCount )
   {
	   for( u8Char = (unsigned char) u16DataSize ; u8Char > (unsigned char) u16RxCount ; u8Char-- ) UART_printf(" ");

	   return( true );
   }

   UART_printf("  ");
   return( false );
}


/*****************************************************************************
 **                                                                         **
 **  Console Debug Command Display(Help Message Display)                    **
 **                                                                         **
 *****************************************************************************/

void CONSOLE_HelpMsgDisplay( uint32_t u32No )
{
   uint16_t i;
   uint16_t u16No;
   uint16_t u16Disp  = 0;
   uint16_t u16Level = 0;

   CONS_USER_FUNCTION *pFunction = (CONS_USER_FUNCTION *) &ghCON.Command[1];

   if( ghCON.u16CommandCount == 0 ) return;

   switch( ghCON.u8DeveloperMode )
   {
       case CONSDEFS_DBG_LEVEL_OPERATOR:   u16Level = 1; break;
	   case CONSDEFS_DBG_LEVEL_SUPERVISOR: u16Level = 2; break;
	   case CONSDEFS_DBG_LEVEL_DEVELOPER:  u16Level = 3; break;
	   default:                            u16Level = 0; break;
   }

   if( ghCON.u16ArgmentNo > 1 )
   {
	   u16No = (uint16_t) u32No;
	   if( u16No <= u16Level ) u16Level = (uint16_t) u16No;
   }

   for( i=0; i< (ghCON.u16CommandCount-1); i++, pFunction++ )
   {
	   if( strncmp( (char*) pFunction->pu8Command, "--LEVEL", 7) == false) {
		   u16Disp++;
		   UART_flush();	// 출력이 다될때 까지 기다림 (출력버퍼가 작음)
	   }
	   if( u16Disp > u16Level )
		   break;

	   if (ghCON.u16ArgmentNo > 1) {
		   if(u16Disp == u16Level)
		   {
			   if( strncmp( (char*) pFunction->pu8Command, "---", 3) == false)
				   UART_printf("\n[----------]");
			   else
			   {
				   UART_printf("\n[%-10s]%d: %s", pFunction->pu8Command, (uint16_t)(pFunction->u16ParameterNo%16), pFunction->pu8Message);
			   }
		   }
	   } else {
		   if(u16Disp <= u16Level)
		   {
			   if( strncmp( (char*) pFunction->pu8Command, "---", 3) == false)
				   UART_printf("\n[----------]");
			   else
			   {
				   UART_printf("\n[%-10s]%d: %s", pFunction->pu8Command, (uint16_t)(pFunction->u16ParameterNo%16), pFunction->pu8Message);
			   }
		   }
	   }
   }
   UART_printf("\n");
}

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_Prompt(void)
{
   UART_printf("\nIMD> ");
}

/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

 void CONSOLE_DispAccessCodeErrMsg(void)
 {
	UART_printf("\n wrong [PassCode].");
 }

/*****************************************************************************
 **                                                                         **
 **  Logo Display                                                           **
 **                                                                         **
 *****************************************************************************/

 void CONSOLE_Logo(void)
{
	#if (0)
	unsigned char i;
	CONSOLE_ClearScreen();

	for(i=0;i<25;i++) {
		   CONSOLE_LineFeed();
	}

	CONSOLE_Gotoxy(1,1);
	CONSOLE_LineFeed();

	CONSOLE_CharPrint( 10, '*', 60, true );
	CONSOLE_CharPrint( 10, '*',  2, true ); UART_printf("%10sCompiled :    %4d/%02d/%02d   %10s %8s**", " ", BUILD_YEAR, BUILD_MONTH, BUILD_DAY, __TIME__, " ");
	CONSOLE_CharPrint( 10, '*', 60, true );
	CONSOLE_CharPrint( 10, '*',  2, true ); UART_printf("%15sCopyright(C) NaraContrl Co., Ltd.%12s**", " ", " ");
	CONSOLE_CharPrint( 10, '*',  2, true );
	CONSOLE_CharPrint( 10, '-', 37, false);
	CONSOLE_CharPrint(  9, '*',  2, false);
	//   CONSOLE_CharPrint( 10, '*',  2, true ); UART_printf("%10sProduct %20s  V:%5.2f%9s**", " ", cdsDspVer.board_name, cdsDspVer.dsp_version/100., " ");
	CONSOLE_CharPrint( 10, '*', 60, true );

	UART_printf("\n");
	#else
	CONSOLE_ClearScreen();
	CONSOLE_Gotoxy(1,1);

	UART_printf("\n*****************************************************************");
	UART_printf("\n**%14sCompiled :    %4d/%02d/%02d   %10s %9s**", " ", BUILD_YEAR, BUILD_MONTH, BUILD_DAY, __TIME__, " ");
	UART_printf("\n**%15sCopyright(C) Nara Control Co., Ltd.%11s**", " ", " ");
	UART_printf("\n**                                                             **");
	UART_printf("\n**   Intelligent Motor Drive Platform  / %10s / V%2d.%02d   **", BOARD_NAME, BTP_VER0, BTP_VER1);
	UART_printf("\n*****************************************************************");
	#endif
}

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_LineFeed(void)
{
   UART_printf("\n");
}

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_SpacePrint( unsigned char u8Space )
{
   for( ; u8Space>0; u8Space-- ) UART_printf(" ");
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_CharPrint( unsigned char u8Space, char cCh, unsigned char u8Number, unsigned char u8LineFeedMode )
{
   if( u8LineFeedMode == true  ) CONSOLE_LineFeed();

   CONSOLE_SpacePrint( u8Space );

   for( ; u8Number>0; u8Number-- ) UART_printf("%c", cCh);
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_Gotoxy(unsigned char u8X, unsigned char u8Y)
{

	unsigned char u8Tx, u8Ty;
	unsigned char u8Xx, u8Yyy;

	u8Tx = u8X;
	u8Ty = u8Y;
	u8Xx = u8Yyy = 0;

	while(u8Ty >= 10){ u8Ty -= 10; u8Yyy++; }
	while(u8Tx >= 10){ u8Tx -= 10; u8Xx++; }

	UART_putch(0x1B);
	UART_putch(0x5B);
	UART_putch('0' + u8Yyy);
	UART_putch('0' + u8Ty);
	UART_putch(';');
	UART_putch('0' + u8Xx);
	UART_putch('0' + u8Tx);
	UART_putch('H');

}


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

void CONSOLE_ClearScreen(void)
{
	UART_putch(0x1B);
	UART_putch(0x5B);
	UART_putch('2');
	UART_putch('J');
	UART_printf("\n");
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_VersionDisp(uint32_t u32Flag)
{
//    unsigned char u8Name[2][13];
//    volatile unsigned char* pu8Buf;
	unsigned char i;

	UART_printf("\n==============================================");
	UART_printf("\n     [ %s ] Version info", BOARD_NAME);
	UART_printf("\n==============================================");
	UART_printf("\n   Board Type : %02d , Motor Type : %02d", gdsSystem.u16BoardType, gdsSystem.u16MotorType );
	UART_printf("\n   IMD        : F/W Ver. %2d.%02d", BTP_VER0, BTP_VER1 );
	UART_printf("\n   Compiled   : %4d/%02d/%02d %s", BUILD_YEAR, BUILD_MONTH, BUILD_DAY, __TIME__);
	UART_printf("\n==============================================");

	// test of variable size
	// UART_printf ("\n UINT = %d, UINT32 = %d", (uint16_t)sizeof(uint16_t), (uint16_t)sizeof(uint32_t));
	// UART_printf ("\n UINT = %x, UINT32 = %lx", (uint16_t)0x1234, (uint32_t)0x12345678 );

	if( ghCON.u16ArgmentNo >= 2 )
	{
        if( u32Flag == PASSCODE )
        {

//        	pu8Buf = ADDR_FPGA_VER_STR0;
            for (i=0; i< 12; ++i ) {
//                u8Name[FPGA_VER_SMPU][i]= *pu8Buf++;
            }
//            u8Name[FPGA_VER_SMPU][12]='\0';

            UART_printf("\n   FPGA");
            UART_printf("\n==============================================");
        }
    }
    else
    {
        UART_printf("\n Ex) Ver [Passcode]");
    }
    UART_printf("\n");

 }


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

 void CONSOLE_SetViewFlag( uint32_t u32Mode, uint32_t u32Value )
 {
	 unsigned char u8Mode = (unsigned char)u32Mode;
	 uint16_t u16Value;

	if( ghCON.u16ArgmentNo > 2 )
	{
		u16Value = (uint16_t)u32Value;

		switch( u8Mode )
		{
		case VIEWFLAG_BTP:     ghCDF.u16DbgFlag[VIEWFLAG_BTP] = (uint16_t) (u16Value & VIEW_FLAG_CS_EVENT_MASK   ); break;
		case VIEWFLAG_ADC:     ghCDF.u16DbgFlag[VIEWFLAG_ADC] = (uint16_t) (u16Value & VIEW_FLAG_ADC_EVENT_MASK  ); break;
		case VIEWFLAG_DIO:     ghCDF.u16DbgFlag[VIEWFLAG_DIO] = (uint16_t) (u16Value & VIEW_FLAG_DIO_EVENT_MASK  ); break;
		default:               ghCON.u16ArgmentNo = 1;                                           break;
		}
	}

	UART_printf("\nConsole Monitor");

	if( ghCON.u16ArgmentNo == 2 )
	{
		UART_printf("\nCONSOLE DEBUG VIEW: %d\n", u8Mode);

		switch( u8Mode )
		{
		case VIEWFLAG_BTP:      UART_printf("\n CSD Status     " );
								UART_printf("\n 01h: CSD SET LV1 EVT " );
								UART_printf("\n 02h: CSD SET LV2 EVT " );
								UART_printf("\n 04h: CSD CTRL EVT " );
								break;

		case VIEWFLAG_ADC:      UART_printf("\n ADC Status"             );
								UART_printf("\n 01h: ADC SET "  );
								UART_printf("\n 02h: ADC EVT "  );
								break;

		case VIEWFLAG_DIO:      UART_printf("\n DIO Status"           );
								UART_printf("\n 01h: DIO SET"   );
								UART_printf("\n 02h: DI EVT"   );
								UART_printf("\n 04h: DO EVT"   );
								UART_printf("\n 08h: HDO EVT"   );
								break;

		}
	}
	else
	{
		UART_printf("\nCONSOLE DEBUG VIEW SETTING\n");
		UART_printf("\n %d:CSD  : %04xh",  VIEWFLAG_BTP, ghCDF.u16DbgFlag[VIEWFLAG_BTP] );
		UART_printf("\n %d:ADC  : %04xh",  VIEWFLAG_ADC, ghCDF.u16DbgFlag[VIEWFLAG_ADC] );
		UART_printf("\n %d:DIO  : %04xh",  VIEWFLAG_DIO, ghCDF.u16DbgFlag[VIEWFLAG_DIO]  );
	}
	CONSOLE_LineFeed();
 }


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_DisplayMemoryMap(void)
{
   UART_printf("\n\n FLASH : %08lxh-%08lxh, R"    , tb_MEMORY_MAP[MEMORY_TYPE_FLASH].u32Start,
												  tb_MEMORY_MAP[MEMORY_TYPE_FLASH].u32End  );
   UART_printf(  "\n SRAM  : %08lxh-%08lxh, R/W"  , tb_MEMORY_MAP[MEMORY_TYPE_SRAM ].u32Start,
												  tb_MEMORY_MAP[MEMORY_TYPE_SRAM ].u32End  );
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

unsigned char CONSOLE_IsReadMemory(uint32_t u32Address)
{
   #if(0)
   if( ( (u32Address >= tb_MEMORY_MAP[MEMORY_TYPE_FLASH    ].u32Start) && (u32Address <= tb_MEMORY_MAP[MEMORY_TYPE_FLASH    ].u32End) ) ||
	   ( (u32Address >= tb_MEMORY_MAP[MEMORY_TYPE_SRAM     ].u32Start) && (u32Address <= tb_MEMORY_MAP[MEMORY_TYPE_SRAM     ].u32End) ) ||
	   )

   {
	   return( true );
   }

   CONSOLE_DisplayMemoryMap();

   return( false );
   #endif

   return (true);
}

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

unsigned char CONSOLE_IsWriteMemory(uint32_t u32Address)
{
	if( ( (u32Address >= tb_MEMORY_MAP[MEMORY_TYPE_FLASH   ].u32Start) && (u32Address <= tb_MEMORY_MAP[MEMORY_TYPE_FLASH    ].u32End) ) ||
		( (u32Address >= tb_MEMORY_MAP[MEMORY_TYPE_SRAM    ].u32Start) && (u32Address <= tb_MEMORY_MAP[MEMORY_TYPE_SRAM     ].u32End) )
		)
   {
	   return( true );
   }

   CONSOLE_DisplayMemoryMap();

   return( false );
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

unsigned char CONSOLE_IsFlashMemory(uint32_t u32Address)
{
	   return( false );
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_MemoryDump(uint32_t u32StartAddress, uint32_t u32Length)
{
   CONSOLE_MemDump(u32StartAddress, u32Length);
}

void CONSOLE_MemoryFill(uint32_t u32StartAddress, uint32_t u32Length, uint32_t u32Value)
{
	unsigned char    u8Value   = (unsigned char) u32Value;

   CONSOLE_MemFill(u32StartAddress, u32Length, u8Value);
}

void CONSOLE_MemoryCheck(uint32_t u32StartAddress, uint32_t u32Length)
{
   //CONSOLE_TimerDisable();
   CONSOLE_MemCheck(u32StartAddress, u32Length);
   //CONSOLE_TimerEnable();
}

void CONSOLE_MemoryEdit(uint32_t u32StartAddress)
{
   //CONSOLE_TimerDisable();
   CONSOLE_MemEdit(u32StartAddress);
   //CONSOLE_TimerEnable();
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_MemDump( uint32_t u32StartAddress, uint32_t u32Length)
{
   uint32_t      i;
   uint32_t      j;
   uint32_t      u32Address;
   uint32_t      u32EndAddress;
   uint32_t      u32Size;
   unsigned char       u8Char;
   unsigned char       u8Buffer[17];

   if(ghCON.u16ArgmentNo <= 1)
   {
       UART_printf("\n MD [StartAddr][Length]\n");

	   u32Address  = ghCON.u32StartAddr;
	   u32Size     = CONSDEFS_BASE_MSIZE;
   }
   else if(ghCON.u16ArgmentNo == 2)
   {
	   u32Address  = u32StartAddress;
	   u32Size     = CONSDEFS_BASE_MSIZE;
   }
   else
   {
	   u32Address  = u32StartAddress;
	   if(u32Address < u32Length) { u32Size = u32Length - u32Address; }
	   else                       { u32Size = u32Length;              }
   }

   UART_printf("\n Memory Display Addr: %08lxh - %08lxh \n", u32Address, u32Address+u32Size);

   u32EndAddress = u32Address + u32Size;

   for( i=0L; u32Address<u32EndAddress; i++ )
   {
	   //ClearWatchdog();

	   UART_printf("%08lx :", u32Address);

	   for( j=0L; j<16L; j++, u32Address++ )
	   {

			if( CONSOLE_IsReadMemory(u32Address) == false)
			{
				UART_printf("MD [StartAddr] [u32Length] => ex) MD 0x80000000 0xFF\n");
				return;
			}

		   u8Char = *((unsigned char *) u32Address);

		   UART_printf(" %02x",(uint16_t)(u8Char&0xff));

		   if((u8Char > ' ') && (u8Char <= 'z')) { u8Buffer[j] = u8Char;  }
		   else                                  { u8Buffer[j] = '.';    }
	   }

	   u8Buffer[16] = 0;


	   UART_printf(" : %s \n", u8Buffer);

	   /*
	   u8Char = (unsigned char) UART_getch();

		if((u8Char == KEYDEFS_ESC) || (u8Char == KEYDEFS_CTRL_C))
	   {
		   UART_printf("\n USER => CANCEL\n");
		   ghCON.u32StartAddr = u32Address;
		   return;
	   }
	   */
   }

   ghCON.u32StartAddr = u32EndAddress;
}

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_MemFill(uint32_t u32StartAddress, uint32_t u32EndAddress, unsigned char u8Value)
{
   uint32_t  i;
   uint32_t  u32Address;
   uint32_t  u32Size;
   uint32_t  u32Value;
   unsigned char   u8Char;

   //if( ghCON.u16ArgmentNo <= 1 ) u32Address = ghCON.u32StartAddr;
   //else                          u32Address = u32StartAddress;

   //if( ghCON.u16ArgmentNo >= 3 ) u32Size    = u32EndAddress;
   if( ghCON.u16ArgmentNo < 4 )
   {
       //u32Value   = (unsigned char)u8Value;
       UART_printf("\n MF [StartAddr][Length][Data]\n");
       return;
   }

    u32Address = u32StartAddress;
    u32Size    = u32EndAddress;
    u32Value   = (unsigned char)u8Value;
    
   if( (CONSOLE_IsWriteMemory(u32Address) == false) ||
	   (CONSOLE_IsFlashMemory(u32Address) == true ) )
   {
	   UART_printf("Usage Ex) MF [StartAddr] [Size] [Val]\n");
	   return;
   }

   if(ghCON.u16ArgmentNo <= 2)
   {
	   UART_printf("\n End Address or Length = ");

	   u8Char = CONSOLE_KeyInputData(&u32Size, CONSDEFS_DATA_TYPE_LONG);

	   if((u8Char == KEYDEFS_ESC) || (u8Char == KEYDEFS_CTRL_C))
	   {
		   UART_printf(" => CANCEL\n");
		   return;
	   }
	   else if(u8Char == false)
	   {
		   u32Size = 1L;
	   }
   }

   if(ghCON.u16ArgmentNo <= 3)
   {
	   UART_printf("\n Fill Value = ");

	   u8Char = CONSOLE_KeyInputData(&u32Value, CONSDEFS_DATA_TYPE_BYTE);

	   if((u8Char == KEYDEFS_ESC) || (u8Char == KEYDEFS_CTRL_C))
	   {
		   UART_printf(" => CANCEL\n");
		   return;
	   }
	   else if(u8Char == false)
	   {
		   UART_printf(" => NONE");
		   return;
	   }
   }

	if(u32Address < u32Size) u32Size = u32Size - u32Address;
	else                     u32Size = u32Size;

   UART_printf("\n Memory Fill [ %08lxh - %08lxh : %02xh ]\n",u32Address,u32Address+u32Size-1,(uint16_t)u32Value);

   for(i=0;i<u32Size;i++)
   {
	   //ClearWatchdog();

	   if(CONSOLE_IsWriteMemory(u32Address+i) == false)
	   {
		   UART_printf("Usage Ex) MF [StartAddr] [EndAddr] [Val]\n");
		   ghCON.u32StartAddr = (u32Address + i);
		   return;
	   }


	   *( (unsigned char *)(u32Address+i) ) = (unsigned char) u32Value;
	   *( (unsigned char *)(u32Address+i) ) = (unsigned char) u32Value;

   }

   ghCON.u32StartAddr = u32Address + u32Size;
   UART_printf(" Success\n");
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

unsigned char CONSOLE_MemCheck(uint32_t u32StartAddress, uint32_t u32Length)
{
   uint32_t      i;
   uint32_t      u32Address;
   uint32_t      u32Size;
   uint32_t      u32Lenth;
   unsigned char       u8Value;
   unsigned char       u8Char;
   unsigned char       u8InChar;
   bool     bCompResult = true;

   if( ghCON.u16ArgmentNo <= 1 )
   {
       UART_printf("\n MC [StartAddr][Length]\n");
       return( false );
	   //u32Address = (uint32_t) ghCON.u32StartAddr;
	   //u32Size    = (uint32_t) CONSDEFS_BASE_MSIZE;
   }
   else if(ghCON.u16ArgmentNo == 2)
   {
	   u32Address = (uint32_t) u32StartAddress;
	   u32Size    = (uint32_t) CONSDEFS_BASE_MSIZE;
   }
   else
   {
	   u32Address = (uint32_t) u32StartAddress;
	   u32Lenth  = (uint32_t) u32Length;

	   if(u32Address < u32Lenth) { u32Size = u32Lenth - u32Address; }
	   else                      { u32Size = u32Lenth;             }
   }

   if( (CONSOLE_IsWriteMemory(u32Address) == false) ||
	   (CONSOLE_IsFlashMemory(u32Address) == true ) )
   {
	   UART_printf("Usage Ex) MC [StartAddr] [EndAddr]\n");
	   return( false );
   }

   UART_printf("\n Memory Check [ %08lxh - %08lxh ]\n", u32Address, (u32Address+u32Size));

   for( i=0L; i<u32Size; i++ )
   {
	   //ClearWatchdog();

	   if(CONSOLE_IsWriteMemory(u32Address) == false)
	   {
		   UART_printf("Usage Ex) MC [StartAddr] [EndAddr]\n");
		   return( false );
	   }

	   u8Value = *( (unsigned char *) (u32Address+i) );
	   *( (unsigned char *) (u32Address+i) ) = (unsigned char) 0x5A;
	   u8Char  = *( (unsigned char *) (u32Address+i) );

	   if(u8Char != 0x5A) bCompResult = false;
	   else
	   {
		   *( (unsigned char *) (u32Address+i) ) = (unsigned char) 0xA5;
		   u8Char = *( (unsigned char *) (u32Address+i) );

		   if(u8Char != 0xA5) { bCompResult = false; }
	   }

	   *( (unsigned char *) (u32Address+i) ) = (unsigned char) u8Value;

	   if(bCompResult == false)
	   {
		   UART_printf("\n Memory Check Error [ %08lxh : %02xh ]\n", (u32Address+i), u8Value);
		   ghCON.u32StartAddr = (u32Address+i);
		   return( false );
	   }

		u8InChar = (unsigned char) UART_getch();

	   if((u8InChar == KEYDEFS_ESC) || (u8InChar == KEYDEFS_CTRL_C))
	   {
		   UART_printf("\n USER => CANCEL\n");
		   ghCON.u32StartAddr = u32Address;
		   return( false );
	   }
   }

   ghCON.u32StartAddr = u32Address + u32Size;

   UART_printf("\n Memory Check success\n");

   return( true );
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void CONSOLE_MemEdit( int32_t u32StartAddress )
{
   uint32_t      i;
   int32_t      u32Address;
   uint32_t      u32Value;
   unsigned char       u8Char;
   unsigned char       u8InChar;

	if(ghCON.u16ArgmentNo <= 1) 
	{ 
	    //u32Address = ghCON.u32StartAddr; 
       UART_printf("\n ME [StartAddr]\n");
       return;
	}
	else                      
	{ 
	    u32Address = u32StartAddress;        
	}

   for( i=0L; i<16L; i++, u32Address++ )
   {
	   if( (CONSOLE_IsWriteMemory(u32Address) == false) )
	   {
		   UART_printf("Usage Ex) ME [EditAddr] => ex) ME 0x80000000\n");
		   return;
	   }

	   u8Char  = *(unsigned char *) u32Address;
	   UART_printf("\n %08lxh : %02xh -> ", u32Address, (uint16_t)u8Char);
	   u8InChar = CONSOLE_KeyInputData( &u32Value, CONSDEFS_DATA_TYPE_BYTE );

	   if(u8InChar  == true)
	   {
		   *(unsigned char *) u32Address = (unsigned char) u32Value;

		   u8Char = *(unsigned char *) u32Address;
		   UART_printf(" => %02xh  Change", (uint16_t)u8Char);
	   }
	   else if((u8InChar == KEYDEFS_ESC) || (u8InChar == KEYDEFS_CTRL_C))
	   {
		   UART_printf("\n USER =>  CANCEL\n");
		   ghCON.u32StartAddr = u32Address;
		   return;
	   }
	   else
	   {
		   UART_printf(" => Not Change");
	   }
   }

   ghCON.u32StartAddr = u32StartAddress;
}

/*****************************************************************************
 **                                                                         **
 **  단말장치 예비포트 디버그 모드 선택 함수                                **
 **                                                                         **
 *****************************************************************************/

 void CONSOLE_SetDebugMode(void)
 {
	if( ghCON.u16ArgmentNo >= 1 )
	{
		if     ( strncmp( (char *)ghCON.u8PassKeyBuffer, PASSWORD_OPERATOR  , strlen(PASSWORD_OPERATOR))   == false) ghCON.u8DeveloperMode = CONSDEFS_DBG_LEVEL_OPERATOR;
		else if( strncmp( (char *)ghCON.u8PassKeyBuffer, PASSWORD_SUPERVISOR, strlen(PASSWORD_SUPERVISOR)) == false) ghCON.u8DeveloperMode = CONSDEFS_DBG_LEVEL_SUPERVISOR;
		else if( strncmp( (char *)ghCON.u8PassKeyBuffer, PASSWORD_DEVELOPER , strlen(PASSWORD_DEVELOPER )) == false) ghCON.u8DeveloperMode = CONSDEFS_DBG_LEVEL_DEVELOPER;
		else                                                                                                         ghCON.u8DeveloperMode = CONSDEFS_DBG_LEVEL_USER;
	}
	else
	{
		ghCON.u8DeveloperMode = CONSDEFS_DBG_LEVEL_USER;
	}


	switch( ghCON.u8DeveloperMode )
	{
		case CONSDEFS_DBG_LEVEL_OPERATOR:     UART_printf("\n OPERATOR");    break;
		case CONSDEFS_DBG_LEVEL_SUPERVISOR:   UART_printf("\n SUPERVISOR");  break;
		case CONSDEFS_DBG_LEVEL_DEVELOPER:    UART_printf("\n DEVELOPER" );  break;
		default:                              UART_printf("\n USER"       ); break;
	}

	UART_printf(" Mode changed.\n");
 }


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

#if (0)
void CONSOLE_MemoryTest( uint32_t u32Idx, uint32_t u32Passcode )
{
    UART_printf ("\nNot implemented yet!");

    #if(0)
    uint16_t u16Val = (uint16_t) u32Idx;

	if( ghCON.u16ArgmentNo >= 3 )
	{
	    if( u32Passcode == PASSCODE )
	    {
	        CONSOLE_TimerDisable();
	        
    	    if( u16Val == 1 )
    	    {
    	        UART_printf("\n SDRAM Check");
                CONSOLE_MemCheck( (uint32_t)0x20000100, (uint32_t)0xFFFEFF);

    	    }
    	    else if( u16Val == 2 )
    	    {
    	        UART_printf("\n FRAM Check");
                CONSOLE_MemCheck( (uint32_t)0x30000000, (uint32_t)0x1FFFFF);
    	    }
    	    else if( u16Val == 3 )
    	    {
    	        UART_printf("\n DPRAM Check");
                CONSOLE_MemCheck( (uint32_t)0x60000000, (uint32_t)0x7FF0);
    	    }
    	    else if( u16Val == 4 )
    	    {
    	        SeepromCheck();
    	    }
    	    else if( u16Val == 5 )
    	    {
    	        ghCON.u16ArgmentNo = 3;
    	        CONSOLE_FlashWrite(0, 0xFF);
    	    }
    	    else if( u16Val == 6 )
    	    {
    	        ghCON.u16ArgmentNo = 3;
    	        CONSOLE_FlashWrite(1, 0xFF);
    	    }
    	    else
    	    {
    	        UART_printf("\n Memory Idx Error! (1~6)");
    	    }
    	    CONSOLE_TimerEnable();
	    }
	    else
	    {
	        UART_printf("\n PassCode Error!");
	    }
	}
	else
	{
	    UART_printf("\n MEMTEST [Idx 1~6][PassCode]");
	    UART_printf("\n 1 : SDRAM");
	    UART_printf("\n 2 : FRAM");
	    UART_printf("\n 3 : DPRAM");
	    UART_printf("\n 4 : SEEPROM");
	    UART_printf("\n 5 : FLASH(PROGRAM)");
	    UART_printf("\n 6 : FLASH(FAULTWAVE)");
	}
	#endif
}

#endif


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

int32_t GetDataPosition(unsigned char **data)
{
	while (**data != '\0') {
		if (**data != ' ')
			return (0);

		*data = *data + 1;
	}

	return (-1);
}


int32_t GetNextDataPosition(unsigned char **data)
{
	*data = *data + 1;

	while (**data != '\0') {
		if (**data != ' ')
			return (0);

		*data = *data + 1;
	}

	return (-1);
}


int32_t GetDigit(unsigned char *data, int *val1, int *val2)
{
	int num;
	int f_minus;

	num = 0;
	*val1 = 0;
	*val2 = 0;


	if (*data == '-') {
		data++;
		f_minus = -1;
	}
	else {
		f_minus = 1;
	}

	if (!IsDigit(*data))
		return(-1);

	while ((*data >= '0') && (*data <= '9')) {
		if (*data == '\0')
			break;

		num = num * 10 + (*data - '0');
		data++;
	}

	*val1 = num * f_minus;


	if(GetDataPosition(&data))
		return(-1);


	if (*data == '-') {
		data++;
		f_minus = -1;
	}
	else {
		f_minus = 1;
	}

	if (!IsDigit(*data))
		return(-1);


	num = 0;

	while ((*data >= '0') && (*data <= '9')) {
		if (*data == '\0')
			break;

		num = num * 10 + (*data - '0');
		data++;
	}

	*val2 = num * f_minus;
	return(0);

}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

unsigned char CONSOLE_DataInput(unsigned char *u32Value, uint16_t u16Size)
{
   unsigned char   u8Char;
   uint16_t  u16DataSize;
   uint16_t  u16RxCount = 0;
   uint16_t  i;


	u16DataSize=u16Size;

	while( (u8Char = (unsigned char)UART_getch() ) != KEYDEFS_CR )
	{

		if((u8Char == KEYDEFS_ESC) || (u8Char == KEYDEFS_CTRL_C)) return( u8Char );

		if( u16RxCount < u16DataSize )
		{
			if( u8Char == KEYDEFS_BS )
			{
				if( u16RxCount != 0 )
				{
					UART_printf("%c %c", KEYDEFS_BS, KEYDEFS_BS);

					u16RxCount--;
				}
			}

			else if(IsDigit(u8Char) )
			{
				UART_printf("%c", u8Char);

				u32Value[u16RxCount] = u8Char;
				u16RxCount++;
			}
			else if( ( 'A' <= u8Char ) && ( u8Char <= 'z' ) )
			{
				UART_printf("%c", u8Char);

				u32Value[u16RxCount] = u8Char;
				u16RxCount++;
			}
			else if( ( '.' == u8Char ) || ( '-' == u8Char ) || ( '_' == u8Char ) ||
					 ( '<' == u8Char ) || ( '>' == u8Char ) || ( '[' == u8Char ) ||
					 ( ']' == u8Char ) || ( '{' == u8Char ) || ( '}' == u8Char ) ||
					 ( '?' == u8Char )
					)
			{
				UART_printf("%c", u8Char);

				u32Value[u16RxCount] = u8Char;
				u16RxCount++;
			}
		}
	}
	if( u16RxCount )
	{
		for(i=u16DataSize; i>u16RxCount; u16RxCount++)
		{
			u32Value[u16RxCount] = 0;
		}

		return( true );
	}

   return( false );
}


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/
extern volatile uint16_t EnableFlag;
extern uint16_t SensorlessFlag;
extern uint16_t Direction;
extern uint16_t ILoopFlag;
extern SPEED_MEAS_CAP speed1;

void CONSOLE_CtrlFlag (char* sbuf0, char* sbuf1)
{
	if( ghCON.u16ArgmentNo >= 3 ) {

		switch(sbuf0[0])
		{
		    case 'C':
				CtrlSrcFlag = atoi(sbuf1);
				break;
			case 'E':
				gdsGui.u16Start = atoi(sbuf1);
				break;
			case 'S':
				SensorlessFlag = atoi(sbuf1);
				break;
			case 'D' :
				gdsGui.u16Dir = atoi(sbuf1);
				break;
			case 'I' :
				ILoopFlag = atoi(sbuf1);
				break;
			case 'R' :
				RunBreakFlag = atoi(sbuf1);
				break;
		}
		Update_Ctrl_parameter ();

	}
	UART_printf ("\nControl Src  = %d", CtrlSrcFlag);
	UART_printf ("\nEnableFlag   = %d", EnableFlag);
	UART_printf ("\nSensorless   = %d", SensorlessFlag);
	UART_printf ("\nDirection    = %d", Direction);
	UART_printf ("\nILoopFlag    = %d", ILoopFlag);
	UART_printf ("\nRunBreakFlag = %d", RunBreakFlag);

}

/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

void CONSOLE_SpeedPid(char* sbuf0, char* sbuf1, char* sbuf2)
{
	if( ghCON.u16ArgmentNo >= 4 ) {
	    pid1_spd.param.Kp = atof(sbuf0);
		pid1_spd.param.Ki = T/atof(sbuf1);
		pid1_spd.param.Kd = atof(sbuf2)/T;
	}
	UART_printf ("\nP, I, D = %f, %f, %f", pid1_spd.param.Kp, T/pid1_spd.param.Ki, pid1_spd.param.Kd*T);
}

/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

void CONSOLE_CurrentPid(char* sbuf0, char* sbuf1, char* sbuf2)
{
	if( ghCON.u16ArgmentNo >= 4 ) {
		pid1_idc.param.Kp = atof(sbuf0);
		pid1_idc.param.Ki = T/atof(sbuf1);
		pid1_idc.param.Kd = atof(sbuf2)/T;
	}
	UART_printf ("\nP, I, D = %f, %f, %f", pid1_idc.param.Kp, T/pid1_idc.param.Ki, pid1_idc.param.Kd*T);
}

/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/
void CONSOLE_PoleFreqCurrent(char* sbuf0, char* sbuf1, char* sbuf2)
{
	if( ghCON.u16ArgmentNo >= 4 ) {
		gu16Pole       = atoi(sbuf0);
		gfBaseFreq     = atof(sbuf1);
		giqBaseCurrent = atof(sbuf2);
		Update_Cfg_parameter();
	}
	UART_printf ("\nPole          = %d", gu16Pole);
	UART_printf ("\nBaseFrequency = %f", gfBaseFreq);
	UART_printf ("\nBaseCurrent   = %f", giqBaseCurrent);
}

/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/
void CONSOLE_Slop(uint32_t u32AccTime, uint32_t u32DecTime)
{
	if( ghCON.u16ArgmentNo >= 3 ) {
		gu16AccTime  = u32AccTime;
		gu16DecTime  = u32DecTime;
	}
	UART_printf ("\nAccTime   = %d", gu16AccTime);
	UART_printf ("\nDecTime   = %d", gu16DecTime);

}


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

void CONSOLE_SpeedRef(char* sbuf)
{
	_iq SpdRpm;
	if( ghCON.u16ArgmentNo >= 2 ) {
		//sscanf (sbuf, "%f", &SpeedRef);
		SpeedRef = atof (sbuf);
	}
	gdsGui.fSpdRef = SpeedRef;
	UART_printf ("\nSpeed Ref      = %f", SpeedRef);
	UART_printf ("\nSpeed Per Unit = %f", speed1.Speed);
	SpdRpm = speed1.Speed*speed1.BaseRpm;
	UART_printf ("\nSpeed rpm      = %f", SpdRpm);

}

/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/
//extern SPEED_MEAS_CAP speed1;
void CONSOLE_CurrentRef(char* sbuf)
{
	//_iq Current;
	if( ghCON.u16ArgmentNo >= 2 ) {
		//sscanf (sbuf, "%f", &CurrentSet);
		CurrentSet = atof (sbuf);
	}
	gdsGui.fCrnRef = CurrentSet;
	UART_printf ("\nCurrent Set   = %f", CurrentSet);
	UART_printf ("\nDC Current PU = %f", DCbus_current);
	UART_printf ("\nA  Current PU = %f", current[0]);
	UART_printf ("\nB  Current PU = %f", current[1]);
	UART_printf ("\nC  Current PU = %f", current[2]);
	//SpdRpm = speed1.Speed*speed1.BaseRpm;
	//UART_printf ("\nCurrent          = %fA", Current);

}

/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/
//extern SPEED_MEAS_CAP speed1;
void CONSOLE_Status(void)
{
	UART_printf ("\nControl Source = %d", CtrlSrcFlag);
	UART_printf ("\nSpeed Ref      = %f", SpeedRef);
	UART_printf ("\nSpeed Per Unit = %f", speed1.Speed);
	UART_printf ("\nSpeed rmp      = %f", speed1.SpeedRpm);
	UART_printf ("\nCurrent Set   = %f", CurrentSet);
	UART_printf ("\nDC Current PU = %f", DCbus_current);
	UART_printf ("\nA  Current PU = %f", current[0]);
	UART_printf ("\nB  Current PU = %f", current[1]);
	UART_printf ("\nC  Current PU = %f", current[2]);

}

/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

extern uint16_t DLOG_4CH_buff1, DLOG_4CH_buff2, DLOG_4CH_buff3, DLOG_4CH_buff4;
void CONSOLE_LogPrint()
{
	uint16_t i;

	for (i=0; i<0xc8; ++i) {
		UART_printf ("\n%d %d %d %d", DLOG_4CH_buff1, DLOG_4CH_buff2, DLOG_4CH_buff3, DLOG_4CH_buff4);
	}

}

#if(0)
void CONSOLE_DetectHole()
{
	uint16_t i, j, hole[12];

	for (i=0; i<12; ++i) {
		pwm1.CmtnPointer = i;
        BLDCPWM_MACRO(pwm1);
		for (j=0; j < 65535; ++j) {
			HALL3_READ_MACRO(hall1)
			hole[i] = hall1.HallGpioAccepted;
		}
	}

	for (i=0; i<12; ++i) {
		UART_printf ("\n%d = %d", i, hole[i])
	}


}
#endif

/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/
//extern PIN_Handle ledPinHandle;

void CONSOLE_DoCtrl(uint32_t u32Ch, uint32_t u32Ctrl)
{
	if (u32Ch&0x01 ) {
		//PIN_setOutputValue(ledPinHandle, Board_LED0, u32Ctrl);
	}

	if (u32Ch&0x02) {
		//PIN_setOutputValue(ledPinHandle, Board_LED1, u32Ctrl);
	}

}

/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

void CONSOLE_DiCtrl()
{
//	UART_printf ( "\n KEY = %d, %d", PIN_getInputValue(Board_BUTTON0), PIN_getInputValue(Board_BUTTON1));

}

/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

void CONSOLE_MemoryMapDisp(void)
{
    UART_printf("\n--------------------- Memory Device Map -----------------------------------");
    CONSOLE_DisplayMemoryMap();
    UART_printf("\n---------------------------------------------------------------------------");
    UART_printf("\n-------------------------- Memory Map -------------------------------------");
//    UART_printf("\n SDRAM  Line Set              ADDR[0x%08X] Size[%d]"  ,  &gdsLine                        ,  sizeof(gdsLine)  );
//    UART_printf("\n SDRAM  Fault Recode Set      ADDR[0x%08X] Size[%d]"  ,  &gdsSfrSV                       ,  sizeof(gdsSfrSV) );
    UART_printf("\n---------------------------------------------------------------------------\n");

}

#pragma CODE_SECTION(Example_Error,"ramfuncs");
void Example_Error(Uint16 Status)
{

//  Error code will be in the AL register.
    asm("    ESTOP0");
    asm("    SB 0, UNC");
}


/*--- Global variables used to interface to the flash routines */
FLASH_ST FlashStatus;
#if(0)
#pragma CODE_SECTION(CONSOLE_FlashTest,"ramfuncs");
void CONSOLE_FlashTest(void)
{
   Uint16  i;
   Uint16  Status;
   Uint16  *Flash_ptr;     // Pointer to a location in flash
   Uint32  Length;         // Number of 16-bit values to be programmed
   Uint16  VersionHex;     // Version of the API in decimal encoded hex

   // Flash 작업 도중 Flash 영역에서 명령이 실행되면 안됨 → 따라서 인터럽트도 허용되지 않음.
	DRTM;	// Disable Global realtime interrupt DBGM
	DINT;	// Disable Global interrupt INTM

   /*------------------------------------------------------------------
	 Check the version of the API

	 FlashAPIVersionHex() returns the version as a decimal encoded hex.

   ------------------------------------------------------------------*/
	VersionHex = Flash_APIVersionHex();
	if(VersionHex != 0x0100)
	{
	  // Unexpected API version
	  // Make a decision based on this info.
	  asm("    ESTOP0");
	}

	/*------------------------------------------------------------------
	Before programming make sure the sectors are Erased.

	------------------------------------------------------------------*/

	 // Example: Erase Sector B - Sector H
	 // Sectors A has example code so leave them unerased

   // SECTORA-SECTORH are defined in Flash2806x_API_Library.h
   Status = Flash_Erase(SECTORG,&FlashStatus);
   if(Status != STATUS_SUCCESS)
   {
	   Example_Error(Status);
   }

   /*------------------------------------------------------------------
	 Program Flash Examples
   ------------------------------------------------------------------*/

   // A buffer can be supplied to the program function.  Each word is
   // programmed until the whole buffer is programmed or a problem is
   // found.  If the buffer goes outside of the range of OTP or Flash
   // then nothing is done and an error is returned.


   // Example: Program 0x400 values in Flash SectorG

   // In this case just fill a buffer with data to program into the flash.
   for(i=0;i<WORDS_IN_FLASH_BUFFER;i++)
   {
	   Buffer[i] = 0x100+i;
   }

   Flash_ptr = Sector[6].StartAddr;			// FlashB
   Length = 0x400;
   Status = Flash_Program(Flash_ptr,Buffer,Length,&FlashStatus);
   if(Status != STATUS_SUCCESS)
   {
	   Example_Error(Status);
   }



   // Verify the values programmed.  The Program step itself does a verify
   // as it goes.  This verify is a 2nd verification that can be done.
   Status = Flash_Verify(Flash_ptr,Buffer,Length,&FlashStatus);
   if(Status != STATUS_SUCCESS)
   {
	   Example_Error(Status);
   }

	EINT;   // Enable Global interrupt INTM
	ERTM;	// Enable Global realtime interrupt DBGM

}
#endif

/*--- Global variables used to interface to the flash routines */
#pragma CODE_SECTION(CONSOLE_SaveCfg,"ramfuncs");
void CONSOLE_SaveCfg(void)
{
   Uint16  Status;
   Uint16  *Flash_ptr;     // Pointer to a location in flash
   Uint32  Length;         // Number of 16-bit values to be programmed
   Uint16  VersionHex;     // Version of the API in decimal encoded hex
   Uint16  data;

   // Flash 작업 도중 Flash 영역에서 명령이 실행되면 안됨 → 따라서 인터럽트도 허용되지 않음.
	DRTM;	// Disable Global realtime interrupt DBGM
	DINT;	// Disable Global interrupt INTM

   /*------------------------------------------------------------------
	 Check the version of the API

	 FlashAPIVersionHex() returns the version as a decimal encoded hex.

   ------------------------------------------------------------------*/
	VersionHex = Flash_APIVersionHex();
	if(VersionHex != 0x0100)
	{
	  // Unexpected API version
	  // Make a decision based on this info.
	  asm("    ESTOP0");
	}

	/*------------------------------------------------------------------
	Before programming make sure the sectors are Erased.

	------------------------------------------------------------------*/

	 // Example: Erase Sector B - Sector H
	 // Sectors A has example code so leave them unerased

   // SECTORA-SECTORH are defined in Flash2806x_API_Library.h
   Status = Flash_Erase(SECTORH,&FlashStatus);
   if(Status != STATUS_SUCCESS)
   {
	   Example_Error(Status);
   }

   /*------------------------------------------------------------------
	 Program Flash Examples
   ------------------------------------------------------------------*/

   // A buffer can be supplied to the program function.  Each word is
   // programmed until the whole buffer is programmed or a problem is
   // found.  If the buffer goes outside of the range of OTP or Flash
   // then nothing is done and an error is returned.


   // Example: Program 0x400 values in Flash SectorG

   // In this case just fill a buffer with data to program into the flash.

   Flash_ptr = Sector[7].StartAddr;			// FlashB
   // flag write (설정이 저장되어 있다는 플레그)
   data = 0x007e;
   Status = Flash_Program(Flash_ptr++,&data,1,&FlashStatus);
   if(Status != STATUS_SUCCESS)
   {
	   Example_Error(Status);
   }

   Length = sizeof(dsSystem);
   Status = Flash_Program(Flash_ptr,(uint16_t *)&gdsSystem,Length,&FlashStatus);
   if(Status != STATUS_SUCCESS)
   {
	   Example_Error(Status);
   }
   Flash_ptr += Length;

	gdsIMDCfgPcIfc.f16sKp = pid1_spd.param.Kp;
	gdsIMDCfgPcIfc.f16sKi = T/pid1_spd.param.Ki;
	gdsIMDCfgPcIfc.f16cKp = pid1_idc.param.Kp;
	gdsIMDCfgPcIfc.f16cKi = T/pid1_idc.param.Ki;
	gdsIMDCfgPcIfc.u16AccTime     = gu16AccTime;
	gdsIMDCfgPcIfc.u16DecTime     = gu16DecTime;
	gdsIMDCfgPcIfc.u16sensorless  = SensorlessFlag;
	gdsIMDCfgPcIfc.u16Sinusoidal  = SinusoidalFlag;
	gdsIMDCfgPcIfc.u16Pole        = gu16Pole;
	gdsIMDCfgPcIfc.u16CtrlMode    = gu16CtrlMode;
	gdsIMDCfgPcIfc.f16BaseFreq    = gfBaseFreq;
	gdsIMDCfgPcIfc.f16BaseCurrent = giqBaseCurrent;

   Length = sizeof(dsIMDCfgPcIfc);
   Status = Flash_Program(Flash_ptr,(uint16_t *)&gdsIMDCfgPcIfc,Length,&FlashStatus);
   if(Status != STATUS_SUCCESS)
   {
	   Example_Error(Status);
   }

   // Verify the values programmed.  The Program step itself does a verify
   // as it goes.  This verify is a 2nd verification that can be done.
   Status = Flash_Verify(Flash_ptr,(uint16_t *)&gdsIMDCfgPcIfc,Length,&FlashStatus);
   if(Status != STATUS_SUCCESS)
   {
	   Example_Error(Status);
   }

	EINT;   // Enable Global interrupt INTM
	ERTM;	// Enable Global realtime interrupt DBGM

}

void CONSOLE_FactoryValue( )
{
    gdsSystem.u16BoardType = BOARD_TYPE;
    gdsSystem.u16MotorType = MOTOR_TYPE;
    gu16CtrlMode		  = C_CASCADE;
	gu16Pole              = (gdsSystem.u16MotorType==M_W78)?POLES_W78:POLES_W750;
	gfBaseFreq            = (gdsSystem.u16MotorType==M_W78)?BASE_FREQ_W78:BASE_FREQ_W750;
	giqBaseCurrent        = (gdsSystem.u16MotorType==M_W78)?BASE_CURRENT_W78:BASE_CURRENT_W750;
	pid1_spd.param.Kp     = _IQ(0.75);
	pid1_spd.param.Ki     = _IQ(T/0.3);
    //pid1_idc.param.Kp     = _IQ(3.176*giqBaseCurrent/BASE_VOLTAGE);	// BLDC-Sensored
	//pid1_idc.param.Ki     = _IQ(T/0.0005);							// BLDC-Sensored
    pid1_idc.param.Kp = _IQ(0.25); 										// BLDC-Int
	pid1_idc.param.Ki = _IQ(T/0.05);									// BLDC-Int
	SensorlessFlag        = 0;
	SinusoidalFlag        = 0;
	gu16AccTime           = 1000;
	gu16DecTime           = 1000;
	Update_Cfg_parameter ();
}

void CONSOLE_LoadCfg(uint32_t u32Dflt )
{
   Uint16  *Flash_ptr;     // Pointer to a location in flash

   Flash_ptr = Sector[7].StartAddr;			// FlashB

   if(*Flash_ptr++ == 0x007e ) {	// 플래그 확인
	   memcpy((uint16_t *)&gdsSystem, Flash_ptr, sizeof(dsSystem));
	   Flash_ptr += sizeof(dsSystem);
	   memcpy((uint16_t *)&gdsIMDCfgPcIfc, Flash_ptr, sizeof(dsIMDCfgPcIfc));
		pid1_spd.param.Kp     = gdsIMDCfgPcIfc.f16sKp;
		pid1_spd.param.Ki     = T/gdsIMDCfgPcIfc.f16sKi;
		pid1_idc.param.Kp     = gdsIMDCfgPcIfc.f16cKp;
		pid1_idc.param.Ki     = T/gdsIMDCfgPcIfc.f16cKi;
		SensorlessFlag        = gdsIMDCfgPcIfc.u16sensorless;
		SinusoidalFlag        = gdsIMDCfgPcIfc.u16Sinusoidal;
		gu16AccTime           = gdsIMDCfgPcIfc.u16AccTime;
		gu16DecTime           = gdsIMDCfgPcIfc.u16DecTime;
		gu16Pole              = gdsIMDCfgPcIfc.u16Pole;
		gu16CtrlMode          = gdsIMDCfgPcIfc.u16CtrlMode;
		gfBaseFreq            = gdsIMDCfgPcIfc.f16BaseFreq;
		giqBaseCurrent        = gdsIMDCfgPcIfc.f16BaseCurrent;

		Update_Cfg_parameter ();
   } else {
		if( (ghCON.u16ArgmentNo >= 2)&&(u32Dflt&0x01) ) {
			CONSOLE_FactoryValue ();
			Update_Cfg_parameter ();
	 	}
   }

}

void CONSOLE_Factory( uint32_t BoardType, uint32_t MotorType )
{
	UART_printf ("\nLoad Default Setting...");
	CONSOLE_FactoryValue();
	if( ghCON.u16ArgmentNo >= 3 ) {
		gdsSystem.u16BoardType = BoardType;
		gdsSystem.u16MotorType = MotorType;
		gu16Pole              = (gdsSystem.u16MotorType==M_W78)?POLES_W78:POLES_W750;
		gfBaseFreq            = (gdsSystem.u16MotorType==M_W78)?BASE_FREQ_W78:BASE_FREQ_W750;
		giqBaseCurrent        = (gdsSystem.u16MotorType==M_W78)?BASE_CURRENT_W78:BASE_CURRENT_W750;
	} else if (ghCON.u16ArgmentNo >= 2) {
			gdsSystem.u16BoardType = BoardType;
	}

	UART_printf ("\nWrite to Flash...");
	UART_flush();
	CONSOLE_SaveCfg();
	UART_printf ("\nInitialized to FATORY Setting");
}

void CONSOLE_Reset()
{
	UART_printf ("\nCold Reset...");
	UART_flush();
	WDogEnable();
	while(1);
}


#define DSP28_DIVSEL   2 // Enable /2 for SYSCLKOUT
#define DSP28_PLLCR   18  // Uncomment for 90 MHz devices [90 MHz = (10MHz * 18)/2]

void DisableDog();
void IntOsc1Sel();
void InitPll(Uint16 val, Uint16 divsel);
Uint16 SCI_Boot();


void CONSOLE_SCIboot()
{
	void    (*ft)();

	UART_printf ("\nSCI boot...");
	UART_flush();

   //
	DRTM;	// Disable Global realtime interrupt DBGM
	DINT;	// Disable Global interrupt INTM

	DisableDog();
	IntOsc1Sel();
	InitPll(DSP28_PLLCR,DSP28_DIVSEL);

	DELAY_US(100);

	ft = (void (*)()) SCI_Boot();
	(*ft)();

}


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

/*****************************[ End of Program ]******************************/
