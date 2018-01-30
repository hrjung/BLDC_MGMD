/******************************************************************************
 **                  Copyright(C) 2016 Nara Control. Ltd.                  **
 **                  --------------------------------------                  **
 ******************************************************************************
    FILE NAME   : PC Interface.H
    AUTHOR      : 
    DATE        : 
    REVISION    : 
    DESCRIPTION : PC Interface

 ******************************************************************************
    HISTORY     :


 ******************************************************************************/

#ifndef _PCINTERFACE_H_
#define _PCINTERFACE_H_

// 모터 상태 감시 구조체
typedef struct {

	uint16_t u16Status;
	uint16_t u16Fault;
	float f16SpeedPu;
	float f16SpeedRpm;
	float f16CurrentPu;
	float f16CurrentA;
	float f16DcVoltage;

}  dsIMDStatusPcIfc;

// 모터 제어 구조체
typedef struct {

	uint16_t u16Direction;
	uint16_t u16Run;
	uint16_t u16Start;
	uint16_t u16CtrlSrc;
	float    f16RefSpd;
	float    f16RefCrn;

} dsIMDCtrlPcIfc;

// 모터 설정 구조체
typedef struct {

	float f16sKp;
	float f16sKi;
	float f16cKp;
	float f16cKi;
	float f16BaseFreq;
	float f16BaseCurrent;
	uint16_t u16sensorless;
	uint16_t u16Sinusoidal;
	uint16_t u16AccTime;
	uint16_t u16DecTime;
	uint16_t u16Pole;
	uint16_t u16CtrlMode;

} dsIMDCfgPcIfc;

// 모터 보호 구조체
typedef struct {

	float f16OVLevel;
	float f16UVLevel;
	float f16OCLevel;
	float f16OTLevel;
	uint16_t f16OCDelay;
	uint16_t f16Rsv;

} dsIMDPrtktPcIfc;


extern dsIMDStatusPcIfc  gdsIMDStatusPcIfc;
extern dsIMDCtrlPcIfc    gdsIMDCtrlPcIfc;
extern dsIMDCfgPcIfc     gdsIMDCfgPcIfc;

void ProcessSerialInterface(void);

extern void GUI_Status_Rd();
extern void GUI_Ctrl_Rd();
extern void GUI_Ctrl_Wr();
extern void GUI_Cfg_Rd();
extern void GUI_Cfg_Wr();
extern void GUI_Cfg_Save();
extern void GUI_Disconnect(void);

#endif

/*****************************[ End of Program ]*****************************/

