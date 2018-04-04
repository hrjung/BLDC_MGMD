/* ==============================================================================
System Name:  	BLDC_Sensored / Sensorless

File Name:	  	BLDC_IMD.C

Description:	Primary system file for the (Trapezoidal) Sensored BLDC Control 
				Using Hall Effect Sensors  

Originator:		Digital control systems Group - Texas Instruments

Note: In this software, the default inverter is supposed to be DRV8312-EVM. 
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 03-01-2011	Version 1.0:  F2806x or F2803x target
===================================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Include header files used in the main function
#include "PeripheralHeaderIncludes.h"
#include "BLDC_IMD-Settings.h"
#include "IQmathLib.h"
#include "BLDC_IMD.h"
#include <math.h>

#include "SoftTimer.h"
#include "uart_printf.h"
#include "console.h"
#include "GUI_Comm.h"
#include "PCInterface.h"


extern void InitSciaGpio();

#ifdef FLASH
#pragma CODE_SECTION(MainISR,"ramfuncs");
void MemCopy();
void InitFlash();
#endif

// Prototype statements for functions found within this file.
interrupt void MainISR(void);
void DeviceInit();

// State Machine function prototypes
//------------------------------------
// Alpha states
void A0(void);	//state A0
void B0(void);	//state B0
void C0(void);	//state C0

// A branch states
void A1(void);	//state A1
void A2(void);	//state A2
void A3(void);	//state A3

// B branch states
void B1(void);	//state B1
void B2(void);	//state B2
void B3(void);	//state B3

// C branch states
void C1(void);	//state C1
void C2(void);	//state C2
void C3(void);	//state C3

// Variable declarations
void (*Alpha_State_Ptr)(void);	// Base States pointer
void (*A_Task_Ptr)(void);		// State pointer A branch
void (*B_Task_Ptr)(void);		// State pointer B branch
void (*C_Task_Ptr)(void);		// State pointer C branch

// Used for running BackGround in flash, and ISR in RAM
extern Uint16 *RamfuncsLoadStart, *RamfuncsLoadEnd, *RamfuncsRunStart;

#ifdef DRV830X
union DRV8301_STATUS_REG_1 DRV8301_stat_reg1;
union DRV8301_STATUS_REG_2 DRV8301_stat_reg2;
union DRV8301_CONTROL_REG_1 DRV8301_cntrl_reg1;
union DRV8301_CONTROL_REG_2 DRV8301_cntrl_reg2;
Uint16 read_drv_status = 0;
union DRV8305_IC_OPER_REG  DRV8305_ic_oper_reg;
#endif




int16	VTimer0[4];			// Virtual Timers slaved off CPU Timer 0 (A events)
int16	VTimer1[4]; 		// Virtual Timers slaved off CPU Timer 1 (B events)
int16	VTimer2[4]; 		// Virtual Timers slaved off CPU Timer 2 (C events)
int16	SerialCommsTimer;

// Global variables used in this system

// T= 0.000025 : 25us
float32 T = 0.001/ISR_FREQUENCY;    // Samping period (sec), see parameter.h 

Uint32 IsrTicker = 0;
Uint16 BackTicker = 0;

volatile Uint16 CtrlSrcFlag = 1;
Uint16 CALIBRATE_FLAG = 1;
Uint16 CALIBRATE_TIME = 0x07FF;		//give the calibration filters about 100ms (~10tc) to settle

Uint16 PreviousState;
Uint16 ClosedFlag = false;
Uint16 Direction = 1;
Uint16 SensorlessFlag = 0;
Uint16 SinusoidalFlag = 0;


Uint32 VirtualTimer = 0;
//Uint16 ILoopFlag = false;
Uint16 SpeedLoopFlag = false;
int16  DFuncDesired = (int16)_IQtoQ15(_IQ(0.15));
int16  DfuncTesting = (int16)_IQtoQ15(_IQ(0.1)); // 0.12

Uint16 AlignFlag = 0x000F;
Uint16 LoopCount = 0;

Uint16 SpeedLoopLog_flag = 0;
Uint16 ClosedLog_flag = 0;

#if 0 //(BUILDLEVEL<= LEVEL2)
Uint32 CmtnPeriodTarget = 168;
Uint32 CmtnPeriodSetpt = 1024;
Uint32 RampDelay = 10;
#define BEGIN_START_RPM		50
#define END_START_RPM       100
int32 RAMP_START_RATE;
int32 RAMP_END_RATE;
#else
#define BEGIN_START_RPM		20
#define END_START_RPM       50
int32 RAMP_START_RATE;
int32 RAMP_END_RATE;
Uint32 CmtnPeriodTarget = 0x00000350; //848
Uint32 CmtnPeriodSetpt = 0x00000500;  //1280
Uint32 RampDelay = 10;
#endif

//_iq CurrentSet = _IQ(0.1);
_iq CurrentSet=_IQ(0.04);	// 전압 0.007Ω x 0.6 A (No Load, 4980rpm) x 10배  = 0.042V  →  0.042V / 3.3V x 10
_iq current[3] = {0, 0, 0};
_iq DCbus_current = 0;
#if (CURRENT_AVG)
_iq DCcurrent_Sum = 0;
_iq DCcurrent_Fbk = 0;
Uint16 DCcurrent_Cnt = 0;
Uint16 DCcurrent_SumCnt = 0;
#endif
_iq SpeedRef=_IQ(0.1);		// 0.3 * 4980	= 1494 rpm (25회/초)
_iq tempIdc = 0;

// Lopass Filter
_iq iqX[2]={_IQ(0),_IQ(0)}, iqY[2]={_IQ(0),_IQ(0)};
_iq iqB[2]={_IQ(1),_IQ(1)}, iqA[2]={_IQ(1),_IQ(-0.612800788139932)};
_iq iqG = _IQ(0.19359960593003397);

dsSystem gdsSystem;

dsIMDCtrlIfc gdsIn;
dsIMDCtrlIfc gdsGui;
dsInBuf gdsInBUf;

Uint16 gu16AccTime  = 5000;
Uint16 gu16DecTime  = 5000;
Uint16 gu16Pole     = POLES;
//Uint16 gu16CtrlMode = C_CASCADE;
Uint16 gu16CtrlMode = C_SPEED;
float  gfBaseFreq   = BASE_FREQ;
_iq giqBaseCurrent   = BASE_CURRENT;
_iq giqCurrentScaler = CURRENT_SCALER;


_iq BemfA = 0;
_iq BemfB = 0;
_iq BemfC = 0;
_iq DcVoltage = 0;

int16 PwmDacCh1 = 0;
int16 PwmDacCh2 = 0;
int16 PwmDacCh3 = 0;

int16 DlogCh1 = 0;
int16 DlogCh2 = 0;
int16 DlogCh3 = 0;
int16 DlogCh4 = 0;


volatile Uint16 EnableFlag = false;
volatile Uint16 RunBreakFlag = false;
volatile Uint16 FailFlag = false;

Uint16 RunMotor = false;

//_iq CurrentStartup = _IQ(0.1);
_iq CurrentStartup = _IQ(0.04);

_iq BemfA_offset = _IQ15(0.0);
_iq BemfB_offset = _IQ15(0.0);
_iq BemfC_offset = _IQ15(0.0);
_iq DcVolt_offset = _IQ15(0.0);
_iq IA_offset = _IQ15(0.5);
_iq IB_offset = _IQ15(0.5);
_iq IC_offset = _IQ15(0.5);
_iq IDC_offset = _IQ15(0.5);
_iq Spd_offset = _IQ15(0.0);
_iq cal_filt_gain;	 

// Instance PID regulator to regulate the DC-bus current and speed
PID_GRANDO_CONTROLLER pid1_idc = {PID_TERM_DEFAULTS,PID_PARAM_DEFAULTS,PID_DATA_DEFAULTS};
PID_GRANDO_CONTROLLER pid1_spd = {PID_TERM_DEFAULTS,PID_PARAM_DEFAULTS,PID_DATA_DEFAULTS};

// Instance a PWM driver instance
PWMGEN pwm1 = PWMGEN_DEFAULTS;

// Instance a PWM DAC driver instance
PWMDAC pwmdac1 = PWMDAC_DEFAULTS;

// Instance a Hall effect driver
HALL3 hall1 = HALL3_DEFAULTS;

// Instance a ramp controller to smoothly ramp the frequency
RMPCNTL rc1 = RMPCNTL_DEFAULTS;

// Instance a RAMP2 Module
RMP2 rmp2 = RMP2_DEFAULTS;

// Instance a RAMP3 Module
RMP3 rmp3 = RMP3_DEFAULTS;

// Instance a MOD6 Module
MOD6CNT mod1 = MOD6CNT_DEFAULTS;

// Instance a IMPULSE Module
IMPULSE impl1 = IMPULSE_DEFAULTS;

// Instance a COMTRIG Module	(Commutation Trigger Module)
CMTN cmtn1 = CMTN_DEFAULTS;

// Instance a SPEED_PR Module
SPEED_MEAS_CAP speed1 = SPEED_MEAS_CAP_DEFAULTS;

// Create an instance of DATALOG Module
DLOG_4CH dlog = DLOG_4CH_DEFAULTS; 

Uint16 DRV_RESET = 0;

/*
// SCIA  8-bit word, baud rate 0x000F, default, 1 STOP bit, no parity
void scia_init()
{

    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function

 	SciaRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
	SciaRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
                                   // Disable RX ERR, SLEEP, TXWAKE

	SciaRegs.SCICTL2.bit.TXINTENA =1;
	SciaRegs.SCICTL2.bit.RXBKINTENA =1;

	SciaRegs.SCIHBAUD    =0x0000;  // 115200 baud @LSPCLK = 22.5MHz (90 MHz SYSCLK).
    SciaRegs.SCILBAUD    =0x0017;

	SciaRegs.SCICTL1.all =0x0023;  // Relinquish SCI from Reset

    return;
}
*/

// Test 1,SCIA  DLB, 8-bit word, baud rate 0x0103, default, 1 STOP bit, no parity
void scia_echoback_init()
{
    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function

 	SciaRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
	SciaRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
                                   // Disable RX ERR, SLEEP, TXWAKE

	SciaRegs.SCICTL2.bit.TXINTENA =1;
	SciaRegs.SCICTL2.bit.RXBKINTENA =1;

	//SciaRegs.SCIHBAUD    =0x0001;  // 9600 baud @LSPCLK = 22.5MHz (90 MHz SYSCLK).
    //SciaRegs.SCILBAUD    =0x0024;

	SciaRegs.SCIHBAUD    =0x0000;  // 115200 baud @LSPCLK = 22.5MHz (90 MHz SYSCLK).
    SciaRegs.SCILBAUD    =0x0017;

    SciaRegs.SCICTL1.all =0x0023;  // Relinquish SCI from Reset
}

// Transmit a character from the SCI
void scia_xmit(Uint16 a)
{
    while (SciaRegs.SCIFFTX.bit.TXFFST != 0) {}
    //while (SciaRegs.SCICTL2.bit.TXRDY != 0) {}
    SciaRegs.SCITXBUF=a;

}

void scia_msg(char * msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
        scia_xmit((Uint16)(msg[i]&0xff));
        i++;
    }
}

// Initalize the SCI FIFO
void scia_fifo_init()
{
    SciaRegs.SCIFFTX.all=0xE040;
    SciaRegs.SCIFFRX.all=0x2044;
    SciaRegs.SCIFFCT.all=0x0;

}

