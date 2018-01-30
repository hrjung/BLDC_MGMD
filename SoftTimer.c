/*
 * SoftTimer.c
 *
 *  Created on: 2017. 2. 21.
 *      Author: sgjeong
 */

#include <stdint.h>
#include <stdbool.h>

#include "SoftTimer.h"

/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

volatile uint32_t           glSoftwareTimer;
DATETIME                    ghDateTime;

GLOBAL_HANDLE_STIMER_VAR    ghSTIMER;


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

 void SoftTimer_InitVariable(void)
 {
    glSoftwareTimer = (0L);

    STimer_Deactivate( &ghSTIMER.SecondScheduleTimer    );
    STimer_Deactivate( &ghSTIMER.MinuteScheduleTimer    );

    STimer_Deactivate( &ghSTIMER.DebugModeClearTimer    );
    STimer_Deactivate( &ghSTIMER.SerialPortReInitTimer  );

    STimer_Deactivate( &ghSTIMER.GuiInUseTimer          );
    STimer_Deactivate( &ghSTIMER.XmodemTimer            );
 }


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

 void STimer_Deactivate( SOFT_TIMER *pSoftTimer )
 {
    pSoftTimer->bIsElapsed = true;
    pSoftTimer->bIsActive  = false;
 }


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

 void STimer_Start( SOFT_TIMER *pSoftTimer, uint32_t lMsTimeout )
 {
    pSoftTimer->lStimerTimeout = (uint32_t) (lMsTimeout + GET_MSECTIME());

    pSoftTimer->bIsActive  = (bool) true;
    pSoftTimer->bIsElapsed = (bool) false;
    pSoftTimer->bIsElapsed = (bool) STimer_IsElapsed( pSoftTimer );
 }


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

 void STimer_Reset(SOFT_TIMER *pSoftTimer, uint32_t lMsTimeout)
 {
    pSoftTimer->lStimerTimeout += lMsTimeout;
    pSoftTimer->bIsActive       = (bool) true;
    pSoftTimer->bIsElapsed      = (bool) false;
    pSoftTimer->bIsElapsed      = (bool) STimer_IsElapsed(pSoftTimer);
 }


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

 bool STimer_IsElapsed( SOFT_TIMER *pSoftTimer )
 {
    if(!pSoftTimer->bIsElapsed)
    {
        pSoftTimer->bIsElapsed = (STimer_ReMainingTime(pSoftTimer)==0);
    }

    return( pSoftTimer->bIsElapsed );
 }


 /*****************************************************************************
  **                                                                         **
  **                                                                         **
  **                                                                         **
  *****************************************************************************/

 bool STimer_IsActElapsed( SOFT_TIMER *pSoftTimer )
{
	if (pSoftTimer->bIsActive) {
		if(!pSoftTimer->bIsElapsed) {
			pSoftTimer->bIsElapsed = (STimer_ReMainingTime(pSoftTimer)==0);
		}
		return( pSoftTimer->bIsElapsed );
	}
	return (false);
}


 /*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

 bool STimer_IsActive( SOFT_TIMER *pSoftTimer )
 {
    return( pSoftTimer->bIsActive );
 }


 /*****************************************************************************
  **                                                                         **
  **                                                                         **
  **                                                                         **
  *****************************************************************************/

 uint32_t STimer_ReMainingTime( SOFT_TIMER *pSoftTimer )
  {
	 uint32_t   lRemainingTime;

     lRemainingTime = pSoftTimer->lStimerTimeout - GET_MSECTIME();

     if( lRemainingTime > MACRO_MAX_DELAY_STIMER ) {
     	lRemainingTime = 0;
     }
     /*
     if( lRemainingTime < 0 ) {
     	lRemainingTime = 0;
     }
     */
     return( lRemainingTime );
  }


 /*****************************************************************************
  **                                                                         **
  **                                                                         **
  **                                                                         **
  *****************************************************************************/

  void STimer_SetElapsed( SOFT_TIMER *pSoftTimer )
  {
     pSoftTimer->bIsActive  = true;
     pSoftTimer->bIsElapsed = true;
  }


