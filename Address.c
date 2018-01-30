/******************************************************************************
 **                  Copyright(C) 2016 NARA Control Co. Ltd.                 **
 ******************************************************************************
    FILE NAME   : Address.C
    AUTHOR      : 
    DATE        : 
    REVISION    : 
    DESCRIPTION : 

 ******************************************************************************
    HISTORY     :


 ******************************************************************************/
#include <stdint.h>
#include "address.h"


/*****************************************************************************
 **                                                                         **
 **  테이블 설정값 정의                                                     **
 **                                                                         **
 *****************************************************************************/

const DESCRIPT_ADDRESS_MAP  tb_MEMORY_MAP[MEMORY_TYPE_MAX] =
{
   /*   시작주소   ,     끝주소   ,      주소크기 ,     최대주소  */
   {     FLASH_BASE,     FLASH_END,     FLASH_SIZE,     FLASH_AREA  },    /* [0] MEMORY_TYPE_FLASH     */
   {     SRAM_BASE,      SRAM_END,      SRAM_SIZE,      SRAM_AREA   }     /* [1] MEMORY_TYPE_SRAM      */
};


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

/*****************************[ End of Program ]******************************/