void Update_Cfg_Gpio()
{
	EALLOW; // below registers are "protected", allow access.

	if (gdsSystem.u16BoardType == B_IMMB ) {

		//  GPIO-6 -  PIN FUNCTION = -- REGEN when IMMB (ACTIVE LOWW) --
		GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;	// 0=GPIO,  1=EPWM4A,  2=SYNCI,  3=SYNCO
		GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;		// 1=OUTput,  0=INput
		GpioDataRegs.GPADAT.bit.GPIO6 = 1;
		//  GPIO-7 -  PIN FUNCTION = -- SPEED-OUT when IMMB --
		GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0;	// 0=GPIO,  1=EPWM4B,  2=SYNCI,  3=SYNCO
		GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;		// 1=OUTput,  0=INput
		GpioDataRegs.GPADAT.bit.GPIO7 = 0;

		//  GPIO-8 -  PIN FUNCTION = -- LED5 (RED) when IMMB --
		GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0;	    // 0=GPIO,  1=EPWM5A,  2=Resv,  3=ADCSOCA
		GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;		// 1=OUTput,  0=INput
		GpioDataRegs.GPADAT.bit.GPIO8 = 1;
		//  GPIO-10 - PIN FUNCTION = -- LED4 (GREEN) when IMMB --
		GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 0;	// 0=GPIO,  1=EPWM6A,  2=Resv,  3=ADCSOCA
		GpioCtrlRegs.GPADIR.bit.GPIO10 = 1;		// 1=OUTput,  0=INput
		GpioDataRegs.GPADAT.bit.GPIO10 = 1;

		/* pwm으로 사용 못함
		//  GPIO-12 - PIN FUNCTION = -- LED4 (GREEN) when IMMB --
		GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;	// 0=GPIO, 1=TZ1n, 2=SCITXDA, 3=SPISIMOB
		GpioCtrlRegs.GPADIR.bit.GPIO12 = 1;		// 1=OUTput,  0=INput
		GpioDataRegs.GPADAT.bit.GPIO12 = 1;
		//  GPIO-13 - PIN FUNCTION = -- LED4 (GREEN) when IMMB --
		GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;	// 0=GPIO, 1=TZ2n, 2=SCITXDA, 3=SPISIMOB
		GpioCtrlRegs.GPADIR.bit.GPIO13 = 1;		// 1=OUTput,  0=INput
		GpioDataRegs.GPADAT.bit.GPIO13 = 1;
		//  GPIO-14 - PIN FUNCTION = -- LED4 (GREEN) when IMMB --
		GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;	// 0=GPIO, 1=TZ3n, 2=SCITXDA, 3=SPISIMOB
		GpioCtrlRegs.GPADIR.bit.GPIO13 = 1;		// 1=OUTput,  0=INput
		GpioDataRegs.GPADAT.bit.GPIO13 = 1;
		*/

		//  GPIO-16 - PIN FUNCTION = --Spare--
			GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1;	// 0=GPIO, 1=SPISIMOA, 2=Resv CAN-B, 3=TZ2n
		//--------------------------------------------------------------------------------------
		//  GPIO-17 - PIN FUNCTION = --Spare--
			GpioCtrlRegs.GPAMUX2.bit.GPIO17 =1;	// 0=GPIO, 1=SPISOMIA, 2=Resv CAN-B, 3=TZ3n
		//--------------------------------------------------------------------------------------
		//  GPIO-18 - PIN FUNCTION = --Spare--
			GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1;	// 0=GPIO, 1=SPICLKA, 2=SCITXDB, 3=XCLKOUT
		//--------------------------------------------------------------------------------------
		//  GPIO-19 - PIN FUNCTION = --Spare--
			GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;	// 0=GPIO, 1=SPISTEA, 2=SCIRXDB, 3=ECAP1
			GpioCtrlRegs.GPBDIR.bit.GPIO50 = 1;		// 1=OUTput,  0=INput
			GpioDataRegs.GPBDAT.bit.GPIO50 = 1;		//

		//  GPIO-34 - BOOT-MODE
		GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;	// 0=GPIO,  1=COMP2OUT,  2=Resv,  3=COMP3OUT
		GpioCtrlRegs.GPBDIR.bit.GPIO34 = 0;		// 1=OUTput,  0=INput
		//  GPIO-39 - INT_SPEED
		GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=Resv
		GpioCtrlRegs.GPBDIR.bit.GPIO39 = 0;		// 1=OUTput,  0=INput
		//  GPIO-40 - ALARM-OUT
		GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=Resv
		GpioCtrlRegs.GPBDIR.bit.GPIO40 = 0;		// 1=OUTput,  0=INput
		//  GPIO-41 - ALAM-RESET
		GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=Resv
		GpioCtrlRegs.GPBDIR.bit.GPIO41 = 0;		// 1=OUTput,  0=INput
		//  GPIO-42 - CW/CCW
		GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=Resv
		GpioCtrlRegs.GPBDIR.bit.GPIO42 = 0;		// 1=OUTput,  0=INput
		//  GPIO-43 - RUN/BREAK
		GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=Resv
		GpioCtrlRegs.GPBDIR.bit.GPIO43 = 0;		// 1=OUTput,  0=INput
		//  GPIO-44 - START/STOP
		GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=Resv
		GpioCtrlRegs.GPBDIR.bit.GPIO44 = 0;		// 1=OUTput,  0=INput
		//  GPIO-50 - Cluch
		GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=Resv
		GpioCtrlRegs.GPBDIR.bit.GPIO50 = 1;		// 1=OUTput,  0=INput
		GpioDataRegs.GPBDAT.bit.GPIO50 = 0;		//
		//  GPIO-51 - EnGate
		GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=Resv
		GpioCtrlRegs.GPBDIR.bit.GPIO51 = 1;		// 1=OUTput,  0=INput
		GpioDataRegs.GPBDAT.bit.GPIO51 = 0;		//
		//  GPIO-52 - DCcal
		GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=Resv
		GpioCtrlRegs.GPBDIR.bit.GPIO52 = 1;		// 1=OUTput,  0=INput
		GpioDataRegs.GPBDAT.bit.GPIO52 = 1;		//
		//  GPIO-53 - Fault
		GpioCtrlRegs.GPBMUX2.bit.GPIO53 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=Resv
		GpioCtrlRegs.GPBDIR.bit.GPIO53 = 0;		// 1=OUTput,  0=INput
		//  GPIO-54 - OC
		GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=Resv
		GpioCtrlRegs.GPBDIR.bit.GPIO54 = 0;		// 1=OUTput,  0=INput

		//  GPIO-55 - DIP5
		GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=Resv
		GpioCtrlRegs.GPBDIR.bit.GPIO55 = 0;		// 1=OUTput,  0=INput
		//  GPIO-56 - DIP6
		GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=Resv
		GpioCtrlRegs.GPBDIR.bit.GPIO56 = 0;		// 1=OUTput,  0=INput
		//  GPIO-57 - DIP7
		GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=Resv
		GpioCtrlRegs.GPBDIR.bit.GPIO57 = 0;		// 1=OUTput,  0=INput
		//  GPIO-58 - DIP8
		GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=Resv
		GpioCtrlRegs.GPBDIR.bit.GPIO58 = 0;		// 1=OUTput,  0=INput

	} else {

		if (gdsSystem.u16BoardType != B_DRV8301HC) {
			GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;	// 0=GPIO, 1=TZ1n, 2=SCITXDA, 3=SPISIMOB
			GpioCtrlRegs.GPADIR.bit.GPIO12 = 0;		// 1=OUTput,  0=INput
		}else {
			//  GPIO-12 - PIN FUNCTION = -- LED1 when DRV8301HC --
			GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;	// 0=GPIO, 1=TZ1n, 2=SCITXDA, 3=SPISIMOB
			GpioCtrlRegs.GPADIR.bit.GPIO12 = 1;		// 1=OUTput,  0=INput
			GpioDataRegs.GPADAT.bit.GPIO12 = 0;
		}

		//  GPIO-15 - PIN FUNCTION = -- LED2 when DRV8301HC --
		if (gdsSystem.u16BoardType != B_DRV8301HC) {
			GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;	// 0=GPIO,  1=ECAP2,  2=SCIRXDB,  3=SPISTEB
			GpioCtrlRegs.GPADIR.bit.GPIO15 = 0;		// 1=OUTput,  0=INput
		} else {
			GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;	// 0=GPIO,  1=ECAP2,  2=SCIRXDB,  3=SPISTEB
			GpioCtrlRegs.GPADIR.bit.GPIO15 = 1;		// 1=OUTput,  0=INput
			GpioDataRegs.GPADAT.bit.GPIO15 = 0;
		}

		//  GPIO-19 - PIN FUNCTION = --SPI-/CS when LUNCHXL (BOOST8301은 FULL-DOWN되어 있음)--
		if (gdsSystem.u16BoardType != B_DRV8301HC) {
			GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;	// 0=GPIO,  1=HRCAP2,  2=EQEP2S,  3=SPISTEB
			GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;		// 1=OUTput,  0=INput
			GpioDataRegs.GPACLEAR.bit.GPIO19 = 1;	// uncomment if --> Set Low initially
		} else {
			GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;	// 0=GPIO,  1=HRCAP2,  2=EQEP2S,  3=SPISTEB
			GpioCtrlRegs.GPADIR.bit.GPIO19 = 0;		// 1=OUTput,  0=INput
		}

		//  GPIO-27 - PIN FUNCTION = --SPI-/CS when DRV8301HC --
		if (gdsSystem.u16BoardType == B_DRV8301HC) {
			GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 0;	// 0=GPIO,  1=HRCAP2,  2=EQEP2S,  3=SPISTEB
			GpioCtrlRegs.GPADIR.bit.GPIO27 = 1;		// 1=OUTput,  0=INput
			GpioDataRegs.GPACLEAR.bit.GPIO27 = 1;	// uncomment if --> Set Low initially
		} else {
			GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 0;	// 0=GPIO,  1=HRCAP2,  2=EQEP2S,  3=SPISTEB
			GpioCtrlRegs.GPADIR.bit.GPIO27 = 0;		// 1=OUTput,  0=INput
		}

		//  GPIO-34 - PIN FUNCTION = -- LED1 when LUNCHXL --
		if (gdsSystem.u16BoardType == B_DRV8301HC) {
			GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;	// 0=GPIO,  1=COMP2OUT,  2=Resv,  3=COMP3OUT
			GpioCtrlRegs.GPBDIR.bit.GPIO34 = 0;		// 1=OUTput,  0=INput
		} else {
			GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;	// 0=GPIO,  1=COMP2OUT,  2=Resv,  3=COMP3OUT
			GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;		// 1=OUTput,  0=INput
			GpioDataRegs.GPBDAT.bit.GPIO34 = 0;
		}

		//  GPIO-39 - PIN FUNCTION = -- LED2 when LUNCHXL --
		if (gdsSystem.u16BoardType == B_DRV8301HC) {
			GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=Resv
			GpioCtrlRegs.GPBDIR.bit.GPIO39 = 0;		// 1=OUTput,  0=INput
		} else {
			GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 0;	// 0=GPIO,  1=COMP2OUT,  2=Resv,  3=COMP3OUT
			GpioCtrlRegs.GPBDIR.bit.GPIO39 = 1;		// 1=OUTput,  0=INput
			GpioDataRegs.GPBDAT.bit.GPIO39 = 0;
		}

		//  GPIO-43 - PIN FUNCTION = -- DC_CAL when DRV8301HC --
		if (gdsSystem.u16BoardType == B_DRV8301HC) {
			GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 0;	// 0=GPIO,  1=Resv,  2=Resv,  3=Resv
			GpioCtrlRegs.GPBDIR.bit.GPIO43 = 1;		// 1=OUTput,  0=INput
			GpioDataRegs.GPBDAT.bit.GPIO43 = 1;
		} else {
			GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 0;	// 0=GPIO,  1=COMP2OUT,  2=Resv,  3=COMP3OUT
			GpioCtrlRegs.GPBDIR.bit.GPIO43 = 0;		// 1=OUTput,  0=INput
		}

		//  GPIO-50 - PIN FUNCTION = -- EN_GATE when LUNCHXL --
		GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 0;	// 0=GPIO,  1=EQEP1A,  2=MDXA,  3=TZ1
		if (gdsSystem.u16BoardType != B_DRV8301HC) {
			GpioCtrlRegs.GPBDIR.bit.GPIO50 = 1;		// 1=OUTput,  0=INput
			GpioDataRegs.GPBDAT.bit.GPIO50 = 1;		// Value = 1
		} else {
			GpioCtrlRegs.GPBDIR.bit.GPIO50 = 0;		// 1=OUTput,  0=INput
		}

		//  GPIO-51 - PIN FUNCTION = -- EN_GATE when B_DRV8301HC --
		//  GPIO-51 - PIN FUNCTION = -- DC_CAL when LUNCHXL --
		if (gdsSystem.u16BoardType == B_DRV8301HC) {
			// Enable Gate
			GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 0;	// 0=GPIO,  1=EQEP1B,  2=MDRA,  3=TZ2
			GpioCtrlRegs.GPBDIR.bit.GPIO51 = 1;		// 1=OUTput,  0=INput
			GpioDataRegs.GPBDAT.bit.GPIO51 = 1;		// Value = 1
		} else {
			// DC calibration
			GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 0;	// 0=GPIO,  1=EQEP1B,  2=MDRA,  3=TZ2
			GpioCtrlRegs.GPBDIR.bit.GPIO51 = 1;		// 1=OUTput,  0=INput
			GpioDataRegs.GPBDAT.bit.GPIO51 = 0;		// Value = 0
		}
	}

	EDIS;	// Disable register access

}

void Update_Ctrl_parameter ()
{
	if(gdsGui.u16Start == 0)
	{
		RunBreakFlag = 0;
	}

}

void Update_Cfg_parameter ()
{
	speed1.BaseRpm     = 120*((Uint32)gfBaseFreq/gu16Pole);
 	speed1.SpeedScaler = (Uint32)(ISR_FREQUENCY/(1*gfBaseFreq*0.001));
 	// current scaler
    if (gdsSystem.u16BoardType == B_IMMB) {
    	giqCurrentScaler = 3.3 / (giqBaseCurrent * 0.02);
    } else if (gdsSystem.u16BoardType == B_DRV8301HC) {
    	giqCurrentScaler = 3.3 / (giqBaseCurrent * 0.02);
    } else if (gdsSystem.u16BoardType == B_BOOST8301) {
    	giqCurrentScaler = 3.3 / (giqBaseCurrent * 0.1);
    } else {
    	giqCurrentScaler = 3.3 / (giqBaseCurrent * 0.07);
    }

}

// 초기화 - S/W적인 기능 요소
void Initialize()
{

	memset (&gdsIn, 0, sizeof(dsIMDCtrlIfc));
	memset (&gdsGui, 0, sizeof(dsIMDCtrlIfc));
	memset (&gdsInBUf, 0, sizeof(dsInBuf));

	/*
	RunMotor = false;
	speed1.Speed     = _IQ(0.0);
	speed1.SpeedRpm = 0;
	DCbus_current = _IQ(0.0);

	EALLOW;
 	EPwm1Regs.TZFRC.bit.OST=1;
	EPwm2Regs.TZFRC.bit.OST=1;
	EPwm3Regs.TZFRC.bit.OST=1;
 	EDIS;
	*/

	// Gui 제어 기본값
	gdsGui.fSpdRef = 0.1;
	gdsGui.fCrnRef = 0.04;
	gdsGui.u16Dir = 1;
	gdsGui.u16Start = 0;

	// 플래시에서 설정값을 로드 한다.
	ghCON.u16ArgmentNo = 2;
	CONSOLE_LoadCfg(1);

	Update_Cfg_Gpio();

	Update_Cfg_parameter();

}

// 설정업데이트
void UpdateParameters()
{

}

uint16_t gu16LEDCnt = 0;
// LED 출력 단자에 대한 처리
void ProcessLed()
{
	if (gdsSystem.u16BoardType == B_IMMB) {
		if (EnableFlag == 1) {
			gu16LEDCnt = ++gu16LEDCnt % 200;	// 300msec Flash
			if (gu16LEDCnt<100) {
				GpioDataRegs.GPADAT.bit.GPIO8 = 0;
			} else {
				GpioDataRegs.GPADAT.bit.GPIO8 = 1;
			}
		} else {
			GpioDataRegs.GPADAT.bit.GPIO8 = 1;
		}

		if (FailFlag == 1) {
			GpioDataRegs.GPADAT.bit.GPIO10 = 0;
		} else {
			GpioDataRegs.GPADAT.bit.GPIO10 = 1;
		}
	} else if (gdsSystem.u16BoardType == B_DRV8301HC) {
		if (EnableFlag == 1) {
			gu16LEDCnt = ++gu16LEDCnt % 200;	// 300msec Flash
			if (gu16LEDCnt<100) {
				GpioDataRegs.GPADAT.bit.GPIO12 = 1;
			} else {
				GpioDataRegs.GPADAT.bit.GPIO12 = 0;

			}
		} else {
			GpioDataRegs.GPADAT.bit.GPIO12 = 0;
		}

		if (FailFlag == 1) {
			GpioDataRegs.GPADAT.bit.GPIO15 = 1;
		} else {
			GpioDataRegs.GPADAT.bit.GPIO15 = 0;
		}
	} else {
		if (EnableFlag == 1) {
			gu16LEDCnt = ++gu16LEDCnt % 200;	// 300msec Flash
			if (gu16LEDCnt<100) {
				GpioDataRegs.GPBDAT.bit.GPIO39 = 0;
			} else {
				GpioDataRegs.GPBDAT.bit.GPIO39 = 1;

			}
		} else {
			GpioDataRegs.GPBDAT.bit.GPIO39 = 1;
		}

		if (FailFlag == 1) {
			GpioDataRegs.GPBDAT.bit.GPIO34 = 0;
		} else {
			GpioDataRegs.GPBDAT.bit.GPIO34 = 1;
		}
	}

}

void ProcessAnalogInput()
{
	if (gdsSystem.u16BoardType == B_IMMB) {
	    DcVoltage =  ((AdcResult.ADCRESULT7)*0.00024414)-DcVolt_offset;
	    DcVoltage *= (95.3+4.99) / 4.99 *3.3;
		gdsIn.fSpdRef =  ((AdcResult.ADCRESULT8)*0.00024414)-Spd_offset;
	} else {
		DcVoltage =  ((AdcResult.ADCRESULT3)*0.00024414)-DcVolt_offset;
		if (gdsSystem.u16BoardType == B_BOOST8301) {
		  DcVoltage *= (34.8+4.99) / 4.99 *3.3;
		} else if (gdsSystem.u16BoardType == B_BOOST8305) {
		  DcVoltage *= (62.0+4.99) / 4.99 *3.3;
		} else if (gdsSystem.u16BoardType == B_DRV8301HC) {
		  DcVoltage *= (95.3+4.99) / 4.99 *3.3;
		}
		gdsIn.fSpdRef =  ((AdcResult.ADCRESULT7)*0.00024414)-Spd_offset;
	}

}

