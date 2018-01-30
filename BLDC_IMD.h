/* ==============================================================================
System Name:  	BLDC_Sensored

File Name:		BLDC_Sensored.h

Description:	Primary system header file for the Trapezoidal BLDC Control 
				Using Hall Effect Sensors  

Originator:		Digital control systems Group - Texas Instruments

Note: In this software, the default inverter is supposed to be DRV8312-EVM. 
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 02-13-2011	Version 1.0
=================================================================================  */

/*-------------------------------------------------------------------------------
Next, Include project specific include files.
-------------------------------------------------------------------------------*/

#include "com_trig.h"       	// Include header for the CMTN object
#include "pid_grando.h"       	// Include header for the PID_GRANDO_CONTROLLER object 
#include "rmp2cntl.h"       	// Include header for the RMP2 object
#include "rmp3cntl.h"       	// Include header for the RMP3 object
#include "impulse.h"       		// Include header for the IMPULSE object
#include "mod6_cnt.h"       	// Include header for the MOD6CNT object
#include "speed_pr.h"           // Include header for the SPEED_MEAS_REV object
#include "rmp_cntl.h"       	// Include header for the RMPCNTL object  

#if (DSP2803x_DEVICE_H==1)
#include "f2803xileg_vdc_BLDC.h"	// Include header for the ILEG2DCBUSMEAS object 
#include "f2803xbldcpwm_BLDC.h"     // Include header for the PWMGEN object
#include "f2803xpwmdac_BLDC.h"    	// Include header for the PWMGEN object
#include "f2803xhall_gpio_BLDC.h"   // Include header for the HALL object
#include "DSP2803x_EPwm_defines.h"	// Include header for PWM defines 
#endif

#if (F2806x_DEVICE_H==1)
#include "f2806xileg_vdc_BLDC.h"	// Include header for the ILEG2DCBUSMEAS object 
#include "f2806xbldcpwm_BLDC.h"     // Include header for the PWMGEN object
#include "f2806xpwmdac_BLDC.h"    	// Include header for the PWMGEN object
#include "f2806xhall_gpio_BLDC.h"   // Include header for the HALL object
#include "F2806x_EPwm_defines.h" 	// Include header for PWM defines 
#endif

#include "dlog4ch-BLDC_Sensored.h" // Include header for the DLOG_4CH object

//SPI only used for DRV830X
#if defined(DRV830X)
#include "DRV830X_SPI.h"
#endif

//===========================================================================
// No more.
//===========================================================================
