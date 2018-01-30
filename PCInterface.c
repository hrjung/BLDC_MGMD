/******************************************************************************
 **                  Copyright(C) 2016 Nara Controls Co. Ltd.                **
 **                  --------------------------------------                  **
 ******************************************************************************
	FILE NAME   : PCInterface.C
	AUTHOR      : 
	DATE        : 
	REVISION    : 
	DESCRIPTION : PC Interface

 ******************************************************************************
	HISTORY     :


 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "PeripheralHeaderIncludes.h"
#include "BLDC_IMD-Settings.h"
#include "IQmathLib.h"
#include "speed_pr.h"
#include "pid_grando.h"
#include "BLDC_IMD_var.h"

#include "SoftTimer.h"
#include "uart_printf.h"
#include "console.h"
#include "GUI_comm.h"
#include "PCInterface.h"

dsIMDStatusPcIfc  gdsIMDStatusPcIfc;
dsIMDCtrlPcIfc    gdsIMDCtrlPcIfc;
dsIMDCfgPcIfc     gdsIMDCfgPcIfc;
#pragma DATA_SECTION(gdsIMDStatusPcIfc,"DMARAML6");
#pragma DATA_SECTION(gdsIMDCtrlPcIfc,"DMARAML6");
#pragma DATA_SECTION(gdsIMDCfgPcIfc,"DMARAML6");

void ProcessSerialInterface(void)
{
	unsigned char ch;

	// Console process

	// 시리얼 포트로 들어온 데이터가 있으면 처리 과정을 수행한다.
	if (ghCDF.u16GuiFlag & GUI_IN_USE)                                      // 만약 현재 GUI 사용 상태이면 IDW 처리 함수를 호출한다.
	{
		GUI_Rx_Msg();
	}
	else                                                                    // 현재 GUI 사용 상태가 아니면 수신된 첫 바이트의 값에 따라 동작을 결정한다.
	{
		if (!UART_readbyte(&ch))
			return;

		if ((ch == START)) //&&(ghEWS_PortDirection == EWSDEFS_PORT_NULL))                                                // 첫 바이트의 값이 START (0x7E) 이면 IDW가 연결되어 메시지를 보낸 것으로 판단한다.
		{
			// 연결 Frame 확인 타이머
			//STimer_Start(&ghSTIMER.ConnectFrameTimer, CONSDEFS_GUI_CONNECT_TIME);
			// test
			GUI_Rx_STX();
		}
		else
		{
			// 첫 바이트가 START가 아닌 경우에는 하이퍼터미널 사용 중으로 판단한다.
			CONSOLE_SerialProcess(ch);
		}
	}
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void GUI_Status_Rd(void)
{


	gdsIMDStatusPcIfc.u16Status     = (Direction<<1)|EnableFlag;
	gdsIMDStatusPcIfc.f16SpeedPu    = speed1.Speed;
	gdsIMDStatusPcIfc.f16SpeedRpm   = speed1.SpeedRpm;
	#if (CURRENT_AVG)
	gdsIMDStatusPcIfc.f16CurrentPu  = DCcurrent_Fbk;
	gdsIMDStatusPcIfc.f16CurrentA   = DCcurrent_Fbk * giqBaseCurrent;
	#else
	gdsIMDStatusPcIfc.f16CurrentPu  = DCbus_current;
	gdsIMDStatusPcIfc.f16CurrentA   = DCbus_current * giqBaseCurrent;
	#endif
	gdsIMDStatusPcIfc.f16DcVoltage  = DcVoltage;
	gdsIMDStatusPcIfc.u16Fault       = 0x56;

    GUI_Tx_Msg( 0, CMD_STATUS_RD, ACK_OK, PACKET_ONE_TIME, (unsigned char*)&gdsIMDStatusPcIfc, sizeof(gdsIMDStatusPcIfc), 0 );
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void GUI_Ctrl_Rd(void)
{

	gdsIMDCtrlPcIfc.u16Run         = RunBreakFlag;
	gdsIMDCtrlPcIfc.u16Start       = gdsGui.u16Start;
	gdsIMDCtrlPcIfc.u16Direction   = gdsGui.u16Dir;
	gdsIMDCtrlPcIfc.u16CtrlSrc     = CtrlSrcFlag;
	gdsIMDCtrlPcIfc.f16RefSpd      = gdsGui.fSpdRef;
	gdsIMDCtrlPcIfc.f16RefCrn      = gdsGui.fCrnRef;

    GUI_Tx_Msg( 0, CMD_CTRL_RD, ACK_OK, PACKET_ONE_TIME, (unsigned char*)&gdsIMDCtrlPcIfc, sizeof(gdsIMDCtrlPcIfc), 0 );
}

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void GUI_Ctrl_Wr(void)
{

	uint16_t * data;

   	data = &gdsGUI_Comm_Msg.u16Rx_Data[GUIDEFS_COMM16_DATA];

	memcpy(&gdsIMDCtrlPcIfc, data, sizeof(gdsIMDCtrlPcIfc));

	RunBreakFlag    = gdsIMDCtrlPcIfc.u16Run;
	gdsGui.u16Start = gdsIMDCtrlPcIfc.u16Start;
	gdsGui.u16Dir   = gdsIMDCtrlPcIfc.u16Direction;
	CtrlSrcFlag     = gdsIMDCtrlPcIfc.u16CtrlSrc;
	gdsGui.fSpdRef  = gdsIMDCtrlPcIfc.f16RefSpd;
	gdsGui.fCrnRef  = gdsIMDCtrlPcIfc.f16RefCrn;

	GUI_Tx_Msg( 0, CMD_CTRL_WR, ACK_OK, PACKET_ONE_TIME, (unsigned char*)&gdsIMDCtrlPcIfc, sizeof(gdsIMDCtrlPcIfc), 0 );
}

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void GUI_Cfg_Rd(void)
{

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

    GUI_Tx_Msg( 0, CMD_CFG_RD, ACK_OK, PACKET_ONE_TIME, (unsigned char*)&gdsIMDCfgPcIfc, sizeof(gdsIMDCfgPcIfc), 0 );
}

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void GUI_Cfg_Wr(void)
{

	uint16_t * data;

   	data = &gdsGUI_Comm_Msg.u16Rx_Data[GUIDEFS_COMM16_DATA];

	memcpy(&gdsIMDCfgPcIfc, data, sizeof(gdsIMDCfgPcIfc));

	pid1_spd.param.Kp     = gdsIMDCfgPcIfc.f16sKp;
	pid1_spd.param.Ki     = T/gdsIMDCfgPcIfc.f16sKi;
	pid1_idc.param.Kp     = gdsIMDCfgPcIfc.f16cKp;
	pid1_idc.param.Ki     = T/gdsIMDCfgPcIfc.f16cKi;
	SensorlessFlag        = gdsIMDCfgPcIfc.u16sensorless;
	SinusoidalFlag        = gdsIMDCfgPcIfc.u16Sinusoidal;
	gu16AccTime           = gdsIMDCfgPcIfc.u16AccTime;
	gu16DecTime           = gdsIMDCfgPcIfc.u16DecTime;
	gu16Pole              = gdsIMDCfgPcIfc.u16Pole;
	gu16CtrlMode		  = gdsIMDCfgPcIfc.u16CtrlMode;
	gfBaseFreq            = gdsIMDCfgPcIfc.f16BaseFreq;
	giqBaseCurrent        = gdsIMDCfgPcIfc.f16BaseCurrent;

	Update_Cfg_parameter ();

	GUI_Tx_Msg( 0, CMD_CFG_WR, ACK_OK, PACKET_ONE_TIME, (unsigned char*)&gdsIMDCfgPcIfc, sizeof(gdsIMDCfgPcIfc), 0 );
}

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void GUI_Cfg_Save(void)
{
	GUI_Tx_Msg( 0, CMD_CFG_SAVE, ACK_OK, PACKET_ONE_TIME, (unsigned char*)&gdsIMDCfgPcIfc, sizeof(gdsIMDCfgPcIfc), 0 );
	CONSOLE_SaveCfg();
}

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void GUI_Disconnect(void)
{
    GUI_Tx_Msg( 0, GUI_CMD_DISCONNECT, ACK_OK, PACKET_ONE_TIME, 0, 0, 0 );

    ghCDF.u16GuiFlag    &= ~GUI_IN_USE;

    STimer_Deactivate( &ghSTIMER.GuiInUseTimer );
}


/*****************************[ End of Program ]******************************/