// 디지털 입력 처리 & 디바운스 처리 필요함
void ProcessDigitalInput()
{

	if ( gdsSystem.u16BoardType == B_IMMB ) {	// B_IMMB (pull-up이 없어서 일단 처리 보류 )
		 if ((gdsInBUf.u16Start == 0x0e)&&(GpioDataRegs.GPBDAT.bit.GPIO55)&&(gdsIn.u16Start == 0)){
			 gdsIn.u16Start =  1;
			 CtrlSrcFlag = 0;
		 } else if ((gdsInBUf.u16Start==0x01)&&(!GpioDataRegs.GPBDAT.bit.GPIO55)&&(gdsIn.u16Start == 1)) {
			 gdsIn.u16Start =  0;
			 CtrlSrcFlag = 0;
		 }
		 gdsInBUf.u16Start = gdsInBUf.u16Start>>1;
		 gdsInBUf.u16Start = (GpioDataRegs.GPBDAT.bit.GPIO55) ?  gdsInBUf.u16Start|0x08 : gdsInBUf.u16Start & 0xf7; // GPIO-55

		 if ((gdsInBUf.u16Dir == 0x0e)&&(GpioDataRegs.GPBDAT.bit.GPIO56)){
			 gdsIn.u16Dir =  1;
		 } else if ((gdsInBUf.u16Dir == 0x01)&&(!GpioDataRegs.GPBDAT.bit.GPIO56)) {
			 gdsIn.u16Dir =  0;
		 }
		 gdsInBUf.u16Dir = gdsInBUf.u16Dir >> 1;
		 gdsInBUf.u16Dir = (GpioDataRegs.GPBDAT.bit.GPIO56) ? gdsInBUf.u16Dir | 0x08 : gdsInBUf.u16Dir & 0xf7; // GPIO-56
	} else if ( gdsSystem.u16BoardType != B_DRV8301HC) {	// BOOST8301 & BOOST8305
		 if ((gdsInBUf.u16Start == 0x0e)&&(!GpioDataRegs.GPADAT.bit.GPIO20)&&(gdsIn.u16Start == 0) ){
			 gdsIn.u16Start =  1;
			 CtrlSrcFlag = 0;
		 } else if ((gdsInBUf.u16Start==0x01)&&(GpioDataRegs.GPADAT.bit.GPIO20)&&(gdsIn.u16Start == 1)) {
			 gdsIn.u16Start =  0;
			 CtrlSrcFlag = 0;
		 }
		 gdsInBUf.u16Start = gdsInBUf.u16Start>>1;
		 gdsInBUf.u16Start = (GpioDataRegs.GPADAT.bit.GPIO20) ? gdsInBUf.u16Start & 0xf7 : gdsInBUf.u16Start|0x08; // GPIO-20

		 if ((gdsInBUf.u16Dir == 0x0e)&&(!GpioDataRegs.GPADAT.bit.GPIO21)){
			 gdsIn.u16Dir =  1;
		 } else if ((gdsInBUf.u16Dir == 0x01)&&(GpioDataRegs.GPADAT.bit.GPIO21)) {
			 gdsIn.u16Dir =  0;
		 }
		 gdsInBUf.u16Dir = gdsInBUf.u16Dir >> 1;
		 gdsInBUf.u16Dir = (GpioDataRegs.GPADAT.bit.GPIO21) ? gdsInBUf.u16Dir & 0xf7 : gdsInBUf.u16Dir | 0x08; // GPIO-21
	} else {				// DRV8301HC
		 if ((gdsInBUf.u16Start == 0x0e)&&(!GpioDataRegs.GPADAT.bit.GPIO9)){
			 gdsIn.u16Start =  1;
			 CtrlSrcFlag = 0;
		 } else if ((gdsInBUf.u16Start == 0x01)&&(GpioDataRegs.GPADAT.bit.GPIO9)) {
			 gdsIn.u16Start =  0;
			 CtrlSrcFlag = 0;
		 }
		 gdsInBUf.u16Start = gdsInBUf.u16Start>>1;
		 gdsInBUf.u16Start = (GpioDataRegs.GPADAT.bit.GPIO9) ? gdsInBUf.u16Start & 0xf7 : gdsInBUf.u16Start|0x08; // GPIO-9

		 if ((gdsInBUf.u16Dir == 0x0e)&&(!GpioDataRegs.GPADAT.bit.GPIO7)){
			 gdsIn.u16Dir =  1;
		 } else if ((gdsInBUf.u16Dir == 0x01)&&(GpioDataRegs.GPADAT.bit.GPIO7)) {
			 gdsIn.u16Dir =  0;
		 }
		 gdsInBUf.u16Dir = gdsInBUf.u16Dir >> 1;
		 gdsInBUf.u16Dir = (GpioDataRegs.GPADAT.bit.GPIO7) ? gdsInBUf.u16Dir & 0xf7 : gdsInBUf.u16Dir | 0x08; // GPIO-7
	}
}

// 제어 단자 입력에 대한 처리
void ProcessInput()
{
	ProcessAnalogInput();
	//hrjung temp ProcessDigitalInput();
}

// 제어단자에 대한 처리
void ProcessControl()
{
	if ( CtrlSrcFlag == 1 ) {
		EnableFlag = gdsGui.u16Start;
		Direction  = gdsGui.u16Dir;
		SpeedRef   = _IQ(gdsGui.fSpdRef);
		CurrentSet = _IQ(gdsGui.fCrnRef);
		RunMotor = RunBreakFlag;
	} else {
		EnableFlag = gdsIn.u16Start;
		Direction  = gdsIn.u16Dir;
		SpeedRef   = _IQ(gdsIn.fSpdRef);
	}

	if(EnableFlag == 0)
	{
		SpeedLoopLog_flag = 0;
		ClosedLog_flag = 0;
	}
}


// 제어단자 출력에 대한 처리
void ProcessOutput()
{

}

void main(void)
{
//    volatile int status = 0;
//	char ch;
	
	DeviceInit();	// Device Life support & GPIO		

    // For this example, only init the pins for the SCI-A port.
    // This function is found in the F2806x_Sci.c file.
	InitSciaGpio();

// Only used if running from FLASH
// Note that the variable FLASH is defined by the compiler

#ifdef FLASH
// Copy time critical code and Flash setup code to RAM
// The  RamfuncsLoadStart, RamfuncsLoadEnd, and RamfuncsRunStart
// symbols are created by the linker. Refer to the linker files. 
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);

// Call Flash Initialization to setup flash waitstates
// This function must reside in RAM
	InitFlash();	// Call the flash wrapper init function
#endif //(FLASH)



// Tasks State-machine init
   Alpha_State_Ptr = &A0;
   A_Task_Ptr = &A1;
   B_Task_Ptr = &B1;

   scia_fifo_init();	   // Initialize the SCI FIFO
   scia_echoback_init();

   #if(0)
   scia_msg("\n\rPlease enter key to control the motor:");
   while(SciaRegs.SCIFFRX.bit.RXFFST !=1) { } // wait for XRDY =1 for empty state
   ch = SciaRegs.SCIRXBUF.all;
   scia_msg("\n\r");
   EnableFlag = true;

   // Waiting for enable flag set
   while (EnableFlag==false) 
   {
     BackTicker++;
   }
   #endif

// Initialize all the Device Peripherals:
// This function is found in DSP280x_CpuTimers.c
   InitCpuTimers();

   // Timing sync for background loops
   // Timer period definitions found in device specific PeripheralHeaderIncludes.h
   CpuTimer1Regs.PRD.all =  mSec1;		// A tasks
   CpuTimer2Regs.PRD.all =  mSec5;		// B tasks
   CpuTimer1Regs.TCR.bit.TRB = 1;		// TRB를 세트 해주어야 PRD.all 최초 값이 적용됨
   CpuTimer1Regs.TCR.bit.TIE = 1;
   CpuTimer1.InterruptCount = 0;
   StartCpuTimer1();

// Configure CPU-Timer 0 to interrupt every ISR Period:
// 60MHz CPU Freq, ISR Period (in uSeconds)
// This function is found in DSP280x_CpuTimers.c
   ConfigCpuTimer(&CpuTimer0, SYSTEM_FREQUENCY, 1000/ISR_FREQUENCY);
   StartCpuTimer0();

   #if defined(DRV830X)
   // Initialize SPI for communication to the DRV8301
   #if (!F2806x_DEVICE_H)
   DRV830X_SPI_Init(&SpibRegs);
   #else
   DRV830X_SPI_Init(&SpiaRegs);
   #endif
   #endif

// Reassign ISRs. 
// Reassign the PIE vector for TINT0 to point to a different 
// ISR then the shell routine found in DSP280x_DefaultIsr.c.
// This is done if the user does not want to use the shell ISR routine
// but instead wants to use their own ISR.

	EALLOW;	// This is needed to write to EALLOW protected registers
	PieVectTable.TINT0 = &MainISR;			// 타이머 인터럽트 함수
	EDIS;   // This is needed to disable write to EALLOW protected registers

// Enable PIE group 1 interrupt 7 for TINT0
    PieCtrlRegs.PIEIER1.all = M_INT7;

// Enable CPU INT1 for TINT0:
	IER |= M_INT1;
	// Enable Global realtime interrupt DBGM

	// 초기화 - S/W적인 기능 요소
	Initialize();

 // Initialize the SPEED_PR module
 	speed1.InputSelect = 0;

	// Initialize PWM module
    pwm1.PeriodMax = (SYSTEM_FREQUENCY/PWM_FREQUENCY)*1000;  // Asymmetric PWM 
    pwm1.DutyFunc = ALIGN_DUTY;            					 // DutyFunc = Q15
	BLDCPWM_INIT_MACRO(pwm1)

// Initialize PWMDAC module
	pwmdac1.PeriodMax = 500;   // 3000->10kHz, 1500->20kHz, 1000-> 30kHz, 500->60kHz
    pwmdac1.PwmDacInPointer0 = &PwmDacCh1;
    pwmdac1.PwmDacInPointer1 = &PwmDacCh2;
    pwmdac1.PwmDacInPointer2 = &PwmDacCh3;
    if (gdsSystem.u16BoardType != B_IMMB){
    	PWMDAC_INIT_MACRO(pwmdac1)
    }


// Initialize Hall module   
    hall1.DebounceAmount = 0;
    hall1.Revolutions = -3;
    if (gdsSystem.u16BoardType != B_IMMB) {
    	HALL3_INIT_MACRO_IMMB(hall1)
    } else if (gdsSystem.u16BoardType != B_DRV8301HC) {
    	HALL3_INIT_MACRO(hall1)
    } else {
    	HALL3_INIT_MACRO_DRV8301HC(hall1)
    }

// Initialize DATALOG module
    dlog.iptr1 = &DlogCh1;
    dlog.iptr2 = &DlogCh2;
    dlog.iptr3 = &DlogCh3;
    dlog.iptr4 = &DlogCh4;
    dlog.trig_value = 0x1;
    dlog.size = 0x0C8;
    //dlog.prescalar = 25;
    dlog.prescalar = 5;
    dlog.init(&dlog);

// Initialize ADC module (F2803XILEG_VDC.H)
    if (gdsSystem.u16BoardType == B_IMMB) {
    	ADC_MACRO_INIT_IMMB()	//
    } else if (gdsSystem.u16BoardType == B_DRV8301HC) {
    	ADC_MACRO_INIT_DRV8301HC()
    } else if (gdsSystem.u16BoardType == B_BOOST8301) {
    	ADC_MACRO_INIT_BOOST8301()
    } else {
    	ADC_MACRO_INIT_BOOST8305()
    }

// Initialize RMPCNTL module
    //rc1.RampDelayMax = 25;
	#ifndef DRV8301HC
    rc1.RampDelayMax = 20; //2;
	#else
    rc1.RampDelayMax = 2;
	#endif
    rc1.RampLowLimit = _IQ(0.0);
    rc1.RampHighLimit = _IQ(1.0);

// Initialize RMP2 module
	rmp2.Out = ALIGN_DUTY;
	rmp2.Ramp2Delay = 0x0050;
    rmp2.Ramp2Max = 0x7FFF;
    rmp2.Ramp2Min = 0x000F;

#if 0 // (BUILDLEVEL<= LEVEL2)
	rmp3.DesiredInput = CmtnPeriodTarget;
	rmp3.Ramp3Delay = RampDelay;
    rmp3.Out = CmtnPeriodSetpt;
    rmp3.Ramp3Min = 0x00000010;
#else
	RAMP_START_RATE = (PWM_FREQUENCY*1000)*60.0/BEGIN_START_RPM/COMMUTATES_PER_E_REV/(gu16Pole/2.0);
	RAMP_END_RATE = (PWM_FREQUENCY*1000)*60.0/END_START_RPM/COMMUTATES_PER_E_REV/(gu16Pole/2.0);
	CmtnPeriodTarget = RAMP_END_RATE; //1000
	CmtnPeriodSetpt = RAMP_START_RATE; //2000

	// Initialize RMP3 module
	rmp3.DesiredInput = CmtnPeriodTarget;
	rmp3.Ramp3Delay = RampDelay;
	//hrjung rmp3.Ramp3Delay = (Uint32)(((float32)gu16AccTime * 0.001)/((float32)(CmtnPeriodSetpt - CmtnPeriodTarget) * T));
	rmp3.Out = CmtnPeriodSetpt;
    rmp3.Ramp3Min = 0x00000010;
#endif

    // Initialize CMTN module (Commutation Trigger Module)
   	cmtn1.NWDelayThres = 20;
   	cmtn1.NWDelta = 2;
    cmtn1.NoiseWindowMax = cmtn1.NWDelayThres - cmtn1.NWDelta;

// Initialize the PID_GRANDO_CONTROLLER module for dc-bus current
    //제어기를 직렬 접속해서  Kp는 1.2정도가 적당
    //pid1_idc.param.Kp = _IQ(3.176*giqBaseCurrent/BASE_VOLTAGE);	// BLDC-Sensored	(발진함)
    //pid1_idc.param.Kp = _IQ(0.25); 								// BLDC-Int			(조금 느리게 반응함)
    pid1_idc.param.Kr = _IQ(1.0);
    //제어기를 직렬 접속해서 시험해보니 Ki는 T/0.1정도가 적당함
	//pid1_idc.param.Ki = _IQ(T/0.0005);							// BLDC-Sensored	(발진함)
	//pid1_idc.param.Ki = _IQ(T/0.05);								// BLDC-Int			(조금 느리게 반응함)
	pid1_idc.param.Kd = _IQ(0/T);
    pid1_idc.param.Km = _IQ(1.0);
    pid1_idc.param.Umax = _IQ(0.95);
    pid1_idc.param.Umin = _IQ(0.0);

// Initialize the PID_GRANDO_CONTROLLER module for Speed 
    //pid1_spd.param.Kp = _IQ(0.75);
    pid1_spd.param.Kr = _IQ(1.0);
	//pid1_spd.param.Ki = _IQ(T/0.3);
	pid1_spd.param.Kd = _IQ(0/T);
    pid1_spd.param.Km = _IQ(1.0);
    pid1_spd.param.Umax = _IQ(0.95);
    pid1_spd.param.Umin = _IQ(0.0);
    
// Initialize the current offset calibration filter
	cal_filt_gain = _IQ15(T/(T+TC_CAL));

// Enable global Interrupts and higher priority real-time debug events:
	EINT;   // Enable Global interrupt INTM
	ERTM;	// Enable Global realtime interrupt DBGM

// IDLE loop. Just sit and loop forever:
	UART_buf_init ();
	GUI_InitComm ();
    SoftTimer_InitVariable();
	CONSOLE_Initialize();
	CONSOLE_Logo();
	CONSOLE_Prompt();


	for(;;)  //infinite loop
	{
		BackTicker++;

		// State machine entry & exit point
		//===========================================================
		(*Alpha_State_Ptr)();	// jump to an Alpha state (A0,B0,...)
		//===========================================================

		// SerialInterface
		ProcessSerialInterface();

	}
} //END MAIN CODE

#define TC1DEFS_TIME_UNIT   1

//=================================================================================
//	STATE-MACHINE SEQUENCING AND SYNCRONIZATION FOR SLOW BACKGROUND TASKS
//=================================================================================

