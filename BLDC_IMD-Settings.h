/* =================================================================================
File name:  BLDC_Sensored-Settings.H                     
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: 
Incremental Build Level control file.
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 02-13-2011	Version 1.0
=================================================================================  */
#ifndef PROJ_SETTINGS_H

/*------------------------------------------------------------------------------
Hardware Configuration.
------------------------------------------------------------------------------*/

enum { B_BOOST8301, B_BOOST8305, B_DRV8301HC, B_IMMB};
enum { M_W78, M_W750 };
enum { C_CURRENT, C_SPEED, C_CASCADE, C_POSITION };

typedef struct {
	Uint16 u16BoardType;
	Uint16 u16MotorType;
} dsSystem;

//#define BOOST8301
//#define BOOST8305
//#define DRV8301HC
#define IMMB

//#define W78
#define W750

#define DRV830X
#define DRV_GAIN 10

//
/*------------------------------------------------------------------------------
Following is the list of the Build Level choices.
------------------------------------------------------------------------------*/
#define LEVEL1  1      		// Module check out (do not connect the motors) 
#define LEVEL2  2           // Verify ADC, calibrate the offset, run the motor open loop 
#define LEVEL3	3			// Auto-calibrate the current sensor offset
#define LEVEL4  4      		// Verify closed-loop operation of BLDC, not regulated. 
#define LEVEL5  5           // Verify the closed current loop and current PI controller.
#define LEVEL6  6           // Verify the closed speed loop and speed PI controller.
#define LEVEL7  7           // Verify the closed speed and speed PI controller & current PI controller.
						
/*------------------------------------------------------------------------------
This line sets the BUILDLEVEL to one of the available choices.
------------------------------------------------------------------------------*/
#define   BUILDLEVEL LEVEL7


#ifndef BUILDLEVEL    
#error  Critical: BUILDLEVEL must be defined !!
#endif  // BUILDLEVEL
//------------------------------------------------------------------------------


#ifndef TRUE
#define FALSE 0
#define TRUE  1
#endif

#define PI 3.14159265358979

// Define the system frequency (MHz)
#if (DSP2803x_DEVICE_H==1)
#define SYSTEM_FREQUENCY 60
#elif (DSP280x_DEVICE_H==1)
#define SYSTEM_FREQUENCY 100
#elif (F2806x_DEVICE_H==1)
#define SYSTEM_FREQUENCY 90
#endif

//Define system Math Type
// Select Floating Math Type for 2806x
// Select IQ Math Type for 2803x 
#if (DSP2803x_DEVICE_H==1)
#define MATH_TYPE 0 
#elif (F2806x_DEVICE_H==1)
#define MATH_TYPE 1
#endif


// Define the ISR frequency (kHz)
#define ISR_FREQUENCY 40
#define PWM_FREQUENCY 20

//cutoff freq and time constant of the offset calibration LPF
#define WC_CAL	100.0
#define TC_CAL	1/WC_CAL

// Parameter Initializations 
#define ALIGN_DUTY   0x0500
#define LOOP_CNT_MAX 0

//define starting and ending parameters for ramp generator
#define COMMUTATES_PER_E_REV 6.0

// This machine parameters are based on 24V PM motors inside Multi-Axis +PFC package
// Define the PMSM motor parameters
#define RS 		0.79               		// Stator resistance (ohm)
#define RR   	0               		// Rotor resistance (ohm) 
#define LS   	0.0012     				// Stator inductance (H) 
#define LR   	0						// Rotor inductance (H) 	
#define LM   	0						// Magnetizing inductance (H)

#define POLES_W78   8						// Number of poles
#define POLES_W750  4						// Number of poles

// Define the base quantites 
#define BASE_VOLTAGE         24		        // Base peak phase voltage (volt), maximum measurable DC Bus/sqrt(3)
// 최대 전류 36A, 부부하 전류는 1.2A정도임
#define BASE_CURRENT_W750    36            	// Base peak phase current (amp) , maximum measurable peak current
#define NOLOAD_CURRENT_W750  1.2          	// Base peak phase current (amp) , maximum measurable peak current
// 최대전류 5A, 부부하 전류 0.35A정도임
#define BASE_CURRENT_W78     5            	// Base peak phase current (amp) , maximum measurable peak current
#define NOLOAD_CURRENT_W78   0.35          	// Base peak phase current (amp) , maximum measurable peak current

#define BASE_FREQ_W78  	     268           	// Base electrical frequency (Hz)	[Load]		, 4000 rpm
#define BASE_FREQ_W750 	     100           	// Base electrical frequency (Hz)	[Load]		, 3000 rpm

#ifdef W750
#define MOTOR_TYPE 		M_W750
#define POLES			POLES_W750
#define BASE_FREQ		BASE_FREQ_W750
#define BASE_CURRENT	BASE_CURRENT_W750

#endif

#define CURRENT_AVG		0

#ifdef W78
#define MOTOR_TYPE 		M_W78
#define POLES			POLES_W78
#define BASE_FREQ		BASE_FREQ_W78
#define BASE_CURRENT	BASE_CURRENT_W78
#endif

#ifdef BOOST8301
#define BOARD_TYPE 		B_BOOST8301
#define CURRENT_SCALER 	(3.3/(BASE_CURRENT * 0.02))
#endif

#ifdef BOOST8305
#define BOARD_TYPE 		B_BOOST8305
#define CURRENT_SCALER 	(3.3/(BASE_CURRENT * 0.07))
#endif

#ifdef DRV8301HC
#define BOARD_TYPE 		B_DRV8301HC
#define CURRENT_SCALER 	(3.3/(BASE_CURRENT * 0.1))
#endif

#ifdef IMMB
#define BOARD_TYPE 		B_IMMB
#define CURRENT_SCALER 	(3.3/(BASE_CURRENT * 0.02))
#endif


typedef struct
{
	Uint16 u16Start;
	Uint16 u16Dir;
	Uint16 u16Run;
	float  fSpdRef;
	float  fCrnRef;
} dsIMDCtrlIfc;

typedef struct
{
	Uint16 u16Start;
	Uint16 u16Dir;
	Uint16 u16Run;
} dsInBuf;

#define PHASE_A_OFF		EPwm1Regs.TZFRC.bit.OST = 1
#define PHASE_A_ON		EPwm1Regs.TZCLR.bit.OST = 1

#define PHASE_B_OFF		EPwm2Regs.TZFRC.bit.OST = 1
#define PHASE_B_ON		EPwm2Regs.TZCLR.bit.OST = 1

#define PHASE_C_OFF		EPwm3Regs.TZFRC.bit.OST = 1
#define PHASE_C_ON		EPwm3Regs.TZCLR.bit.OST = 1

#endif 