//--------------------------------- FRAMEWORK -------------------------------------
// 메인루프에서 호출한다. (1msec)
void A0(void)
{
	// loop rate synchronizer for A-tasks
	// 타이머가 셋트 되면 timer1을 사용해야 될것 같음.
	if(CpuTimer1Regs.TCR.bit.TIF == 1)
	{
		CpuTimer1Regs.TCR.bit.TIF = 1;	// clear flag

		//-----------------------------------------------------------
		(*A_Task_Ptr)();		// jump to an A Task (A1,A2,A3,...)
		//-----------------------------------------------------------

		VTimer0[0]++;			// virtual timer 0, instance 0 (spare)
		SerialCommsTimer++;

		// Software Timer
	    glSoftwareTimer += TC1DEFS_TIME_UNIT;

	    if( ghDateTime.u16Msec >= (1000-TC1DEFS_TIME_UNIT))
	    	ghDateTime.u16Msec  = 0;
	    else
	    	ghDateTime.u16Msec += TC1DEFS_TIME_UNIT;

	    /* 초단위 시간처리 */
	    if( STimer_IsElapsed( &ghSTIMER.SecondScheduleTimer ) )
	    {
	        STimer_Start( &ghSTIMER.SecondScheduleTimer, MACRO_SECONDS_STIMER(1) );

	        /* 분단위 시간처리 */
	        if( STimer_IsElapsed( &ghSTIMER.MinuteScheduleTimer ) )
	        {
	            STimer_Start( &ghSTIMER.MinuteScheduleTimer, MACRO_MINUTES_STIMER(1) );

	    		/* GUI모드의 콘솔복귀 처리  */
				if( STimer_IsActive( &ghSTIMER.SerialPortReInitTimer ) )
				{
					if( STimer_IsElapsed( &ghSTIMER.SerialPortReInitTimer ) )
					{
						STimer_Deactivate( &ghSTIMER.SerialPortReInitTimer );
					    scia_fifo_init();	   // Initialize the SCI FIFO
					    scia_echoback_init();
					}
				}

				/* 콘솔 디버그 모드 해제 처리   */
				if( (bool)STimer_IsActive(&ghSTIMER.DebugModeClearTimer) )
				{
					if( (bool) STimer_IsElapsed(&ghSTIMER.DebugModeClearTimer) )
					{
						STimer_Deactivate( &ghSTIMER.DebugModeClearTimer );
						CONSOLE_Initialize();
						ghCON.u8DeveloperMode = FALSE;
					}
				}

	            /* PC 인터페이스 모드 해제 처리 */
	            if( STimer_IsActive( &ghSTIMER.GuiInUseTimer ) )
	            {
	                if( STimer_IsElapsed( &ghSTIMER.GuiInUseTimer ) )
	                {
	                    ghCDF.u16GuiFlag    &= ~GUI_IN_USE;

	                    STimer_Deactivate( &ghSTIMER.GuiInUseTimer );

	                }
	            }

	        }
	    }
	}

	Alpha_State_Ptr = &B0;		// Comment out to allow only A tasks
}

// 5msec 타이머
void B0(void)
{
	// loop rate synchronizer for B-tasks
	if(CpuTimer2Regs.TCR.bit.TIF == 1)
	{
		CpuTimer2Regs.TCR.bit.TIF = 1;				// clear flag

		//-----------------------------------------------------------
		(*B_Task_Ptr)();		// jump to a B Task (B1,B2,B3,...)
		//-----------------------------------------------------------
		VTimer1[0]++;			// virtual timer 1, instance 0 (spare)
	}

	Alpha_State_Ptr = &A0;		// Allow C state tasks
}

//=================================================================================
//	A - TASKS (executed in every 1 msec)
//=================================================================================
//--------------------------------------------------------
void A1(void) // SPARE (not used)
//--------------------------------------------------------
{
	if (EnableFlag == false)
	{
		RunMotor = false;
		
		speed1.Speed     = _IQ(0.0);
		speed1.SpeedRpm  = 0;
		DCbus_current    = _IQ(0.0);

		EALLOW;
	 	EPwm1Regs.TZFRC.bit.OST=1;
		EPwm2Regs.TZFRC.bit.OST=1;
		EPwm3Regs.TZFRC.bit.OST=1;
	 	EDIS;
	}
	else if((EnableFlag == true) && (RunMotor == false))
	{
		if(DRV_RESET == 0)
		{
			#if defined(DRV830X)
			//assert the DRV830x EN_GATE pin
			#if (!F2806x_DEVICE_H)
			GpioDataRegs.GPBSET.bit.GPIO39 = 1;
			#else
			GpioDataRegs.GPBSET.bit.GPIO51 = 1;
			#endif

			DELAY_US(50000);		//delay to allow DRV830x supplies to ramp up

			if (gdsSystem.u16BoardType != B_BOOST8305) {
				DRV8301_cntrl_reg1.bit.GATE_CURRENT = 0;		// full current 1.7A
	//			DRV8301_cntrl_reg1.bit.GATE_CURRENT = 1;		// med current 0.7A
	//			DRV8301_cntrl_reg1.bit.GATE_CURRENT = 2;		// min current 0.25A
				DRV8301_cntrl_reg1.bit.GATE_RESET = 0;			// Normal Mode
				DRV8301_cntrl_reg1.bit.PWM_MODE = 0;			// six independant PWMs
	//			DRV8301_cntrl_reg1.bit.OC_MODE = 0;				// current limiting when OC detected
				DRV8301_cntrl_reg1.bit.OC_MODE = 1;				// latched OC shutdown
	//			DRV8301_cntrl_reg1.bit.OC_MODE = 2;				// Report on OCTWn pin and SPI reg only, no shut-down
	//			DRV8301_cntrl_reg1.bit.OC_MODE = 3;				// OC protection disabled
	//			DRV8301_cntrl_reg1.bit.OC_ADJ_SET = 0;			// OC @ Vds=0.060V
	//			DRV8301_cntrl_reg1.bit.OC_ADJ_SET = 4;			// OC @ Vds=0.097V
	//			DRV8301_cntrl_reg1.bit.OC_ADJ_SET = 6;			// OC @ Vds=0.123V
	//			DRV8301_cntrl_reg1.bit.OC_ADJ_SET = 9;			// OC @ Vds=0.175V
				DRV8301_cntrl_reg1.bit.OC_ADJ_SET = 15;			// OC @ Vds=0.358V
	//			DRV8301_cntrl_reg1.bit.OC_ADJ_SET = 16;			// OC @ Vds=0.403V
	//			DRV8301_cntrl_reg1.bit.OC_ADJ_SET = 17;			// OC @ Vds=0.454V
	//			DRV8301_cntrl_reg1.bit.OC_ADJ_SET = 18;			// OC @ Vds=0.511V
				DRV8301_cntrl_reg1.bit.Reserved = 0;

	//			DRV8301_cntrl_reg2.bit.OCTW_SET = 0;			// report OT and OC
				DRV8301_cntrl_reg2.bit.OCTW_SET = 1;			// report OT only
				#if DRV_GAIN == 10
				DRV8301_cntrl_reg2.bit.GAIN = 0;				// CS amplifier gain = 10
				#elif DRV_GAIN == 20
				DRV8301_cntrl_reg2.bit.GAIN = 1;				// CS amplifier gain = 20
				#elif DRV_GAIN == 40
				DRV8301_cntrl_reg2.bit.GAIN = 2;				// CS amplifier gain = 40
				#elif DRV_GAIN == 80
				DRV8301_cntrl_reg2.bit.GAIN = 3;				// CS amplifier gain = 80
				#endif
				DRV8301_cntrl_reg2.bit.DC_CAL_CH1 = 0;			// not in CS calibrate mode
				DRV8301_cntrl_reg2.bit.DC_CAL_CH2 = 0;			// not in CS calibrate mode
				DRV8301_cntrl_reg2.bit.OC_TOFF = 0;				// normal mode
				DRV8301_cntrl_reg2.bit.Reserved = 0;

				#if (!F2806x_DEVICE_H)
				//write to DRV8301 control register 1, returns status register 1
				DRV8301_stat_reg1.all = DRV830X_SPI_Write(&SpibRegs,CNTRL_REG_1_ADDR,DRV8301_cntrl_reg1.all);
				//write to DRV8301 control register 2, returns status register 1
				DRV8301_stat_reg1.all = DRV830X_SPI_Write(&SpibRegs,CNTRL_REG_2_ADDR,DRV8301_cntrl_reg2.all);
				#else
				//write to DRV8301 control register 1, returns status register 1
				//DRV8301_stat_reg1.all = DRV830X_SPI_Write(&SpiaRegs,CNTRL_REG_1_ADDR,DRV8301_cntrl_reg1.all);
				//write to DRV8301 control register 2, returns status register 1
				//DRV8301_stat_reg1.all = DRV830X_SPI_Write(&SpiaRegs,CNTRL_REG_2_ADDR,DRV8301_cntrl_reg2.all);
				#endif	// F2806x
			} else {

				// ic operation register setting
				DRV8305_ic_oper_reg.all = 0;				// all bit reset
				DRV8305_ic_oper_reg.bit.WD_DLY = 1;			// WD_DLY = 20msec
				DRV8305_ic_oper_reg.bit.CLR_FLTS = 1;		// Fault Clear

				//write to DRV8301 control register 1, returns status register 1
				DRV8301_stat_reg1.all = DRV830X_SPI_Write(&SpiaRegs,IC_OPERATION_ADDR,DRV8305_ic_oper_reg.all);
			}
			#endif	// DRV830X
		} // DRV_RESET == 0

		hall1.CmtnTrigHall = 0;
    	hall1.CapCounter = 0;
    	hall1.DebounceCount = 0;
    	hall1.DebounceAmount = 0;
    	hall1.HallGpio = 0;
    	hall1.HallGpioBuffer = 0;
    	hall1.HallGpioAccepted = 0;
    	hall1.EdgeDebounced = 0;
    	hall1.HallMap[0] = 0;
    	hall1.HallMap[1] = 0;
    	hall1.HallMap[2] = 0;
    	hall1.HallMap[3] = 0;
    	hall1.HallMap[4] = 0;
    	hall1.HallMap[5] = 0;
    	hall1.CapFlag = 0;
    	hall1.StallCount = 0xFFFF;
    	hall1.HallMapPointer = 0;
    	hall1.Revolutions = -3;

    	if (!SensorlessFlag) {
			speed1.InputSelect = 0;
    	} else {
			speed1.InputSelect = 1;
    	}
		speed1.NewTimeStamp = 0;
		speed1.OldTimeStamp = 0;
		speed1.EventPeriod = 0;
		speed1.Speed = 0;
		
		rc1.EqualFlag = 0;
		rc1.RampDelayCount = 0;
		rc1.SetpointValue = 0;
		rc1.TargetValue = 0;
		
		rmp2.DesiredInput = 0;
		rmp2.Ramp2DelayCount = 0;
		rmp2.Out = ALIGN_DUTY;
		
		if (!SensorlessFlag) {
//			CmtnPeriodTarget = 0x00000350;
//			CmtnPeriodSetpt = 0x00000500;
			CmtnPeriodTarget = RAMP_END_RATE;
			CmtnPeriodSetpt = RAMP_START_RATE;
		} else {
			CmtnPeriodTarget = 168;
			CmtnPeriodSetpt = 1024;
		}
		rmp3.DesiredInput = CmtnPeriodTarget;
		rmp3.Ramp3Delay = (Uint32)(((float32)gu16AccTime * 0.001)/((float32)(CmtnPeriodSetpt - CmtnPeriodTarget) * T));
    	rmp3.Out = CmtnPeriodSetpt;
		rmp3.Ramp3DelayCount = 0;
		rmp3.Ramp3DoneFlag = 0; 
		
		impl1.Counter = 0;
		impl1.Out = 0;
		
		mod1.Counter = 0;
		
		cmtn1.NWDelayThres = 20;
		cmtn1.NWDelta = 2;
   		cmtn1.NoiseWindowMax = cmtn1.NWDelayThres - cmtn1.NWDelta;
   		cmtn1.CmtnDelay = 0;
   		cmtn1.CmtnDelayCounter = 0;
   		cmtn1.CmtnPointer = 0;
   		cmtn1.CmtnTrig = 0;
   		cmtn1.DebugBemf = 0;
   		cmtn1.Delay30DoneFlag = 0;
   		cmtn1.DelayTaskPointer = 1;
   		cmtn1.Neutral = 0;
   		cmtn1.NewTimeStamp = 0;
   		cmtn1.NoiseWindowCounter = 0;
   		cmtn1.OldTimeStamp = 0;
   		cmtn1.RevPeriod = 100;
   		cmtn1.Va = 0;
   		cmtn1.Vb = 0;
   		cmtn1.Vc = 0;
   		cmtn1.VirtualTimer = 0;
   		cmtn1.ZcTrig = 0;

   		ClosedFlag = false;
		//ILoopFlag = false;
		SpeedLoopFlag = false;
		AlignFlag = 0x000F;
		LoopCount = 0;
		
		pid1_idc.data.d1 = 0;
		pid1_idc.data.d2 = 0;
		pid1_idc.data.i1 = 0;
		pid1_idc.data.ud = 0;
		pid1_idc.data.ui = 0;
		pid1_idc.data.up = 0;
		pid1_idc.data.v1 = 0;
		pid1_idc.data.w1 = 0;
		pid1_idc.term.Out = 0;

		pid1_spd.data.d1 = 0;
		pid1_spd.data.d2 = 0;
		pid1_spd.data.i1 = 0;
		pid1_spd.data.ud = 0;
		pid1_spd.data.ui = 0;
		pid1_spd.data.up = 0;
		pid1_spd.data.v1 = 0;
		pid1_spd.data.w1 = 0;
		pid1_spd.term.Out = 0;

		// BOOTS8305의 경우 처음 calibration하면 더 이상해짐.
		// 추후 필요 여부 확임 필요함
		#if(0)
		if (gdsSystem.u16BoardType != B_BOOST8305) {
			CALIBRATE_FLAG = 0;
		} else {
			CALIBRATE_FLAG = 1;
		}
		#else
		CALIBRATE_FLAG = 0;
		#endif

		RunMotor = true;
			
		EALLOW;
			EPwm1Regs.TZCLR.bit.OST=1;
			EPwm2Regs.TZCLR.bit.OST=1;
			EPwm3Regs.TZCLR.bit.OST=1;
		EDIS;
	}


	//-------------------
	//the next time CpuTimer0 'counter' reaches Period value go to A2
	A_Task_Ptr = &A2;
	//-------------------
}

//-----------------------------------------------------------------
void A2(void) // SoftTimer
//-----------------------------------------------------------------
{	

	// 설정업데이트
    UpdateParameters();

    // LED 출력 단자에 대한 처리
    ProcessLed();

	//-------------------
	//the next time CpuTimer0 'counter' reaches Period value go to A3
	A_Task_Ptr = &A3;
	//-------------------
}

//-----------------------------------------
void A3(void) // SPARE (not used)
//-----------------------------------------
{
	// 3msec 주시로 실행됨
    // 제어 단자 입력에 대한 처리
    ProcessInput();

    // 제어에 대한 처리
    ProcessControl();

    // 제어 출력 단자에 대한 처리
    ProcessOutput();

	//-----------------
	//the next time CpuTimer0 'counter' reaches Period value go to A1
	A_Task_Ptr = &A1;
	//-----------------
}



//=================================================================================
//	B - TASKS (executed in every 5 msec)
//=================================================================================

//----------------------------------- USER ----------------------------------------

//----------------------------------------
void B1(void) // Toggle GPIO-00
//----------------------------------------
{
	//GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B2
	B_Task_Ptr = &B2;	
	//-----------------
}

//----------------------------------------
void B2(void) //  SPARE
//----------------------------------------
{

	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B3
	B_Task_Ptr = &B3;
	//-----------------
}

//----------------------------------------
void B3(void) //  SPARE
//----------------------------------------
{

	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B1
	B_Task_Ptr = &B1;	
	//-----------------
}

// ==============================================================================
// =============================== MAIN ISR =====================================
// ==============================================================================


interrupt void MainISR(void)

{
// Verifying the ISR
    IsrTicker++;

    // Uart송신에 대한 처리
    UART_txprocess();
    UART_rxprocess();

if(RunMotor)
	{

	if(CALIBRATE_FLAG)
	{
// ------------------------------------------------------------------------------
//    ADC conversion and offset adjustment
// ------------------------------------------------------------------------------
		BemfA =  ((AdcResult.ADCRESULT4)*0.00024414)-BemfA_offset;
		BemfB =  ((AdcResult.ADCRESULT5)*0.00024414)-BemfB_offset;
		BemfC =  ((AdcResult.ADCRESULT6)*0.00024414)-BemfC_offset;
	   	if (gdsSystem.u16BoardType == B_IMMB) {
			current[0]=((AdcResult.ADCRESULT0)*0.00024414)-IA_offset;
			current[1]=((AdcResult.ADCRESULT1)*0.00024414)-IB_offset;
			current[2]=((AdcResult.ADCRESULT2)*0.00024414)-IC_offset;
			DCbus_current=((AdcResult.ADCRESULT3)*0.00024414)-IDC_offset;
	   	} else if (gdsSystem.u16BoardType == B_DRV8301HC) {
				current[0]=((AdcResult.ADCRESULT0)*0.00024414)-IA_offset;
				current[1]=((AdcResult.ADCRESULT1)*0.00024414)-IB_offset;
				DCbus_current=((AdcResult.ADCRESULT2)*0.00024414)-IDC_offset;
	   	} else {
			current[0]=((AdcResult.ADCRESULT0)*0.00024414)-IA_offset;
			current[1]=((AdcResult.ADCRESULT1)*0.00024414)-IB_offset;
			current[2]=((AdcResult.ADCRESULT2)*0.00024414)-IC_offset;
		}
// ------------------------------------------------------------------------------
//  LPF to average the calibration offsets
//  Use the offsets calculated here to initialize BemfA_offset, BemfB_offset
//  and BemfC_offset so that they are used for the remaining build levels
// ------------------------------------------------------------------------------
    	BemfA_offset = cal_filt_gain * BemfA + BemfA_offset;
    	BemfB_offset = cal_filt_gain * BemfB + BemfB_offset;
    	BemfC_offset = cal_filt_gain * BemfC + BemfC_offset;
    	if (gdsSystem.u16BoardType == B_IMMB) {
			IA_offset = cal_filt_gain * current[0] + IA_offset;
			IB_offset = cal_filt_gain * current[1] + IB_offset;
			IC_offset = cal_filt_gain * current[2] + IC_offset;
			IDC_offset = cal_filt_gain * DCbus_current + IDC_offset;
    	} else if (gdsSystem.u16BoardType == B_DRV8301HC) {
				IA_offset = cal_filt_gain * current[0] + IA_offset;
				IB_offset = cal_filt_gain * current[1] + IB_offset;
				IDC_offset = cal_filt_gain * DCbus_current + IDC_offset;
		} else {
    		IA_offset = cal_filt_gain * current[0] + IA_offset;
    		IB_offset = cal_filt_gain * current[1] + IB_offset;
    		IC_offset = cal_filt_gain * current[2] + IC_offset;
		}

// ------------------------------------------------------------------------------
//  force all PWMs to 0% duty cycle
// ------------------------------------------------------------------------------
		PHASE_A_ON;
		PHASE_B_ON;
		PHASE_C_ON;

		EPwm1Regs.CMPA.half.CMPA=0;	// PWM 1A - PhaseA
		EPwm2Regs.CMPA.half.CMPA=0;	// PWM 2A - PhaseB
		EPwm3Regs.CMPA.half.CMPA=0;	// PWM 3A - PhaseC

 		CALIBRATE_FLAG++;
 		CALIBRATE_FLAG &= CALIBRATE_TIME;
 		if (!CALIBRATE_FLAG) {
 			// DC_CAL OFF
 		   	if (gdsSystem.u16BoardType != B_IMMB) {
 		   		GpioDataRegs.GPBDAT.bit.GPIO52 = 0;
 		   	}else if (gdsSystem.u16BoardType != B_DRV8301HC) {
 		   		GpioDataRegs.GPBDAT.bit.GPIO51 = 0;
 		   	} else {
 		   		GpioDataRegs.GPBDAT.bit.GPIO43 = 0;
 		   	}
 		}
	} else { // !CALIBRATE_FLAG

#ifdef BLDC_SENSORLESS
		// Initial Rotor Alignment Process
		if ((AlignFlag != 0) && (SensorlessFlag))
		{
		  mod1.Counter = 0;
		  pwm1.CmtnPointer = 0;
		  if (Direction == 1) {
			   BLDCPWM_MACRO(pwm1);
		  } else {
			   BLDCPWM_RVS_MACRO(pwm1);
		  }
		  if (VirtualTimer > 0x7FFE)
		  {
			if (LoopCount != LOOP_CNT_MAX)
			   LoopCount++;
			else
			 {
			   AlignFlag = 0;
			   VirtualTimer = 0;
			   VirtualTimer++;
			   VirtualTimer &= 0x00007FFF;
			 }
		  }
		  else
		  {
			 VirtualTimer++;
			 VirtualTimer &= 0x00007FFF;
		  }
		}
	   else
#endif
		{

	// =============================== LEVEL 6 ======================================
	//	  Level 6 verifies the closed speed loop and speed PI controller.
	// ==============================================================================

	#if (BUILDLEVEL==LEVEL7)

			// ------------------------------------------------------------------------------
			//    ADC conversion and offset adjustment (observing back-emfs is optional for this prj.)
			// ------------------------------------------------------------------------------
				#ifdef F2806x_DEVICE_H
		      if (gdsSystem.u16BoardType == B_IMMB) {
					  BemfA =  ((AdcResult.ADCRESULT4)*0.00024414)-BemfA_offset;
					  BemfB =  ((AdcResult.ADCRESULT5)*0.00024414)-BemfB_offset;
					  BemfC =  ((AdcResult.ADCRESULT6)*0.00024414)-BemfC_offset;
					  current[0] = - ((AdcResult.ADCRESULT0)*0.00024414-IA_offset)*giqCurrentScaler;
					  current[1] = - ((AdcResult.ADCRESULT1)*0.00024414-IB_offset)*giqCurrentScaler;
					  current[2] = - ((AdcResult.ADCRESULT2)*0.00024414-IC_offset)*giqCurrentScaler;
					  // Lowpass Filter 필터 (IIR필터, 40K샘플링 / 3K cut-frequency )
					  iqX[1] = iqX[0];
					  iqY[1] = iqY[0];
					  iqX[0] = - ((AdcResult.ADCRESULT3)*0.00024414-IDC_offset)*giqCurrentScaler;
					  iqY[0] = (iqB[0]*iqX[0] + iqB[1]*iqX[1] - iqA[1]*iqY[1]) * iqG;
					  DCbus_current = iqY[0];
			    } else if (gdsSystem.u16BoardType == B_DRV8301HC) {
					  BemfA =  ((AdcResult.ADCRESULT4)*0.00024414)-BemfA_offset;
					  BemfB =  ((AdcResult.ADCRESULT5)*0.00024414)-BemfB_offset;
					  BemfC =  ((AdcResult.ADCRESULT6)*0.00024414)-BemfC_offset;
					  current[0] = - ((AdcResult.ADCRESULT0)*0.00024414-IA_offset)*giqCurrentScaler;
					  current[1] = - ((AdcResult.ADCRESULT1)*0.00024414-IB_offset)*giqCurrentScaler;
					  // Lopass Filter 필터 (IIR필터, 40K샘플링 / 3K cut-frequency )
					  iqX[1] = iqX[0];
					  iqY[1] = iqY[0];
					  iqX[0] = - ((AdcResult.ADCRESULT2)*0.00024414-IDC_offset)*giqCurrentScaler;
					  iqY[0] = (iqB[0]*iqX[0] + iqB[1]*iqX[1] - iqA[1]*iqY[1]) * iqG;
					  DCbus_current = iqY[0];
		 	    } else {
					  BemfA =  ((AdcResult.ADCRESULT4)*0.00024414)-BemfA_offset;
					  BemfB =  ((AdcResult.ADCRESULT5)*0.00024414)-BemfB_offset;
					  BemfC =  ((AdcResult.ADCRESULT6)*0.00024414)-BemfC_offset;
					  current[0] = -((AdcResult.ADCRESULT0)*0.00024414-IA_offset)*giqCurrentScaler;
					  current[1] = -((AdcResult.ADCRESULT1)*0.00024414-IB_offset)*giqCurrentScaler;
					  current[2] = -((AdcResult.ADCRESULT2)*0.00024414-IC_offset)*giqCurrentScaler;
					  // Lopass Filter 필터 (IIR필터, 40K샘플링 / 3K cut-frequency )
					  iqX[1] = iqX[0];
					  iqY[1] = iqY[0];
					  iqX[0] = current[0] + current[1] + current[2];
					  iqY[0] = (iqB[0]*iqX[0] + iqB[1]*iqX[1] - iqA[1]*iqY[1]) * iqG;
					  DCbus_current = iqY[0];
				 }

				#if (CURRENT_AVG)
				 DCcurrent_Sum += DCbus_current;
				 ++DCcurrent_Cnt;
				#endif
				#endif

				#ifdef DSP2803x_DEVICE_H
				 BemfA =  _IQ15toIQ((AdcResult.ADCRESULT4<<3)-BemfA_offset);
				 BemfB =  _IQ15toIQ((AdcResult.ADCRESULT5<<3)-BemfB_offset);
				 BemfC =  _IQ15toIQ((AdcResult.ADCRESULT6<<3)-BemfC_offset);
				  // IIR 필터
				 iqX[1] = iqX[0];
				 iqX[0] = current[0] + current[1] + current[2];
				 iqY[1] = iqY[0];
				 iqY[0] = _IQmpy(iqB[0], iqX[0]) + _IQmpy(iqB[1],iqX[1]) - _IQmpy(iqA[1],iqY[1]);
				#endif

			// ------------------------------------------------------------------------------
			//   Connect inputs of the RMP module and call the Ramp control macro.
			// ------------------------------------------------------------------------------
				 if (SpeedLoopFlag) {
					  rc1.TargetValue = SpeedRef;
				 } else {
					  rc1.TargetValue = 0.3;
					  //rc1.RampDelayMax = 1;
				 }
				 RC_MACRO(rc1)

			// ------------------------------------------------------------------------------
			//   Connect inputs of the RMP3 module and call the Ramp control 3 macro.
			// ------------------------------------------------------------------------------
				 rmp3.DesiredInput = CmtnPeriodTarget;
				 rmp3.Ramp3Delay = RampDelay;
				 RC3_MACRO(rmp3)

			// ------------------------------------------------------------------------------
			//   Connect inputs of the IMPULSE module and call the Impulse macro.
			// ------------------------------------------------------------------------------
				  impl1.Period = rmp3.Out;
				  IMPULSE_MACRO(impl1)

				 if (!SensorlessFlag) {
				// ------------------------------------------------------------------------------
				//    Connect inputs of the HALL module and call the Hall sensor read macro.
				// ------------------------------------------------------------------------------
					  hall1.HallMapPointer = (int16)mod1.Counter;
					  HALL3_READ_MACRO(hall1)	// 내부에 하드웨어에 따라 다른 설정 옵션 있음

					  if (hall1.Revolutions>=0)
					  {
						 ClosedFlag=true;
						  if(ClosedLog_flag==0)
						  {
							  ClosedLog_flag = 1;
							  UART_printf("\n ClosedLog_flag set!");
						  }
					  }

					// ------------------------------------------------------------------------------
					//    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
					// ------------------------------------------------------------------------------
					  if (ClosedFlag==false) {
						  mod1.TrigInput = impl1.Out;
						  mod1.Counter = (int32)hall1.HallMapPointer;
					  } else {
						  mod1.TrigInput = (int32)hall1.CmtnTrigHall;
					  }

				 }
#ifdef BLDC_SENSORLESS
				 else {	// Sensorless

					// test
					hall1.HallMapPointer = (int16)mod1.Counter;
					HALL3_READ_MACRO(hall1)

				   // ------------------------------------------------------------------------------
				   //    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
				   // ------------------------------------------------------------------------------
					 // Switch from open-loop to closed-loop operation by Ramp3DoneFlag variable
					  if (rmp3.Ramp3DoneFlag == false) {
						 mod1.TrigInput = impl1.Out;        // open-loop operation
					  } else {
						 mod1.TrigInput = cmtn1.CmtnTrig;   // closed-loop operation
					  }
				 }
#endif

				 if (Direction == 1) {
					  MOD6CNT_MACRO(mod1)
				 } else {
					  MOD6CNT_RVS_MACRO(mod1)
				 }

			// ------------------------------------------------------------------------------
			//    Connect inputs of the RMP2 module and call the Ramp control 2 macro.
			// ------------------------------------------------------------------------------
				 rmp2.DesiredInput = DFuncDesired;
				 RC2_MACRO(rmp2)

		    // ------------------------------------------------------------------------------
		    //   Connect inputs of the PID_REG3 module and call the PID speed controller
		    //	 macro.
		    // ------------------------------------------------------------------------------
				 if ((gu16CtrlMode == C_CASCADE)||(gu16CtrlMode == C_SPEED)) {
					  pid1_spd.term.Ref = rc1.SetpointValue;
					  pid1_spd.term.Fbk = speed1.Speed;
					  PID_GR_MACRO(pid1_spd)
				 }

			  // ------------------------------------------------------------------------------
			  //    Connect inputs of the PID_REG3 module and call the PID current controller
			  //	  macro.
			  // ------------------------------------------------------------------------------
			  // Switch from fixed duty-cycle or controlled Speed duty-cycle by SpeedLoopFlag variable
			  if ((gu16CtrlMode == C_CASCADE)||(gu16CtrlMode == C_CURRENT)) {
				 if(SpeedLoopFlag == FALSE) {
					pid1_idc.term.Ref = CurrentStartup;
				 } else {
					  if (gu16CtrlMode==C_CASCADE) {
						  pid1_idc.term.Ref = pid1_spd.term.Out;
					  } else {
						  pid1_idc.term.Ref = CurrentSet;
					  }
				 }
				 //pid1_idc.term.Fbk = DCbus_current;
				#if (CURRENT_AVG)
				 pid1_idc.term.Fbk = DCcurrent_Fbk;
				#else
				 pid1_idc.term.Fbk = DCbus_current;
				#endif

				 PID_GR_MACRO(pid1_idc)
			  }

			// ------------------------------------------------------------------------------
			//    Set the speed closed loop flag once the speed is built up to a desired value.
			// ------------------------------------------------------------------------------
				 if ( rc1.EqualFlag == 0x7FFFFFFF ) {
					  SpeedLoopFlag = true;
					  if(SpeedLoopLog_flag==0)
					  {
						  SpeedLoopLog_flag = 1;
						  UART_printf("\n SpeedLoopLog_flag set, rc2=%d, rc1=%d fb=%d", rmp2.Out, (int16)_IQtoQ15(pid1_spd.term.Out), (int16)_IQtoQ15(speed1.Speed) );
					  }
				 }

			// ------------------------------------------------------------------------------
			//    Connect inputs of the PWM_DRV module and call the PWM signal generation
			//    update macro.
			// ------------------------------------------------------------------------------
			// Switch from fixed duty-cycle or controlled Speed duty-cycle by SpeedLoopFlag variable
				 if (SpeedLoopFlag == false){
					  pwm1.DutyFunc = rmp2.Out;                 // fixed duty-cycle
					  pid1_spd.data.ui=0;
					  pid1_spd.data.i1=0;
					  pid1_idc.data.ui=0;
					  pid1_idc.data.i1=0;
				 } else {
					  if ( gu16CtrlMode == C_SPEED ) {
					      pwm1.DutyFunc = (int16)_IQtoQ15(pid1_spd.term.Out);   // controlled Speed duty-cycle
					  } else if ((gu16CtrlMode == C_CASCADE)||(gu16CtrlMode == C_CURRENT))  {
						  pwm1.DutyFunc = (int16)_IQtoQ15(pid1_idc.term.Out);
					  }
				 }

				 if (!SensorlessFlag ) {
					  if (ClosedFlag==true) {
						  if (hall1.CmtnTrigHall==0x7FFF) {

							PreviousState = pwm1.CmtnPointer;
							  // Comment the following if-else-if statements in case of
							  // inverted Hall logics for commutation states.
							if (gdsSystem.u16MotorType == M_W750SM) {

							  if (hall1.HallGpioAccepted==5) //2 -> 5
								pwm1.CmtnPointer = 0;

							  else if (hall1.HallGpioAccepted==1) // 6 -> 1
								pwm1.CmtnPointer = 1;

							  else if (hall1.HallGpioAccepted==3) // 4 -> 3
								pwm1.CmtnPointer = 2;

							  else if (hall1.HallGpioAccepted==2) // 5 -> 2
								pwm1.CmtnPointer = 3;

							  else if (hall1.HallGpioAccepted==6) // 1 -> 6
								pwm1.CmtnPointer = 4;

							  else if (hall1.HallGpioAccepted==4) // 3 -> 4
								pwm1.CmtnPointer = 5;
							}
							else if (gdsSystem.u16MotorType == M_W78) {
							  if (hall1.HallGpioAccepted==5)
								pwm1.CmtnPointer = 0;

							  else if (hall1.HallGpioAccepted==1)
								pwm1.CmtnPointer = 1;

							  else if (hall1.HallGpioAccepted==3)
								pwm1.CmtnPointer = 2;

							  else if (hall1.HallGpioAccepted==2)
								pwm1.CmtnPointer = 3;

							  else if (hall1.HallGpioAccepted==6)
								pwm1.CmtnPointer = 4;

							  else if (hall1.HallGpioAccepted==4)
								pwm1.CmtnPointer = 5;
							} else if (gdsSystem.u16MotorType == M_W750) {

							  if (hall1.HallGpioAccepted==2)
								pwm1.CmtnPointer = 0;

							  else if (hall1.HallGpioAccepted==6)
								pwm1.CmtnPointer = 1;

							  else if (hall1.HallGpioAccepted==4)
								pwm1.CmtnPointer = 2;

							  else if (hall1.HallGpioAccepted==5)
								pwm1.CmtnPointer = 3;

							  else if (hall1.HallGpioAccepted==1)
								pwm1.CmtnPointer = 4;

							  else if (hall1.HallGpioAccepted==3)
								pwm1.CmtnPointer = 5;
						    }

							  /*

							  // Comment the following if-else-if statements in case of
							  // non-inverted Hall logics for commutation states.
							  if (hall1.HallGpioAccepted==2)
								pwm1.CmtnPointer = 0;

							  else if (hall1.HallGpioAccepted==6)
								pwm1.CmtnPointer = 1;

							  else if (hall1.HallGpioAccepted==4)
								pwm1.CmtnPointer = 2;

							  else if (hall1.HallGpioAccepted==5)
								pwm1.CmtnPointer = 3;

							  else if (hall1.HallGpioAccepted==1)
								pwm1.CmtnPointer = 4;

							  else if (hall1.HallGpioAccepted==3)
								pwm1.CmtnPointer = 5;
							  */

						  }    // Hall1.CmtnTrigHall == 0x7FFF
					  } else {  // ClosedFlag!=true
						  pwm1.CmtnPointer = (int16)mod1.Counter;
					  }
				 } else {
					  pwm1.CmtnPointer = (int16)mod1.Counter;

				 }

				 if (Direction == 1) {
					  BLDCPWM_MACRO(pwm1);
				 } else {
					  BLDCPWM_RVS_MACRO(pwm1);
				 }

			// ------------------------------------------------------------------------------
			//    Connect inputs of the SPEED_PR module and call the speed calculation macro.
			// ------------------------------------------------------------------------------
				 if (!SensorlessFlag ) {	// Sensored
					 if (Direction == 1) {
						 if ((pwm1.CmtnPointer==5)&&(PreviousState==4)&&(hall1.CmtnTrigHall==0x7FFF))
						 {
							 speed1.TimeStamp = VirtualTimer;
							 SPEED_PR_MACRO(speed1)
						 }
						#if (CURRENT_AVG)
						 if (DCcurrent_SumCnt == 10) {
							 DCcurrent_Fbk = DCcurrent_Sum / DCcurrent_Cnt;
							 DCcurrent_Sum = 0;
							 DCcurrent_Cnt = 0;
							 DCcurrent_SumCnt = 0;
						 } else {
							 ++DCcurrent_SumCnt;
						 }
						#endif
					 } else {
						 if ((pwm1.CmtnPointer==4)&&(PreviousState==5)&&(hall1.CmtnTrigHall==0x7FFF)) {
							 speed1.TimeStamp = VirtualTimer;
							 SPEED_PR_MACRO(speed1)
						 }
						#if (CURRENT_AVG)
						 if (DCcurrent_SumCnt == 10) {
							 DCcurrent_Fbk = DCcurrent_Sum / DCcurrent_Cnt;
							 DCcurrent_Sum = 0;
							 DCcurrent_Cnt = 0;
							 DCcurrent_SumCnt = 0;
						 } else {
							 ++DCcurrent_SumCnt;
						 }
						#endif
					 }
				 }
#ifdef BLDC_SENSORLESS
				 else {	// Sensorless
					if ((gdsSystem.u16BoardType != B_DRV8301HC) && (gdsSystem.u16BoardType != B_IMMB)){
						  cmtn1.Va = BemfA;
						  cmtn1.Vb = BemfB;
						  cmtn1.Vc = BemfC;
					} else {
						  cmtn1.Va = BemfA * 3;
						  cmtn1.Vb = BemfB * 3;
						  cmtn1.Vc = BemfC * 3;
					}

					cmtn1.CmtnPointer = mod1.Counter;
					cmtn1.VirtualTimer = VirtualTimer;
					CMTN_TRIG_MACRO(cmtn1)

					speed1.EventPeriod = cmtn1.RevPeriod;
					SPEED_PR_MACRO(speed1)

				 }
#endif
			// ------------------------------------------------------------------------------
			//    Connect inputs of the PWMDAC module
			// ------------------------------------------------------------------------------
				 PwmDacCh1 = (int16)(mod1.Counter * 4096.0L);
				 PwmDacCh2 = _IQtoQ15(BemfA);
				 PwmDacCh3 = _IQtoQ15(BemfB);
			//      PwmDacCh3 = _IQtoQ15(BemfC);

			// ------------------------------------------------------------------------------
			//    Connect inputs of the DATALOG module
			// ------------------------------------------------------------------------------

				 DlogCh1 = mod1.Counter; //pwm1.CmtnPointer;
				 DlogCh2 = _IQtoQ15(DCbus_current/5.);
			     //DlogCh2 = hall1.HallGpioAccepted;
				 //DlogCh3 = _IQtoQ15(BemfA);
				 DlogCh3 = _IQtoQ15(current[0]/5.);
				 DlogCh4 = _IQtoQ15(BemfA);


	#endif

	// =============================== LEVEL 1 ======================================
	//	This Level describes the steps for a minimum system check-out which confirms
	//	operation of system interrupts, some peripheral & target independent modules
	//	and one peripheral dependent module.
	// ==============================================================================

	#if (BUILDLEVEL==LEVEL1)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the RMP3 module and call the RAMP Control 3 macro.
	// ------------------------------------------------------------------------------
		  rmp3.DesiredInput = CmtnPeriodTarget;
		  rmp3.Ramp3Delay = RampDelay;
		  RC3_MACRO(rmp3)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the IMPULSE module and call the Impulse macro.
	// ------------------------------------------------------------------------------
		  impl1.Period = rmp3.Out;
		  IMPULSE_MACRO(impl1)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the MOD6 module and call the Mod 6 counter macro.
	// ------------------------------------------------------------------------------
		  mod1.TrigInput = impl1.Out;
		  MOD6CNT_MACRO(mod1)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWM_DRV module and call the PWM signal generation
	//	  update macro.
	// ------------------------------------------------------------------------------
		  pwm1.CmtnPointer = (int16)mod1.Counter;
		  pwm1.DutyFunc = DfuncTesting;
		  BLDCPWM_MACRO(pwm1)

	#endif // (BUILDLEVEL==LEVEL1)

	// ------------------------------------------------------------------------------
	//    Call the PWMDAC update macro.
	// ------------------------------------------------------------------------------
		if (gdsSystem.u16BoardType != B_IMMB){
			PWMDAC_MACRO(pwmdac1)
		}

	// ------------------------------------------------------------------------------
	//    Call the DATALOG update function.
	// ------------------------------------------------------------------------------
		dlog.update(&dlog);

	// ------------------------------------------------------------------------------
	//    Increase virtual timer and force 15 bit wrap around
	// ------------------------------------------------------------------------------
		VirtualTimer++;
		VirtualTimer &= 0x00007FFF;
		}//end else
	} // CALIBRATION
	} // end if(RunMotor)


#if (DSP2803x_DEVICE_H==1)||(F2806x_DEVICE_H==1)
/* Enable more interrupts from this timer
	EPwm1Regs.ETCLR.bit.INT = 1;

// Acknowledge interrupt to recieve more interrupts from PIE group 3
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
*/

// Acknowledge interrupt to recieve more interrupts from PIE group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

#endif

#if (DSP280x_DEVICE_H==1)
// Enable more interrupts from this timer
	EPwm1Regs.ETCLR.bit.INT = 1;

// Acknowledge interrupt to recieve more interrupts from PIE group 3
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
#endif




	// =============================== LEVEL 2 ======================================
	//	  Level 2 verifies the analog-to-digital conversion, offset compensation,
	//    open loop motor operation.
	// ==============================================================================

	#if (BUILDLEVEL==LEVEL2)

	// ------------------------------------------------------------------------------
	//    ADC conversion and offset adjustment (observing back-emfs is optinal for this prj.)
	// ------------------------------------------------------------------------------
		#ifdef F2806x_DEVICE_H
		  BemfA =  ((AdcResult.ADCRESULT4)*0.00024414)-BemfA_offset;
		  BemfB =  ((AdcResult.ADCRESULT5)*0.00024414)-BemfB_offset;
		  BemfC =  ((AdcResult.ADCRESULT6)*0.00024414)-BemfC_offset;
		  current[0] = - ((AdcResult.ADCRESULT0)*0.00024414-IA_offset)*giqCurrentScaler;
		  current[1] = - ((AdcResult.ADCRESULT1)*0.00024414-IB_offset)*giqCurrentScaler;
		  // Lopass Filter 필터 (IIR필터, 40K샘플링 / 3K cut-frequency )
		  iqX[1] = iqX[0];
		  iqY[1] = iqY[0];
		  iqX[0] = - ((AdcResult.ADCRESULT2)*0.00024414-IDC_offset)*giqCurrentScaler;
		  iqY[0] = (iqB[0]*iqX[0] + iqB[1]*iqX[1] - iqA[1]*iqY[1]) * iqG;
		  DCbus_current = iqY[0];

		#endif

		#ifdef DSP2803x_DEVICE_H
		  BemfA =  _IQ15toIQ((AdcResult.ADCRESULT4<<3)-BemfA_offset);
		  BemfB =  _IQ15toIQ((AdcResult.ADCRESULT5<<3)-BemfB_offset);
		  BemfC =  _IQ15toIQ((AdcResult.ADCRESULT6<<3)-BemfC_offset);
		  DCbus_current = _IQ15toIQ((AdcResult.ADCRESULT7<<3)-IDC_offset)<<1; //1.65V offset added on DRV8312 board.
		#endif

	// ------------------------------------------------------------------------------
	//    Connect inputs of the RMP3 module and call the Ramp control 3 macro.
	// ------------------------------------------------------------------------------
		  rmp3.DesiredInput = CmtnPeriodTarget;
		  rmp3.Ramp3Delay = RampDelay;
		  RC3_MACRO(rmp3)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the IMPULSE module and call the Impulse macro.
	// ------------------------------------------------------------------------------
		  impl1.Period = rmp3.Out;
		  IMPULSE_MACRO(impl1)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
	// ------------------------------------------------------------------------------
		  mod1.TrigInput = impl1.Out;
		  MOD6CNT_MACRO(mod1)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWM_DRV module and call the PWM signal generation
	//    update macro.
	// ------------------------------------------------------------------------------
		  pwm1.CmtnPointer = (int16)mod1.Counter;
		  pwm1.DutyFunc = DfuncTesting;
		  BLDCPWM_MACRO(pwm1)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWMDAC module
	// ------------------------------------------------------------------------------
		  PwmDacCh1 = _IQtoQ15(BemfA);
		  PwmDacCh2 = _IQtoQ15(BemfB);
		  PwmDacCh3 = _IQtoQ15(BemfC);
	//      PwmDacCh3 = (int16)(mod1.Counter * 4096.0L);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the DATALOG module
	// ------------------------------------------------------------------------------
		  DlogCh1 = (int16)mod1.Counter;
		  DlogCh2 = _IQtoQ15(BemfA);
		  DlogCh3 = _IQtoQ15(BemfB);
		  DlogCh4 = _IQtoQ15(BemfC);

	#endif // (BUILDLEVEL==LEVEL2)

	// =============================== LEVEL 3 ======================================
	//	  Level 3 auto-calculates the current sensor offset calibration
	// ==============================================================================

	#if (BUILDLEVEL==LEVEL3)

	_iq IDCfdbk;
	// ------------------------------------------------------------------------------
	//  Measure DC Bus current, subtract the offset and normalize from (-0.5,+0.5) to (-1,+1).
	// ------------------------------------------------------------------------------
		#ifdef F2806x_DEVICE_H
		current[0] = (AdcResult.ADCRESULT0)*0.00024414-IDC_offset;	//1.65V offset added on DRV8305 board.
		current[1] = (AdcResult.ADCRESULT1)*0.00024414-IDC_offset;	//1.65V offset added on DRV8305 board.
		current[2] = (AdcResult.ADCRESULT2)*0.00024414-IDC_offset;	//1.65V offset added on DRV8305 board.
		current_avg[0] += (current[0] - current_avg[0])/2;
		current_avg[1] += (current[1] - current_avg[1])/2;
		current_avg[2] += (current[2] - current_avg[2])/2;
		DCbus_current = current_avg[0]+current_avg[1]+current_avg[2];
		IDCfdbk = DCbus_current; // DC Bus curr.
		#endif													 	   // ((ADCmeas(q12)/2^12)-0.5)*2

		#ifdef DSP2803x_DEVICE_H
		IDCfdbk=_IQ15toIQ((AdcResult.ADCRESULT7<<3)-IDC_offset)<<1;
		#endif

	// ------------------------------------------------------------------------------
	//  LPF to average the calibration offsets
	//  Use the offsets calculated here to initialize IDC_offset
	//  so that they are used for the remaining build levels
	// ------------------------------------------------------------------------------
		IDC_offset = _IQ15mpy(cal_filt_gain,_IQtoIQ15(IDCfdbk)) + IDC_offset;

	// ------------------------------------------------------------------------------
	//  force all PWMs to 0% duty cycle
	// ------------------------------------------------------------------------------
		EPwm1Regs.CMPA.half.CMPA=0;	// PWM 1A - PhaseA
		EPwm2Regs.CMPA.half.CMPA=0;	// PWM 2A - PhaseB
		EPwm3Regs.CMPA.half.CMPA=0;	// PWM 3A - PhaseC

	#endif // (BUILDLEVEL==LEVEL3)

	// =============================== LEVEL 4 ======================================
	//	  Level 4 describes the closed-loop operation of sensored trapezoidal
	//	  drive of BLDC motor using Hall sensor.
	// ==============================================================================

	#if (BUILDLEVEL==LEVEL4)

	// ------------------------------------------------------------------------------
	//    ADC conversion and offset adjustment (observing back-emfs is optinal for this prj.)
	// ------------------------------------------------------------------------------
		#ifdef F2806x_DEVICE_H
		  if (gdsSystem.u16BoardType != B_DRV8301HC) {
			  BemfA =  ((AdcResult.ADCRESULT4)*0.00024414)-BemfA_offset;
			  BemfB =  ((AdcResult.ADCRESULT5)*0.00024414)-BemfB_offset;
			  BemfC =  ((AdcResult.ADCRESULT6)*0.00024414)-BemfC_offset;
			  current[0] = -((AdcResult.ADCRESULT0)*0.00024414-IA_offset)*iqCurrentScaler;
			  current[1] = -((AdcResult.ADCRESULT1)*0.00024414-IB_offset)*iqCurrentScaler;
			  current[2] = -((AdcResult.ADCRESULT2)*0.00024414-IC_offset)*iqCurrentScaler;
			  // Lopass Filter 필터 (IIR필터, 40K샘플링 / 3K cut-frequency )
			  iqX[1] = iqX[0];
			  iqY[1] = iqY[0];
			  iqX[0] = current[0] + current[1] + current[2];
			  iqY[0] = (iqB[0]*iqX[0] + iqB[1]*iqX[1] - iqA[1]*iqY[1]) * iqG;
			  DCbus_current = iqY[0]*3.3;

		  } else {
			  BemfA =  ((AdcResult.ADCRESULT4)*0.00024414)-BemfA_offset;
			  BemfB =  ((AdcResult.ADCRESULT5)*0.00024414)-BemfB_offset;
			  BemfC =  ((AdcResult.ADCRESULT6)*0.00024414)-BemfC_offset;
			  current[0] = ((AdcResult.ADCRESULT0)*0.00024414-IA_offset)*iqCurrentScaler;
			  current[1] = ((AdcResult.ADCRESULT1)*0.00024414-IB_offset)*iqCurrentScaler;
			  // Lopass Filter 필터 (IIR필터, 40K샘플링 / 3K cut-frequency )
			  iqX[1] = iqX[0];
			  iqY[1] = iqY[0];
			  iqX[0] = ((AdcResult.ADCRESULT2)*0.00024414-IDC_offset)*iqCurrentScaler;
			  iqY[0] = (iqB[0]*iqX[0] + iqB[1]*iqX[1] - iqA[1]*iqY[1]) * iqG;
			  DCbus_current = -iqY[0];
		  }
		#endif

		#ifdef DSP2803x_DEVICE_H
		  BemfA =  _IQ15toIQ((AdcResult.ADCRESULT4<<3)-BemfA_offset);
		  BemfB =  _IQ15toIQ((AdcResult.ADCRESULT5<<3)-BemfB_offset);
		  BemfC =  _IQ15toIQ((AdcResult.ADCRESULT6<<3)-BemfC_offset);
		  DCbus_current = _IQ15toIQ((AdcResult.ADCRESULT7<<3)-IDC_offset)<<1; //1.65V offset added on DRV8312 board.
		#endif

	// ------------------------------------------------------------------------------
	//    Connect inputs of the RMP3 module and call the Ramp control 3 macro.
	// ------------------------------------------------------------------------------
		  rmp3.DesiredInput = CmtnPeriodTarget;
		  rmp3.Ramp3Delay = RampDelay;
		  RC3_MACRO(rmp3)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the IMPULSE module and call the Impulse macro.
	// ------------------------------------------------------------------------------
		  impl1.Period = rmp3.Out;
		  IMPULSE_MACRO(impl1)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the HALL module and call the Hall sensor read macro.
	// ------------------------------------------------------------------------------
		  hall1.HallMapPointer = (int16)mod1.Counter;
		  HALL3_READ_MACRO(hall1)
		  if (hall1.Revolutions>=0)
			ClosedFlag=true;

	// ------------------------------------------------------------------------------
	//    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
	// ------------------------------------------------------------------------------
	   if (ClosedFlag==false)
	   {
		  mod1.TrigInput = impl1.Out;
		  mod1.Counter = (int32)hall1.HallMapPointer;
	   }
	   else
		  mod1.TrigInput = (int32)hall1.CmtnTrigHall;

		 MOD6CNT_MACRO(mod1)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the RMP2 module and call the Ramp control 2 macro.
	// ------------------------------------------------------------------------------
		  rmp2.DesiredInput = DFuncDesired;
		  RC2_MACRO(rmp2)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWM_DRV module and call the PWM signal generation
	//    update macro.
	// ------------------------------------------------------------------------------
	   if (ClosedFlag==true)  {
		 if (hall1.CmtnTrigHall==0x7FFF) {

		  PreviousState = pwm1.CmtnPointer;



	// Comment the following if-else-if statements in case of
	// inverted Hall logics for commutation states.
		if (gdsSystem.u16MotorType == M_W78) {
		  if (hall1.HallGpioAccepted==5)
			pwm1.CmtnPointer = 0;

		  else if (hall1.HallGpioAccepted==1)
			pwm1.CmtnPointer = 1;

		  else if (hall1.HallGpioAccepted==3)
			pwm1.CmtnPointer = 2;

		  else if (hall1.HallGpioAccepted==2)
			pwm1.CmtnPointer = 3;

		  else if (hall1.HallGpioAccepted==6)
			pwm1.CmtnPointer = 4;

		  else if (hall1.HallGpioAccepted==4)
			pwm1.CmtnPointer = 5;
		} else if (gdsSystem.u16MotorType == M_W750) {

		  if (hall1.HallGpioAccepted==2)
			pwm1.CmtnPointer = 0;

		  else if (hall1.HallGpioAccepted==6)
			pwm1.CmtnPointer = 1;

		  else if (hall1.HallGpioAccepted==4)
			pwm1.CmtnPointer = 2;

		  else if (hall1.HallGpioAccepted==5)
			pwm1.CmtnPointer = 3;

		  else if (hall1.HallGpioAccepted==1)
			pwm1.CmtnPointer = 4;

		  else if (hall1.HallGpioAccepted==3)
			pwm1.CmtnPointer = 5;
	  }

	/*
	// Comment the following if-else-if statements in case of
	// non-inverted Hall logics for commutation states.
		  if (hall1.HallGpioAccepted==2)
			pwm1.CmtnPointer = 0;

		  else if (hall1.HallGpioAccepted==6)
			pwm1.CmtnPointer = 1;

		  else if (hall1.HallGpioAccepted==4)
			pwm1.CmtnPointer = 2;

		  else if (hall1.HallGpioAccepted==5)
			pwm1.CmtnPointer = 3;

		  else if (hall1.HallGpioAccepted==1)
			pwm1.CmtnPointer = 4;

		  else if (hall1.HallGpioAccepted==3)
			pwm1.CmtnPointer = 5;
	*/

		}    //hall1.CmtnTrigHall == 0x7FFF
	  } // ClosedFlag==true
	  else
		  pwm1.CmtnPointer = (int16)mod1.Counter;

		pwm1.DutyFunc = rmp2.Out;
		BLDCPWM_MACRO(pwm1)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the SPEED_PR module and call the speed calculation macro.
	// ------------------------------------------------------------------------------
	  if ((pwm1.CmtnPointer==5)&&(PreviousState==4)&&(hall1.CmtnTrigHall==0x7FFF))
	  {
		   speed1.TimeStamp = VirtualTimer;
		   SPEED_PR_MACRO(speed1);
	  }

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWMDAC module
	// ------------------------------------------------------------------------------
		  PwmDacCh1 = (int16)(mod1.Counter * 4096.0L);
		  PwmDacCh2 = (int16)(hall1.HallGpioAccepted * 4096.0L);
		  PwmDacCh3 = _IQtoQ15(BemfA);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the DATALOG module
	// ------------------------------------------------------------------------------
		  DlogCh1 = (int16)mod1.Counter;
		  DlogCh2 = hall1.HallGpioAccepted;
		  DlogCh3 = _IQtoQ15(BemfA);
		  DlogCh4 = _IQtoQ15(BemfB);


	#endif // (BUILDLEVEL==LEVEL4)

	// =============================== LEVEL 5 ======================================
	//	  Level 5 verifies the closed current loop and current PI controller.
	// ==============================================================================

	#if (BUILDLEVEL==LEVEL5)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the RMP3 module and call the Ramp control 3 macro.
	// ------------------------------------------------------------------------------
		  rmp3.DesiredInput = CmtnPeriodTarget;
		  rmp3.Ramp3Delay = RampDelay;
		  RC3_MACRO(rmp3)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the IMPULSE module and call the Impulse macro.
	// ------------------------------------------------------------------------------
		  impl1.Period = rmp3.Out;
		  IMPULSE_MACRO(impl1)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the HALL module and call the Hall sensor read macro.
	// ------------------------------------------------------------------------------
		  hall1.HallMapPointer = (int16)mod1.Counter;
		  HALL3_READ_MACRO(hall1)

		  if (hall1.Revolutions>=0)
			 ClosedFlag=true;

	// ------------------------------------------------------------------------------
	//    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
	// ------------------------------------------------------------------------------
		   if (ClosedFlag==false) {
			  mod1.TrigInput = impl1.Out;
			  mod1.Counter = (int32)hall1.HallMapPointer;
		   }
		   else
			  mod1.TrigInput = (int32)hall1.CmtnTrigHall;

		   MOD6CNT_MACRO(mod1)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the RMP2 module and call the Ramp control 2 macro.
	// ------------------------------------------------------------------------------
		  rmp2.DesiredInput = DFuncDesired;
		  RC2_MACRO(rmp2)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PID_GRANDO_CONTROLLER module and call the PID controller macro.
	// ------------------------------------------------------------------------------
		  tempIdc=pid1_idc.term.Fbk;
		  pid1_idc.term.Ref = CurrentSet;
		#ifdef F2806x_DEVICE_H
		  if (gdsSystem.u16BoardType != B_DRV8301HC) {
			  BemfA =  ((AdcResult.ADCRESULT4)*0.00024414)-BemfA_offset;
			  BemfB =  ((AdcResult.ADCRESULT5)*0.00024414)-BemfB_offset;
			  BemfC =  ((AdcResult.ADCRESULT6)*0.00024414)-BemfC_offset;
			  if (gdsSystem.u16BoardType == B_BOOST8301) {
				  current[0] = -((AdcResult.ADCRESULT0)*0.00024414-IA_offset)*3.3;
				  current[1] = -((AdcResult.ADCRESULT1)*0.00024414-IB_offset)*3.3;
				  current[2] = -((AdcResult.ADCRESULT2)*0.00024414-IC_offset)*3.3;
			  }else {
				  current[0] = -((AdcResult.ADCRESULT0)*0.00024414-IA_offset)*1.428571429*3.3;
				  current[1] = -((AdcResult.ADCRESULT1)*0.00024414-IB_offset)*1.428571429*3.3;
				  current[2] = -((AdcResult.ADCRESULT2)*0.00024414-IC_offset)*1.428571429*3.3;
			  }
			  // Lopass Filter 필터 (IIR필터, 40K샘플링 / 3K cut-frequency )
			  iqX[1] = iqX[0];
			  iqY[1] = iqY[0];
			  iqX[0] = current[0] + current[1] + current[2];
			  iqY[0] = (iqB[0]*iqX[0] + iqB[1]*iqX[1] - iqA[1]*iqY[1]) * iqG;
			  DCbus_current = iqY[0]*3.3;

		  } else {
			  BemfA =  ((AdcResult.ADCRESULT4)*0.00024414)-BemfA_offset;
			  BemfB =  ((AdcResult.ADCRESULT5)*0.00024414)-BemfB_offset;
			  BemfC =  ((AdcResult.ADCRESULT6)*0.00024414)-BemfC_offset;
			  current[0] = ((AdcResult.ADCRESULT0)*0.00024414-IA_offset)*4.58;
			  current[1] = ((AdcResult.ADCRESULT1)*0.00024414-IB_offset)*4.58;
			  // Lopass Filter 필터 (IIR필터, 40K샘플링 / 3K cut-frequency )
			  iqX[1] = iqX[0];
			  iqY[1] = iqY[0];
			  iqX[0] = ((AdcResult.ADCRESULT2)*0.00024414-IDC_offset)*4.58;
			  iqY[0] = (iqB[0]*iqX[0] + iqB[1]*iqX[1] - iqA[1]*iqY[1]) * iqG;
			  DCbus_current = -iqY[0];
		  }
        pid1_idc.term.Fbk = DCbus_current; //1.65V offset added on DRV8312 board.
		#endif

		#ifdef DSP2803x_DEVICE_H
		  pid1_idc.term.Fbk = _IQ15toIQ((AdcResult.ADCRESULT7<<3)-IDC_offset)<<1; //1.65V offset added on DRV8312 board.
	//      pid1_idc.term.Fbk = -(_IQ15toIQ((AdcResult.ADCRESULT7<<3)-IDC_offset)<<1); //1.65V offset added on DRV8312 board.
		#endif

		  if(pid1_idc.term.Fbk<0) pid1_idc.term.Fbk=tempIdc; // Eliminate negative values
		  PID_GR_MACRO(pid1_idc)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWM_DRV module and call the PWM signal generation
	//    update macro.
	// ------------------------------------------------------------------------------
	// Switch from fixed duty-cycle or controlled Idc duty-cycle by ILoopFlag variable

		  if (ILoopFlag == false)
			{ pwm1.DutyFunc = rmp2.Out;                 // fixed duty-cycle
			 pid1_idc.data.ui=0;
			 pid1_idc.data.i1=0;
			}
		  else
			{
			 pwm1.DutyFunc = _IQtoQ15(pid1_idc.term.Out);   // controlled Idc duty-cycle
			}

		if (ClosedFlag==true)
		{
		 if (hall1.CmtnTrigHall==0x7FFF)
		 {

		  PreviousState = pwm1.CmtnPointer;


	// Comment the following if-else-if statements in case of
	// inverted Hall logics for commutation states. r
		if (gdsSystem.u16MotorType == M_W78) {
		  if (hall1.HallGpioAccepted==5)
			pwm1.CmtnPointer = 0;

		  else if (hall1.HallGpioAccepted==1)
			pwm1.CmtnPointer = 1;

		  else if (hall1.HallGpioAccepted==3)
			pwm1.CmtnPointer = 2;

		  else if (hall1.HallGpioAccepted==2)
			pwm1.CmtnPointer = 3;

		  else if (hall1.HallGpioAccepted==6)
			pwm1.CmtnPointer = 4;

		  else if (hall1.HallGpioAccepted==4)
			pwm1.CmtnPointer = 5;
		} else if (gdsSystem.u16MotorType == M_W750) {

		  if (hall1.HallGpioAccepted==2)
			pwm1.CmtnPointer = 0;

		  else if (hall1.HallGpioAccepted==6)
			pwm1.CmtnPointer = 1;

		  else if (hall1.HallGpioAccepted==4)
			pwm1.CmtnPointer = 2;

		  else if (hall1.HallGpioAccepted==5)
			pwm1.CmtnPointer = 3;

		  else if (hall1.HallGpioAccepted==1)
			pwm1.CmtnPointer = 4;

		  else if (hall1.HallGpioAccepted==3)
			pwm1.CmtnPointer = 5;
		}

	/*

	// Comment the following if-else-if statements in case of
	// non-inverted Hall logics for commutation states.

		  if (hall1.HallGpioAccepted==2)
			pwm1.CmtnPointer = 0;

		  else if (hall1.HallGpioAccepted==6)
			pwm1.CmtnPointer = 1;

		  else if (hall1.HallGpioAccepted==4)
			pwm1.CmtnPointer = 2;

		  else if (hall1.HallGpioAccepted==5)
			pwm1.CmtnPointer = 3;

		  else if (hall1.HallGpioAccepted==1)
			pwm1.CmtnPointer = 4;

		  else if (hall1.HallGpioAccepted==3)
			pwm1.CmtnPointer = 5;
	*/
		}    // Hall1.CmtnTrigHall == 0x7FFF
	  }      // ClosedFlag==true

	  else
			pwm1.CmtnPointer = (int16)mod1.Counter;

			BLDCPWM_MACRO(pwm1)
	// ------------------------------------------------------------------------------
	//    Connect inputs of the SPEED_PR module and call the speed calculation macro.
	// ------------------------------------------------------------------------------
		  if ((pwm1.CmtnPointer==5)&&(PreviousState==4)&&(hall1.CmtnTrigHall==0x7FFF))
		  {
			 speed1.TimeStamp = VirtualTimer;
			 SPEED_PR_MACRO(speed1)
		  }

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWMDAC module
	// ------------------------------------------------------------------------------
		  PwmDacCh1 = (int16)(mod1.Counter * 4096.0L);
		  PwmDacCh2 = (int16)(hall1.HallGpioAccepted * 4096.0L);
		  PwmDacCh3 = _IQtoQ15(speed1.Speed);
	//      PwmDacCh3 = _IQtoQ15(pid1_idc.term.Fbk);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the DATALOG module
	// ------------------------------------------------------------------------------
		  DlogCh1 = pwm1.CmtnPointer;
		  DlogCh2 = hall1.HallGpioAccepted;
		  DlogCh3 = _IQtoQ15(speed1.Speed);
		  DlogCh4 = _IQtoQ15(pid1_idc.term.Fbk);

	#endif // (BUILDLEVEL==LEVEL5)


	// =============================== LEVEL 6 ======================================
	//	  Level 6 verifies the closed speed loop and speed PI controller.
	// ==============================================================================

	#if (BUILDLEVEL==LEVEL6)

	// ------------------------------------------------------------------------------
	//    ADC conversion and offset adjustment (observing back-emfs is optinal for this prj.)
	// ------------------------------------------------------------------------------
		#ifdef F2806x_DEVICE_H
		  if (gdsSystem.u16BoardType != B_DRV8301HC) {
			  BemfA =  ((AdcResult.ADCRESULT4)*0.00024414)-BemfA_offset;
			  BemfB =  ((AdcResult.ADCRESULT5)*0.00024414)-BemfB_offset;
			  BemfC =  ((AdcResult.ADCRESULT6)*0.00024414)-BemfC_offset;
			  current[0] = -((AdcResult.ADCRESULT0)*0.00024414-IA_offset)*giqCurrentScaler;
			  current[1] = -((AdcResult.ADCRESULT1)*0.00024414-IB_offset)*giqCurrentScaler;
			  current[2] = -((AdcResult.ADCRESULT2)*0.00024414-IC_offset)*giqCurrentScaler;
			  // Lopass Filter 필터 (IIR필터, 40K샘플링 / 3K cut-frequency )
			  iqX[1] = iqX[0];
			  iqY[1] = iqY[0];
			  iqX[0] = current[0] + current[1] + current[2];
			  iqY[0] = (iqB[0]*iqX[0] + iqB[1]*iqX[1] - iqA[1]*iqY[1]) * iqG;
			  DCbus_current = iqY[0];

		  } else {
			  BemfA =  ((AdcResult.ADCRESULT4)*0.00024414)-BemfA_offset;
			  BemfB =  ((AdcResult.ADCRESULT5)*0.00024414)-BemfB_offset;
			  BemfC =  ((AdcResult.ADCRESULT6)*0.00024414)-BemfC_offset;
			  current[0] = - ((AdcResult.ADCRESULT0)*0.00024414-IA_offset)*giqCurrentScaler;
			  current[1] = - ((AdcResult.ADCRESULT1)*0.00024414-IB_offset)*giqCurrentScaler;
			  // Lopass Filter 필터 (IIR필터, 40K샘플링 / 3K cut-frequency )
			  iqX[1] = iqX[0];
			  iqY[1] = iqY[0];
			  iqX[0] = - ((AdcResult.ADCRESULT2)*0.00024414-IDC_offset)*giqCurrentScaler;
			  iqY[0] = (iqB[0]*iqX[0] + iqB[1]*iqX[1] - iqA[1]*iqY[1]) * iqG;
			  DCbus_current = iqY[0];
		  }
		#endif

		#ifdef DSP2803x_DEVICE_H
		  BemfA =  _IQ15toIQ((AdcResult.ADCRESULT4<<3)-BemfA_offset);
		  BemfB =  _IQ15toIQ((AdcResult.ADCRESULT5<<3)-BemfB_offset);
		  BemfC =  _IQ15toIQ((AdcResult.ADCRESULT6<<3)-BemfC_offset);
		  // IIR 필터
		  iqX[1] = iqX[0];
		  iqX[0] = current[0] + current[1] + current[2];
		  iqY[1] = iqY[0];
		  iqY[0] = _IQmpy(iqB[0], iqX[0]) + _IQmpy(iqB[1],iqX[1]) - _IQmpy(iqA[1],iqY[1]);
		#endif

	// ------------------------------------------------------------------------------
	//    Connect inputs of the RMP module and call the Ramp control macro.
	// ------------------------------------------------------------------------------
		  if (SpeedLoopFlag) {
			  rc1.TargetValue = SpeedRef;
		  } else {
			  rc1.TargetValue = 0.3;
			  //rc1.RampDelayMax = 1;
		  }
		  RC_MACRO(rc1)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the RMP3 module and call the Ramp control 3 macro.
	// ------------------------------------------------------------------------------
		  rmp3.DesiredInput = CmtnPeriodTarget;
		  rmp3.Ramp3Delay = RampDelay;
		  RC3_MACRO(rmp3)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the IMPULSE module and call the Impulse macro.
	// ------------------------------------------------------------------------------
		  impl1.Period = rmp3.Out;
		  IMPULSE_MACRO(impl1)

		  if (!SensorlessFlag) {
		// ------------------------------------------------------------------------------
		//    Connect inputs of the HALL module and call the Hall sensor read macro.
		// ------------------------------------------------------------------------------
			  hall1.HallMapPointer = (int16)mod1.Counter;
			  HALL3_READ_MACRO(hall1)	// 내부에 하드웨어에 따라 다른 설정 옵션 있음

			  if (hall1.Revolutions>=0)
				 ClosedFlag=true;

			// ------------------------------------------------------------------------------
			//    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
			// ------------------------------------------------------------------------------
			  if (ClosedFlag==false) {
				  mod1.TrigInput = impl1.Out;
				  mod1.Counter = (int32)hall1.HallMapPointer;
			  } else {
				  mod1.TrigInput = (int32)hall1.CmtnTrigHall;
			  }

		   } else {	// Sensorless

			// test
			hall1.HallMapPointer = (int16)mod1.Counter;
			HALL3_READ_MACRO(hall1)

		   // ------------------------------------------------------------------------------
		   //    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
		   // ------------------------------------------------------------------------------
			 // Switch from open-loop to closed-loop operation by Ramp3DoneFlag variable
			  if (rmp3.Ramp3DoneFlag == false) {
				 mod1.TrigInput = impl1.Out;        // open-loop operation
			  } else {
				 mod1.TrigInput = cmtn1.CmtnTrig;   // closed-loop operation
			  }
		  }

		  if (Direction == 1) {
			  MOD6CNT_MACRO(mod1)
		  } else {
			  MOD6CNT_RVS_MACRO(mod1)
		  }

	// ------------------------------------------------------------------------------
	//    Connect inputs of the RMP2 module and call the Ramp control 2 macro.
	// ------------------------------------------------------------------------------
		  rmp2.DesiredInput = DFuncDesired;
		  RC2_MACRO(rmp2)

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PID_REG3 module and call the PID speed controller
	//	  macro.
	// ------------------------------------------------------------------------------
		  pid1_spd.term.Ref = rc1.SetpointValue;
		  pid1_spd.term.Fbk = speed1.Speed;
		  PID_GR_MACRO(pid1_spd)

	// ------------------------------------------------------------------------------
	//    Set the speed closed loop flag once the speed is built up to a desired value.
	// ------------------------------------------------------------------------------
		  if ( rc1.EqualFlag == 0x7FFFFFFF ) {
			  SpeedLoopFlag = true;
		  }

	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWM_DRV module and call the PWM signal generation
	//    update macro.
	// ------------------------------------------------------------------------------
	// Switch from fixed duty-cycle or controlled Speed duty-cycle by SpeedLoopFlag variable
		  if (SpeedLoopFlag == false){
			  pwm1.DutyFunc = rmp2.Out;                 // fixed duty-cycle
			  pid1_spd.data.ui=0;
			  pid1_spd.data.i1=0;
		  } else {
			  pwm1.DutyFunc = (int16)_IQtoQ15(pid1_spd.term.Out);   // controlled Speed duty-cycle
		  }

		  if (!SensorlessFlag ) {
			  if (ClosedFlag==true) {
				  if (hall1.CmtnTrigHall==0x7FFF) {

					  PreviousState = pwm1.CmtnPointer;

					  // Comment the following if-else-if statements in case of
					  // inverted Hall logics for commutation states.
					if (gdsSystem.u16MotorType == M_W78) {
					  if (hall1.HallGpioAccepted==5)
						pwm1.CmtnPointer = 0;

					  else if (hall1.HallGpioAccepted==1)
						pwm1.CmtnPointer = 1;

					  else if (hall1.HallGpioAccepted==3)
						pwm1.CmtnPointer = 2;

					  else if (hall1.HallGpioAccepted==2)
						pwm1.CmtnPointer = 3;

					  else if (hall1.HallGpioAccepted==6)
						pwm1.CmtnPointer = 4;

					  else if (hall1.HallGpioAccepted==4)
						pwm1.CmtnPointer = 5;
					} else if (gdsSystem.u16MotorType == M_W750) {

					  if (hall1.HallGpioAccepted==2)
						pwm1.CmtnPointer = 0;

					  else if (hall1.HallGpioAccepted==6)
						pwm1.CmtnPointer = 1;

					  else if (hall1.HallGpioAccepted==4)
						pwm1.CmtnPointer = 2;

					  else if (hall1.HallGpioAccepted==5)
						pwm1.CmtnPointer = 3;

					  else if (hall1.HallGpioAccepted==1)
						pwm1.CmtnPointer = 4;

					  else if (hall1.HallGpioAccepted==3)
						pwm1.CmtnPointer = 5;
				  }

					if (gdsSystem.u16MotorType != B_DRV8301HC) {
					  //DCbus_current = current_avg[pwm1.CmtnPointer>>1];
					}

					  /*

					  // Comment the following if-else-if statements in case of
					  // non-inverted Hall logics for commutation states.
					  if (hall1.HallGpioAccepted==2)
						pwm1.CmtnPointer = 0;

					  else if (hall1.HallGpioAccepted==6)
						pwm1.CmtnPointer = 1;

					  else if (hall1.HallGpioAccepted==4)
						pwm1.CmtnPointer = 2;

					  else if (hall1.HallGpioAccepted==5)
						pwm1.CmtnPointer = 3;

					  else if (hall1.HallGpioAccepted==1)
						pwm1.CmtnPointer = 4;

					  else if (hall1.HallGpioAccepted==3)
						pwm1.CmtnPointer = 5;
					  */

				  }    // Hall1.CmtnTrigHall == 0x7FFF
			  } else {  // ClosedFlag==true
				  pwm1.CmtnPointer = (int16)mod1.Counter;
			  }
		  } else {
			  pwm1.CmtnPointer = (int16)mod1.Counter;

				if (gdsSystem.u16MotorType != B_DRV8301HC) {
					//DCbus_current = current_avg[pwm1.CmtnPointer>>1];
				}
		}

		 if (Direction == 1) {
			  BLDCPWM_MACRO(pwm1);
		 } else {
			  BLDCPWM_RVS_MACRO(pwm1);
		 }

	// ------------------------------------------------------------------------------
	//    Connect inputs of the SPEED_PR module and call the speed calculation macro.
	// ------------------------------------------------------------------------------
		 if (!SensorlessFlag ) {	// Sensored
			 if (Direction == 1) {
				 if ((pwm1.CmtnPointer==5)&&(PreviousState==4)&&(hall1.CmtnTrigHall==0x7FFF))
				 {
					 speed1.TimeStamp = VirtualTimer;
					 SPEED_PR_MACRO(speed1)
				 }
			 } else {
				 if ((pwm1.CmtnPointer==4)&&(PreviousState==5)&&(hall1.CmtnTrigHall==0x7FFF))
				 {
					 speed1.TimeStamp = VirtualTimer;
					 SPEED_PR_MACRO(speed1)
				 }
			 }
		 } else {	// Sensorless
			if (gdsSystem.u16BoardType != B_DRV8301HC) {
				  cmtn1.Va = BemfA;
				  cmtn1.Vb = BemfB;
				  cmtn1.Vc = BemfC;
			} else {
				  cmtn1.Va = BemfA * 3;
				  cmtn1.Vb = BemfB * 3;
				  cmtn1.Vc = BemfC * 3;
			}

			  cmtn1.CmtnPointer = mod1.Counter;
			  cmtn1.VirtualTimer = VirtualTimer;
			  CMTN_TRIG_MACRO(cmtn1)

			 speed1.EventPeriod = cmtn1.RevPeriod;
			 SPEED_PR_MACRO(speed1)
		 }
	// ------------------------------------------------------------------------------
	//    Connect inputs of the PWMDAC module
	// ------------------------------------------------------------------------------
		  PwmDacCh1 = (int16)(mod1.Counter * 4096.0L);
		  PwmDacCh2 = _IQtoQ15(BemfA);
		  PwmDacCh3 = _IQtoQ15(BemfB);
	//      PwmDacCh3 = _IQtoQ15(BemfC);

	// ------------------------------------------------------------------------------
	//    Connect inputs of the DATALOG module
	// ------------------------------------------------------------------------------

		  DlogCh1 = pwm1.CmtnPointer;
		  DlogCh2 = _IQtoQ15(DCbus_current/5.);
		  //DlogCh2 = hall1.HallGpioAccepted;
		  //DlogCh3 = _IQtoQ15(BemfA);
		  DlogCh3 = _IQtoQ15(current[0]/5.);
		  DlogCh4 = _IQtoQ15(BemfA);


	#endif // (BUILDLEVEL==LEVEL6)


}// ISR Ends Here


//===========================================================================
// No more.
//===========================================================================
