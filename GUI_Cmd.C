/******************************************************************************
    FILE NAME   : GUI_Cmd.C
    AUTHOR      : SJeong
    DATE        : 2016.xx.xx ~ 2016.xx.xx
    REVISION    : Ver0.10
    DESCRIPTION : GUI Cmd. Function.

 ******************************************************************************
    HISTORY     :


 ******************************************************************************/


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "GUI_Comm.h"
#include "GUI_Cmd.H"


#if(0)

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/
 
uint32_t u32FCSBitMask[8];


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

FCS_CONFIGDATA_VARS       gFCSConfigData;
FCS_CONFIG_INFO_VARS      gFCSConfigInfo;

/******************************************************************************
 **                                                                          **
 **   각 FB type별 함수 실행                                                 **
 **                                                                          **
 ******************************************************************************/
const uint32_t   u32FCSInterlockDevIdx[FCS_DEVICE_MAX] =
{
    #if (CURRDIFF_RELAY)
    ELMT_COMM_43CA_ON_C,
    ELMT_COMM_43CA_OFF_C,
    ELMT_COMM_43RC_ON_C,
    ELMT_COMM_43RC_OFF_C,
    ELMT_COMM_43CA_ON_C_P,
    ELMT_COMM_43CA_OFF_C_P,
    ELMT_COMM_43RC_ON_C_P,
    ELMT_COMM_43RC_OFF_C_P,
    ELMT_COMM_43PDA_AUTO_C,
    ELMT_COMM_43PDA_MANU_C,
    ELMT_COMM_43PDA_OFF_C,
    ELMT_COMM_43PDA_A_BUS_C,
    ELMT_COMM_43PDA_B_BUS_C,
    ELMT_COMM_43PDA_A_AUTO_C,
    ELMT_COMM_43PDA_B_AUTO_C,
    ELMT_COMM_GOOSE_SUBFAIL,
    ELMT_COMM_REPORT_OVER,
    ELMT_COMM_IED_ALARM,
    ELMT_COMM_CB_CLOSE_C,
    ELMT_COMM_CB_OPEN_C,   
    ELMT_COMM_CB_CLOSE_INTERLOCK,
    ELMT_COMM_CB_OPEN_INTERLOCK,   
    #endif
    #if (DISTANCE_RELAY)
    ELMT_COMM_43PDA_AUTO_C,
    ELMT_COMM_43PDA_MANU_C,
    ELMT_COMM_43PDA_OFF_C,
    ELMT_COMM_43PDA_A_BUS_C,
    ELMT_COMM_43PDA_B_BUS_C,
    ELMT_COMM_43PDA_A_AUTO_C,
    ELMT_COMM_43PDA_B_AUTO_C,
    ELMT_COMM_GOOSE_SUBFAIL,
    ELMT_COMM_REPORT_OVER,
    ELMT_COMM_IED_ALARM,
    ELMT_COMM_RESERVED1,
    ELMT_COMM_RESERVED2,
    ELMT_COMM_CB_CLOSE_C,
    ELMT_COMM_CB_OPEN_C,
    ELMT_COMM_1DS_CLOSE_C,
    ELMT_COMM_1DS_OPEN_C,
    ELMT_COMM_2DS_CLOSE_C,
    ELMT_COMM_2DS_OPEN_C,
    ELMT_COMM_6DS_CLOSE_C,
    ELMT_COMM_6DS_OPEN_C,    
    ELMT_COMM_CB_CLOSE_INTERLOCK,
    ELMT_COMM_CB_OPEN_INTERLOCK,
    ELMT_COMM_1DS_CLOSE_INTERLOCK,
    ELMT_COMM_1DS_OPEN_INTERLOCK,
    ELMT_COMM_2DS_CLOSE_INTERLOCK,
    ELMT_COMM_2DS_OPEN_INTERLOCK,
    ELMT_COMM_6DS_CLOSE_INTERLOCK,
    ELMT_COMM_6DS_OPEN_INTERLOCK,    
    #endif
};

#endif

#if(0)
/******************************************************************************
 **                                                                          **
 **   각 FB type별 함수 실행                                                 **
 **                                                                          **
 ******************************************************************************/

void FCS_SetFBLib(unsigned char u8Type, unsigned char * pbData)
{
    switch( u8Type )
    {
    case FCSDEF_IO_CODE          : FCS_FB_io     ((FCS_IO_VARS *) pbData);      break;
    case FCSDEF_VAR_CODE         : FCS_FB_var    ((FCS_VAR_VARS *) pbData);     break;
    case FCSDEF_REM_BLK_CODE     : FCS_FB_rem_blk((FCS_REM_BLK_VARS *) pbData); break;

    case FCSDEF_BI_CODE          : break;
    case FCSDEF_BO_CODE          : break;
    case FCSDEF_AI_CODE          : break;
    case FCSDEF_AO_CODE          : break;

#if (FCS_FB_NEWDEF)
    case FCSDEF_AND2_CODE		  : FCS_FB_and2 ((FCS_AND2_VARS  *) pbData);     break;
    case FCSDEF_AND3_CODE		  : FCS_FB_and3 ((FCS_AND3_VARS  *) pbData);     break;
    case FCSDEF_AND4_CODE		  : FCS_FB_and4 ((FCS_AND4_VARS  *) pbData);     break;
    case FCSDEF_AND5_CODE		  : FCS_FB_and5 ((FCS_AND5_VARS  *) pbData);     break;
    case FCSDEF_AND6_CODE		  : FCS_FB_and6 ((FCS_AND6_VARS  *) pbData);     break;
    case FCSDEF_AND7_CODE		  : FCS_FB_and7 ((FCS_AND7_VARS  *) pbData);     break;
    case FCSDEF_AND8_CODE		  : FCS_FB_and8 ((FCS_AND8_VARS  *) pbData);     break;
    case FCSDEF_OR2_CODE		  : FCS_FB_or2  ((FCS_OR2_VARS   *) pbData);     break;
    case FCSDEF_OR3_CODE		  : FCS_FB_or3  ((FCS_OR3_VARS   *) pbData);     break;
    case FCSDEF_OR4_CODE		  : FCS_FB_or4  ((FCS_OR4_VARS   *) pbData);     break;
    case FCSDEF_OR5_CODE		  : FCS_FB_or5  ((FCS_OR5_VARS   *) pbData);     break;
    case FCSDEF_OR6_CODE		  : FCS_FB_or6  ((FCS_OR6_VARS   *) pbData);     break;
    case FCSDEF_OR7_CODE		  : FCS_FB_or7  ((FCS_OR7_VARS   *) pbData);     break;
    case FCSDEF_OR8_CODE		  : FCS_FB_or8  ((FCS_OR8_VARS   *) pbData);     break;
    case FCSDEF_NOT_CODE		  : FCS_FB_not  ((FCS_NOT_VARS   *) pbData);     break;
    case FCSDEF_NAND2_CODE		  : FCS_FB_nand2((FCS_AND2_VARS  *) pbData);     break;
    case FCSDEF_NAND3_CODE		  : FCS_FB_nand3((FCS_AND3_VARS  *) pbData);     break;
    case FCSDEF_NAND4_CODE		  : FCS_FB_nand4((FCS_AND4_VARS  *) pbData);     break;
    case FCSDEF_NAND5_CODE		  : FCS_FB_nand5((FCS_AND5_VARS  *) pbData);     break;
    case FCSDEF_NAND6_CODE		  : FCS_FB_nand6((FCS_AND6_VARS  *) pbData);     break;
    case FCSDEF_NAND7_CODE		  : FCS_FB_nand7((FCS_AND7_VARS  *) pbData);     break;
    case FCSDEF_NAND8_CODE		  : FCS_FB_nand8((FCS_AND8_VARS  *) pbData);     break;
    case FCSDEF_NOR2_CODE		  : FCS_FB_nor2 ((FCS_OR2_VARS   *) pbData);     break;
    case FCSDEF_NOR3_CODE		  : FCS_FB_nor3 ((FCS_OR3_VARS   *) pbData);     break;
    case FCSDEF_NOR4_CODE		  : FCS_FB_nor4 ((FCS_OR4_VARS   *) pbData);     break;
    case FCSDEF_NOR5_CODE		  : FCS_FB_nor5 ((FCS_OR5_VARS   *) pbData);     break;
    case FCSDEF_NOR6_CODE		  : FCS_FB_nor6 ((FCS_OR6_VARS   *) pbData);     break;
    case FCSDEF_NOR7_CODE		  : FCS_FB_nor7 ((FCS_OR7_VARS   *) pbData);     break;
    case FCSDEF_NOR8_CODE		  : FCS_FB_nor8 ((FCS_OR8_VARS   *) pbData);     break;
    case FCSDEF_XOR2_CODE		  : FCS_FB_xor2 ((FCS_XOR_VARS   *) pbData);     break;
    case FCSDEF_SUM2_CODE		  : FCS_FB_sum2 ((FCS_SUM2_VARS  *) pbData);     break;
    case FCSDEF_SUM3_CODE		  : FCS_FB_sum3 ((FCS_SUM3_VARS  *) pbData);     break;
    case FCSDEF_DIF2_CODE		  : FCS_FB_dif2 ((FCS_DIF2_VARS  *) pbData);     break;
    case FCSDEF_MUL2_CODE		  : FCS_FB_mul2 ((FCS_MUL2_VARS  *) pbData);     break;
    case FCSDEF_QUO2_CODE		  : FCS_FB_quo2 ((FCS_QUO2_VARS  *) pbData);     break;
    case FCSDEF_REM2_CODE		  : FCS_FB_rem2 ((FCS_REM2_VARS  *) pbData);     break;
    case FCSDEF_MAX2_CODE		  : FCS_FB_max2 ((FCS_MAX2_VARS  *) pbData);     break;
    case FCSDEF_MAX3_CODE		  : FCS_FB_max3 ((FCS_MAX3_VARS  *) pbData);     break;
    case FCSDEF_MIN2_CODE		  : FCS_FB_min2 ((FCS_MIN2_VARS  *) pbData);     break;
    case FCSDEF_MIN3_CODE		  : FCS_FB_min3 ((FCS_MIN3_VARS  *) pbData);     break;
    case FCSDEF_ABS_CODE		  : FCS_FB_abs  ((FCS_ABS_VARS   *) pbData);     break;
    case FCSDEF_GT_CODE			  : FCS_FB_gt   ((FCS_GT_VARS    *) pbData);     break;
    case FCSDEF_LT_CODE			  : FCS_FB_lt   ((FCS_LT_VARS    *) pbData);     break;
    case FCSDEF_EQU_CODE		  : FCS_FB_equ  ((FCS_EQU_VARS   *) pbData);     break;
    case FCSDEF_SUMF2_CODE		  : FCS_FB_sumf2((FCS_SUMF2_VARS *) pbData);     break;
    case FCSDEF_SUMF3_CODE		  : FCS_FB_sumf3((FCS_SUMF3_VARS *) pbData);     break;
    case FCSDEF_DIFF2_CODE		  : FCS_FB_diff2((FCS_DIFF2_VARS *) pbData);     break;
    case FCSDEF_MULF2_CODE		  : FCS_FB_mulf2((FCS_MULF2_VARS *) pbData);     break;
    case FCSDEF_DIVF2_CODE		  : FCS_FB_divf2((FCS_DIVF2_VARS *) pbData);     break;
    case FCSDEF_MAXF2_CODE		  : FCS_FB_maxf2((FCS_MAXF2_VARS *) pbData);     break;
    case FCSDEF_MAXF3_CODE		  : FCS_FB_maxf3((FCS_MAXF3_VARS *) pbData);     break;
    case FCSDEF_MINF2_CODE		  : FCS_FB_minf2((FCS_MINF2_VARS *) pbData);     break;
    case FCSDEF_MINF3_CODE		  : FCS_FB_minf3((FCS_MINF3_VARS *) pbData);     break;
    case FCSDEF_ABSF_CODE		  : FCS_FB_absf ((FCS_ABSF_VARS  *) pbData);     break;
    case FCSDEF_GTF_CODE		  : FCS_FB_gtf  ((FCS_GTF_VARS   *) pbData);     break;
    case FCSDEF_LTF_CODE		  : FCS_FB_ltf  ((FCS_LTF_VARS   *) pbData);     break;
    case FCSDEF_EQUF_CODE		  : FCS_FB_equf ((FCS_EQUF_VARS  *) pbData);     break;

    case FCSDEF_SRLATCH_CODE	  : FCS_FB_srlatch    ((FCS_SRLATCH_VARS *) pbData);     break;
    case FCSDEF_RISINGEDGE_CODE	  : FCS_FB_risingedge ((FCS_EDGE_VARS    *) pbData);     break;
    case FCSDEF_FALLINGEDGE_CODE  : FCS_FB_fallingedge((FCS_EDGE_VARS    *) pbData);     break;

    case FCSDEF_ONTIMER_CODE	  : FCS_FB_ondelaytimer ((FCS_TIMER_VARS *) pbData);     break;
    case FCSDEF_OFFTIMER_CODE	  : FCS_FB_offdelaytimer((FCS_TIMER_VARS *) pbData);     break;
    case FCSDEF_ONESHOTTIMER_CODE : FCS_FB_oneshottimer ((FCS_TIMER_VARS *) pbData);     break;
#else
    case FCSDEF_AND2_CODE        : FCS_FB_and2((FCS_AND2_VARS *) pbData);       break;
    case FCSDEF_OR2_CODE         : FCS_FB_or2((FCS_OR2_VARS *) pbData);         break;
    case FCSDEF_NAND2_CODE       : FCS_FB_nand2((FCS_AND2_VARS *) pbData);      break;
    case FCSDEF_NOR2_CODE        : FCS_FB_nor2((FCS_OR2_VARS *) pbData);        break;

    case FCSDEF_AND3_CODE        : FCS_FB_and3((FCS_AND3_VARS *) pbData);       break;
    case FCSDEF_AND4_CODE        : FCS_FB_and4((FCS_AND4_VARS *) pbData);       break;
    case FCSDEF_AND5_CODE        : FCS_FB_and5((FCS_AND5_VARS *) pbData);       break;
    case FCSDEF_AND6_CODE        : FCS_FB_and6((FCS_AND6_VARS *) pbData);       break;
    case FCSDEF_AND8_CODE        : FCS_FB_and8((FCS_AND8_VARS *) pbData);       break;
    case FCSDEF_OR3_CODE         : FCS_FB_or3((FCS_OR3_VARS *) pbData);         break;
    case FCSDEF_OR4_CODE         : FCS_FB_or4((FCS_OR4_VARS *) pbData);         break;
    case FCSDEF_OR5_CODE         : FCS_FB_or5((FCS_OR5_VARS *) pbData);         break;
    case FCSDEF_OR6_CODE         : FCS_FB_or6((FCS_OR6_VARS *) pbData);         break;
    case FCSDEF_OR7_CODE         : FCS_FB_or7((FCS_OR7_VARS *) pbData);         break;
    case FCSDEF_OR8_CODE         : FCS_FB_or8((FCS_OR8_VARS *) pbData);         break;

    case FCSDEF_NOT_CODE         : FCS_FB_not((FCS_NOT_VARS *) pbData);         break;

    case FCSDEF_SUM2_CODE        : FCS_FB_sum2 ((FCS_SUM2_VARS  *) pbData);     break;

    case FCSDEF_GT_CODE          : FCS_FB_gt((FCS_GT_VARS *) pbData);           break;

#endif
    default : break;

    }    
}


/******************************************************************************
 **                                                                          **
 **  Function Block의 data 구조체의 사이즈 저장                              **
 **                                                                          **
 ******************************************************************************/

void FCS_SetSizeofFB()
{
	// I/O, variable 등 기본 데이터 구조체 크기 지정
	u32FCSSizeofFB[FCSDEF_IO_CODE]		     = sizeof(FCS_IO_VARS);
	u32FCSSizeofFB[FCSDEF_VAR_CODE]		     = sizeof(FCS_VAR_VARS);
	u32FCSSizeofFB[FCSDEF_REM_BLK_CODE] 	 = sizeof(FCS_REM_BLK_VARS);

	u32FCSSizeofFB[FCSDEF_BI_CODE]		     = sizeof(FCS_BI_VARS);
	u32FCSSizeofFB[FCSDEF_BO_CODE]		     = sizeof(FCS_BO_VARS);
	u32FCSSizeofFB[FCSDEF_AI_CODE]		     = sizeof(FCS_AI_VARS);
	u32FCSSizeofFB[FCSDEF_AO_CODE]		     = sizeof(FCS_AO_VARS);

    ClearWatchdog();

	// Function block의 데이터 구조체 크기 지정
#if (FCS_FB_NEWDEF)
	u32FCSSizeofFB[FCSDEF_AND2_CODE 	  ] = sizeof(FCS_AND2_VARS  );
	u32FCSSizeofFB[FCSDEF_AND3_CODE 	  ] = sizeof(FCS_AND3_VARS  );
	u32FCSSizeofFB[FCSDEF_AND4_CODE 	  ] = sizeof(FCS_AND4_VARS  );
	u32FCSSizeofFB[FCSDEF_AND5_CODE 	  ] = sizeof(FCS_AND5_VARS  );
	u32FCSSizeofFB[FCSDEF_AND6_CODE 	  ] = sizeof(FCS_AND6_VARS  );
	u32FCSSizeofFB[FCSDEF_AND7_CODE 	  ] = sizeof(FCS_AND7_VARS  );
	u32FCSSizeofFB[FCSDEF_AND8_CODE 	  ] = sizeof(FCS_AND8_VARS  );
	u32FCSSizeofFB[FCSDEF_OR2_CODE		  ] = sizeof(FCS_OR2_VARS   );
	u32FCSSizeofFB[FCSDEF_OR3_CODE		  ] = sizeof(FCS_OR3_VARS   );
	u32FCSSizeofFB[FCSDEF_OR4_CODE		  ] = sizeof(FCS_OR4_VARS   );
	u32FCSSizeofFB[FCSDEF_OR5_CODE		  ] = sizeof(FCS_OR5_VARS   );
	u32FCSSizeofFB[FCSDEF_OR6_CODE		  ] = sizeof(FCS_OR6_VARS   );
	u32FCSSizeofFB[FCSDEF_OR7_CODE		  ] = sizeof(FCS_OR7_VARS   );
	u32FCSSizeofFB[FCSDEF_OR8_CODE		  ] = sizeof(FCS_OR8_VARS   );
	u32FCSSizeofFB[FCSDEF_NOT_CODE		  ] = sizeof(FCS_NOT_VARS  );
	u32FCSSizeofFB[FCSDEF_NAND2_CODE	  ] = sizeof(FCS_AND2_VARS  );
	u32FCSSizeofFB[FCSDEF_NAND3_CODE	  ] = sizeof(FCS_AND3_VARS  );
	u32FCSSizeofFB[FCSDEF_NAND4_CODE	  ] = sizeof(FCS_AND4_VARS  );
	u32FCSSizeofFB[FCSDEF_NAND5_CODE	  ] = sizeof(FCS_AND5_VARS  );
	u32FCSSizeofFB[FCSDEF_NAND6_CODE	  ] = sizeof(FCS_AND6_VARS  );
	u32FCSSizeofFB[FCSDEF_NAND7_CODE	  ] = sizeof(FCS_AND7_VARS  );
	u32FCSSizeofFB[FCSDEF_NAND8_CODE	  ] = sizeof(FCS_AND8_VARS  );
	u32FCSSizeofFB[FCSDEF_NOR2_CODE 	  ] = sizeof(FCS_OR2_VARS   );
	u32FCSSizeofFB[FCSDEF_NOR3_CODE 	  ] = sizeof(FCS_OR3_VARS   );
	u32FCSSizeofFB[FCSDEF_NOR4_CODE 	  ] = sizeof(FCS_OR4_VARS   );
	u32FCSSizeofFB[FCSDEF_NOR5_CODE 	  ] = sizeof(FCS_OR5_VARS   );
	u32FCSSizeofFB[FCSDEF_NOR6_CODE 	  ] = sizeof(FCS_OR6_VARS   );
	u32FCSSizeofFB[FCSDEF_NOR7_CODE 	  ] = sizeof(FCS_OR7_VARS   );
	u32FCSSizeofFB[FCSDEF_NOR8_CODE 	  ] = sizeof(FCS_OR8_VARS   );
	u32FCSSizeofFB[FCSDEF_XOR2_CODE 	  ] = sizeof(FCS_XOR_VARS   );
	u32FCSSizeofFB[FCSDEF_SUM2_CODE 	  ] = sizeof(FCS_SUM2_VARS  );
	u32FCSSizeofFB[FCSDEF_SUM3_CODE 	  ] = sizeof(FCS_SUM3_VARS  );
	u32FCSSizeofFB[FCSDEF_DIF2_CODE 	  ] = sizeof(FCS_DIF2_VARS  );
	u32FCSSizeofFB[FCSDEF_MUL2_CODE 	  ] = sizeof(FCS_MUL2_VARS  );
	u32FCSSizeofFB[FCSDEF_QUO2_CODE 	  ] = sizeof(FCS_QUO2_VARS  );
	u32FCSSizeofFB[FCSDEF_REM2_CODE 	  ] = sizeof(FCS_REM2_VARS  );
	u32FCSSizeofFB[FCSDEF_MAX2_CODE 	  ] = sizeof(FCS_MAX2_VARS  );
	u32FCSSizeofFB[FCSDEF_MAX3_CODE 	  ] = sizeof(FCS_MAX3_VARS  );
	u32FCSSizeofFB[FCSDEF_MIN2_CODE 	  ] = sizeof(FCS_MIN2_VARS  );
	u32FCSSizeofFB[FCSDEF_MIN3_CODE 	  ] = sizeof(FCS_MIN3_VARS  );
	u32FCSSizeofFB[FCSDEF_ABS_CODE  	  ] = sizeof(FCS_ABS_VARS   );
	u32FCSSizeofFB[FCSDEF_GT_CODE   	  ] = sizeof(FCS_GT_VARS    );
	u32FCSSizeofFB[FCSDEF_LT_CODE   	  ] = sizeof(FCS_LT_VARS    );
	u32FCSSizeofFB[FCSDEF_EQU_CODE		  ] = sizeof(FCS_EQU_VARS   );
	u32FCSSizeofFB[FCSDEF_SUMF2_CODE	  ] = sizeof(FCS_SUMF2_VARS );
	u32FCSSizeofFB[FCSDEF_SUMF3_CODE	  ] = sizeof(FCS_SUMF3_VARS );
	u32FCSSizeofFB[FCSDEF_DIFF2_CODE	  ] = sizeof(FCS_DIFF2_VARS );
	u32FCSSizeofFB[FCSDEF_MULF2_CODE	  ] = sizeof(FCS_MULF2_VARS );
	u32FCSSizeofFB[FCSDEF_DIVF2_CODE	  ] = sizeof(FCS_DIVF2_VARS );
	u32FCSSizeofFB[FCSDEF_MAXF2_CODE	  ] = sizeof(FCS_MAXF2_VARS );
	u32FCSSizeofFB[FCSDEF_MAXF3_CODE	  ] = sizeof(FCS_MAXF3_VARS );
	u32FCSSizeofFB[FCSDEF_MINF2_CODE	  ] = sizeof(FCS_MINF2_VARS );
	u32FCSSizeofFB[FCSDEF_MINF3_CODE	  ] = sizeof(FCS_MINF3_VARS );
	u32FCSSizeofFB[FCSDEF_ABSF_CODE 	  ] = sizeof(FCS_ABSF_VARS  );
	u32FCSSizeofFB[FCSDEF_GTF_CODE		  ] = sizeof(FCS_GTF_VARS   );
	u32FCSSizeofFB[FCSDEF_LTF_CODE		  ] = sizeof(FCS_LTF_VARS   );
	u32FCSSizeofFB[FCSDEF_EQUF_CODE 	  ] = sizeof(FCS_EQUF_VARS  );
	u32FCSSizeofFB[FCSDEF_SRLATCH_CODE    ] = sizeof(FCS_SRLATCH_VARS);
	u32FCSSizeofFB[FCSDEF_RISINGEDGE_CODE ] = sizeof(FCS_EDGE_VARS   );
	u32FCSSizeofFB[FCSDEF_FALLINGEDGE_CODE] = sizeof(FCS_EDGE_VARS   );

	u32FCSSizeofFB[FCSDEF_ONTIMER_CODE	   ] = sizeof(FCS_TIMER_VARS );
	u32FCSSizeofFB[FCSDEF_OFFTIMER_CODE	   ] = sizeof(FCS_TIMER_VARS );
	u32FCSSizeofFB[FCSDEF_ONESHOTTIMER_CODE] = sizeof(FCS_TIMER_VARS );
#else
	u32FCSSizeofFB[FCSDEF_AND2_CODE]		 = sizeof(FCS_AND2_VARS);
	u32FCSSizeofFB[FCSDEF_AND3_CODE]		 = sizeof(FCS_AND3_VARS);
	u32FCSSizeofFB[FCSDEF_AND4_CODE]		 = sizeof(FCS_AND4_VARS);
	u32FCSSizeofFB[FCSDEF_AND5_CODE]		 = sizeof(FCS_AND5_VARS);
	u32FCSSizeofFB[FCSDEF_AND6_CODE]		 = sizeof(FCS_AND6_VARS);
	u32FCSSizeofFB[FCSDEF_AND8_CODE]		 = sizeof(FCS_AND8_VARS);

	u32FCSSizeofFB[FCSDEF_OR2_CODE]		     = sizeof(FCS_OR2_VARS);
	u32FCSSizeofFB[FCSDEF_OR3_CODE]		     = sizeof(FCS_OR3_VARS);
	u32FCSSizeofFB[FCSDEF_OR4_CODE]		     = sizeof(FCS_OR4_VARS);
	u32FCSSizeofFB[FCSDEF_OR5_CODE]		     = sizeof(FCS_OR5_VARS);
	u32FCSSizeofFB[FCSDEF_OR6_CODE]		     = sizeof(FCS_OR6_VARS);
	u32FCSSizeofFB[FCSDEF_OR7_CODE]		     = sizeof(FCS_OR7_VARS);
	u32FCSSizeofFB[FCSDEF_OR8_CODE]		     = sizeof(FCS_OR8_VARS);

	u32FCSSizeofFB[FCSDEF_NAND2_CODE]		 = sizeof(FCS_AND2_VARS);

	u32FCSSizeofFB[FCSDEF_NOR2_CODE]		 = sizeof(FCS_OR2_VARS);

	u32FCSSizeofFB[FCSDEF_NOT_CODE]		     = sizeof(FCS_NOT_VARS);

	u32FCSSizeofFB[FCSDEF_SUM2_CODE]		 = sizeof(FCS_SUM2_VARS);

	u32FCSSizeofFB[FCSDEF_GT_CODE]		     = sizeof(FCS_GT_VARS);
#endif
    ClearWatchdog();

}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void FCS_InterlockDataSave(unsigned char u8PageNo, uint32_t *pu32FbSrc)
{
	FCS_IO_VARS        *pIOVar;
	FCS_REM_BLK_VARS   *pRemBlkVar;
    FCS_AND2_VARS      *pAND2Var;
    FCS_AND3_VARS      *pAND3Var;
    FCS_AND4_VARS      *pAND4Var;
    FCS_AND5_VARS      *pAND5Var;
    FCS_AND6_VARS      *pAND6Var;
    FCS_AND7_VARS      *pAND7Var;
    FCS_AND8_VARS      *pAND8Var;
    FCS_NOT_VARS       *pNotVar;
	FCS_TAG_ID_STRUCT  *pTagId;
	unsigned char					i;

 	pTagId = (FCS_TAG_ID_STRUCT *)pu32FbSrc;

    ClearWatchdog();

    if( pTagId->fb.type != FCSDEF_REM_BLK_CODE )
    {
        gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8FbId              = pTagId->fb.type;
        gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8Tag_Info.u8PageId = u8PageNo;
        gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8Tag_Info.u8TagId  = pTagId->fb.id - 1;
        gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8Tag_xy.u8PageId   = 0;
        gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8Tag_xy.u8TagId    = 0;
        gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8Flag              = 0;
        gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8Out_Idx           = 0;
        memset( gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Idx, 0,  FCS_MAX_IN_NUM);
        
        ClearWatchdog();

        if( pTagId->fb.type == FCSDEF_IO_CODE )
        {
        	pIOVar = (FCS_IO_VARS *)pu32FbSrc;
            gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8IOId = pIOVar->obj.io.id;

            if( pIOVar->in.addrOrVal.addr == FCSDEF_NULL_FB_INPUT )
            {
                gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Num  = 0;
                gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[0].u8PageId = 0;
                gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[0].u8TagId  = 0;
            }
            else
            {
                gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Num = 1;
                if( (pIOVar->in.addrOrVal.tagId.io.type != FCSDEF_REM_BLK_CODE)&&(pIOVar->in.addrOrVal.tagId.io.type != FCSDEF_VAR_CODE) )
                {
                    gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[0].u8PageId = u8PageNo;
                    gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[0].u8TagId  = pIOVar->in.addrOrVal.tagId.io.id-1;
                }
                else
                {
                    pRemBlkVar = (FCS_REM_BLK_VARS *)gFCSLogicData.u32pBlk[u8PageNo][pIOVar->in.addrOrVal.tagId.io.id - 1];
                    gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[0].u8PageId = (pRemBlkVar->obj.fb.pageId-1);
                    gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[0].u8TagId  = pRemBlkVar->obj.fb.id-1;
                
                }
            }
        }
        else
        {
            gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8IOId = 0;

            if( (pTagId->fb.type == FCSDEF_AND2_CODE)||(pTagId->fb.type == FCSDEF_NAND2_CODE)||(pTagId->fb.type == FCSDEF_OR2_CODE)||
                (pTagId->fb.type == FCSDEF_NOR2_CODE)||(pTagId->fb.type == FCSDEF_XOR2_CODE )||(pTagId->fb.type == FCSDEF_SRLATCH_CODE) )
            {
                pAND2Var = (FCS_AND2_VARS *)pu32FbSrc;
                gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Num  = pAND2Var->numIn;

				for( i=0; i<2; i++)
				{
					if( (pAND2Var->in[i].addrOrVal.tagId.io.type != FCSDEF_REM_BLK_CODE)||(pAND2Var->in[i].addrOrVal.tagId.io.type != FCSDEF_VAR_CODE) )
					{
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8PageId = u8PageNo;
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8TagId  = pAND2Var->in[i].addrOrVal.tagId.io.id-1;
					}
					else
					{
						pRemBlkVar = (FCS_REM_BLK_VARS *)gFCSLogicData.u32pBlk[u8PageNo][pAND2Var->in[i].addrOrVal.tagId.io.id - 1];
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8PageId = (pRemBlkVar->obj.fb.pageId-1);
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8TagId  = (pRemBlkVar->obj.fb.id-1);
					
					}
					
                    ClearWatchdog();
				}
            }
            else if( (pTagId->fb.type == FCSDEF_AND3_CODE)||(pTagId->fb.type == FCSDEF_OR3_CODE )
                   ||(pTagId->fb.type == FCSDEF_NAND3_CODE)||(pTagId->fb.type == FCSDEF_NOR3_CODE))
            {
                pAND3Var = (FCS_AND3_VARS *)pu32FbSrc;
                gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Num  = pAND3Var->numIn;

				for( i=0; i<3; i++)
				{
					if( (pAND3Var->in[i].addrOrVal.tagId.io.type != FCSDEF_REM_BLK_CODE)||(pAND3Var->in[i].addrOrVal.tagId.io.type != FCSDEF_VAR_CODE) )
					{
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8PageId = u8PageNo;
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8TagId  = pAND3Var->in[i].addrOrVal.tagId.io.id-1;
					}
					else
					{
						pRemBlkVar = (FCS_REM_BLK_VARS *)gFCSLogicData.u32pBlk[u8PageNo][pAND3Var->in[i].addrOrVal.tagId.io.id - 1];
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8PageId = (pRemBlkVar->obj.fb.pageId-1);
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8TagId  = (pRemBlkVar->obj.fb.id-1);
					
					}
					
                    ClearWatchdog();
				}
            }
            else if( (pTagId->fb.type == FCSDEF_AND4_CODE)||(pTagId->fb.type == FCSDEF_OR4_CODE ) 
                   ||(pTagId->fb.type == FCSDEF_NAND4_CODE)||(pTagId->fb.type == FCSDEF_NOR4_CODE))
            {
                pAND4Var = (FCS_AND4_VARS *)pu32FbSrc;
                gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Num  = pAND4Var->numIn;

				for( i=0; i<4; i++)
				{
					if( (pAND4Var->in[i].addrOrVal.tagId.io.type != FCSDEF_REM_BLK_CODE)||(pAND4Var->in[i].addrOrVal.tagId.io.type != FCSDEF_VAR_CODE) )
					{
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8PageId = u8PageNo;
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8TagId  = pAND4Var->in[i].addrOrVal.tagId.io.id-1;
					}
					else
					{
						pRemBlkVar = (FCS_REM_BLK_VARS *)gFCSLogicData.u32pBlk[u8PageNo][pAND4Var->in[i].addrOrVal.tagId.io.id - 1];
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8PageId = (pRemBlkVar->obj.fb.pageId-1);
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8TagId  = (pRemBlkVar->obj.fb.id-1);
					
					}
					
                    ClearWatchdog();
				}
            }
            else if( (pTagId->fb.type == FCSDEF_AND5_CODE)||(pTagId->fb.type == FCSDEF_OR5_CODE ) 
                   ||(pTagId->fb.type == FCSDEF_NAND5_CODE)||(pTagId->fb.type == FCSDEF_NOR5_CODE))
            {
                pAND5Var = (FCS_AND5_VARS *)pu32FbSrc;
                gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Num  = pAND5Var->numIn;

				for( i=0; i<5; i++)
				{
					if( (pAND5Var->in[i].addrOrVal.tagId.io.type != FCSDEF_REM_BLK_CODE)||(pAND5Var->in[i].addrOrVal.tagId.io.type != FCSDEF_VAR_CODE) )
					{
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8PageId = u8PageNo;
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8TagId  = pAND5Var->in[i].addrOrVal.tagId.io.id-1;
					}
					else
					{
						pRemBlkVar = (FCS_REM_BLK_VARS *)gFCSLogicData.u32pBlk[u8PageNo][pAND5Var->in[i].addrOrVal.tagId.io.id - 1];
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8PageId = (pRemBlkVar->obj.fb.pageId-1);
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8TagId  = (pRemBlkVar->obj.fb.id-1);
					
					}
					
                    ClearWatchdog();
				}
            }
            else if( (pTagId->fb.type == FCSDEF_AND6_CODE)||(pTagId->fb.type == FCSDEF_OR6_CODE ) 
                   ||(pTagId->fb.type == FCSDEF_NAND6_CODE)||(pTagId->fb.type == FCSDEF_NOR6_CODE))
            {
                pAND6Var = (FCS_AND6_VARS *)pu32FbSrc;
                gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Num  = pAND6Var->numIn;

				for( i=0; i<6; i++)
				{
					if( (pAND6Var->in[i].addrOrVal.tagId.io.type != FCSDEF_REM_BLK_CODE)||(pAND6Var->in[i].addrOrVal.tagId.io.type != FCSDEF_VAR_CODE) )
					{
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8PageId = u8PageNo;
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8TagId  = pAND6Var->in[i].addrOrVal.tagId.io.id-1;
					}
					else
					{
						pRemBlkVar = (FCS_REM_BLK_VARS *)gFCSLogicData.u32pBlk[u8PageNo][pAND6Var->in[i].addrOrVal.tagId.io.id - 1];
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8PageId = (pRemBlkVar->obj.fb.pageId-1);
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8TagId  = (pRemBlkVar->obj.fb.id-1);
					
					}
					
                    ClearWatchdog();
				}
            }
            else if( (pTagId->fb.type == FCSDEF_AND7_CODE)||(pTagId->fb.type == FCSDEF_OR7_CODE )
                   ||(pTagId->fb.type == FCSDEF_NAND7_CODE)||(pTagId->fb.type == FCSDEF_NOR7_CODE))
            {
                pAND7Var = (FCS_AND7_VARS *)pu32FbSrc;
                gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Num  = pAND7Var->numIn;

				for( i=0; i<7; i++)
				{
					if( (pAND7Var->in[i].addrOrVal.tagId.io.type != FCSDEF_REM_BLK_CODE)||(pAND7Var->in[i].addrOrVal.tagId.io.type != FCSDEF_VAR_CODE) )
					{
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8PageId = u8PageNo;
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8TagId  = pAND7Var->in[i].addrOrVal.tagId.io.id-1;
					}
					else
					{
						pRemBlkVar = (FCS_REM_BLK_VARS *)gFCSLogicData.u32pBlk[u8PageNo][pAND7Var->in[i].addrOrVal.tagId.io.id - 1];
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8PageId = (pRemBlkVar->obj.fb.pageId-1);
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8TagId  = (pRemBlkVar->obj.fb.id-1);
					
					}
					
                    ClearWatchdog();
				}
            }
            else if( (pTagId->fb.type == FCSDEF_AND8_CODE)||(pTagId->fb.type == FCSDEF_OR8_CODE )
                   ||(pTagId->fb.type == FCSDEF_NAND8_CODE)||(pTagId->fb.type == FCSDEF_NOR8_CODE))
            {
                pAND8Var = (FCS_AND8_VARS *)pu32FbSrc;
                gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Num  = pAND8Var->numIn;

				for( i=0; i<8; i++)
				{
					if( (pAND8Var->in[i].addrOrVal.tagId.io.type != FCSDEF_REM_BLK_CODE)||(pAND8Var->in[i].addrOrVal.tagId.io.type != FCSDEF_VAR_CODE) )
					{
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8PageId = u8PageNo;
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8TagId  = pAND8Var->in[i].addrOrVal.tagId.io.id-1;
					}
					else
					{
						pRemBlkVar = (FCS_REM_BLK_VARS *)gFCSLogicData.u32pBlk[u8PageNo][pAND8Var->in[i].addrOrVal.tagId.io.id - 1];
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8PageId = (pRemBlkVar->obj.fb.pageId-1);
						gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[i].u8TagId  = (pRemBlkVar->obj.fb.id-1);
					}
					
                    ClearWatchdog();
				}
            }
            else if( (pTagId->fb.type == FCSDEF_NOT_CODE)||(pTagId->fb.type == FCSDEF_RISINGEDGE_CODE)||(pTagId->fb.type == FCSDEF_FALLINGEDGE_CODE)
                   ||(pTagId->fb.type == FCSDEF_ONTIMER_CODE)||(pTagId->fb.type == FCSDEF_OFFTIMER_CODE)||(pTagId->fb.type == FCSDEF_ONESHOTTIMER_CODE))
            {
                pNotVar = (FCS_NOT_VARS *)pu32FbSrc;
                gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Num  = pNotVar->numIn;
                if( (pNotVar->in.addrOrVal.tagId.io.type != FCSDEF_REM_BLK_CODE)||(pNotVar->in.addrOrVal.tagId.io.type != FCSDEF_VAR_CODE) )
                {
                    gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[0].u8PageId = u8PageNo;
                    gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[0].u8TagId  = pNotVar->in.addrOrVal.tagId.io.id-1;
                }
                else
                {
                    pRemBlkVar = (FCS_REM_BLK_VARS *)gFCSLogicData.u32pBlk[u8PageNo][pNotVar->in.addrOrVal.tagId.io.id - 1];
                    gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[0].u8PageId = (pRemBlkVar->obj.fb.pageId-1);
                    gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[0].u8TagId  = pRemBlkVar->obj.fb.id-1;
                
                }
            } 
//			else	AT91F_printf("      not type %d    ", pTagId->fb.type);
        }
        
        ClearWatchdog();
#if (FCS_FB_DEBUG)
    printf("\n [%d][%d] %d, %d", u8PageNo,pTagId->fb.id - 1, gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8Tag_Info.u8PageId, gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8Tag_Info.u8TagId);                    
    printf("\n fb id           %d", gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8FbId);                    
    printf("\n io id           %d", gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8IOId);                    
    printf("\n in num          %d", gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Num);                    
    printf("\n in 0info Pageid %d", gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[0].u8PageId);                    
    printf("\n in 0info tagid  %d", gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[0].u8TagId);                    
    printf("\n in 1info Pageid %d", gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[1].u8PageId);                    
    printf("\n in 1info tagid  %d", gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[1].u8TagId);                    
    printf("\n in 2info Pageid %d", gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[2].u8PageId);                    
    printf("\n in 2info tagid  %d", gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[2].u8TagId);                    
    printf("\n in 3info Pageid %d", gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[3].u8PageId);                    
    printf("\n in 3info tagid  %d", gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[3].u8TagId);                    
    printf("\n in 4info Pageid %d", gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[4].u8PageId);                    
    printf("\n in 4info tagid  %d \n", gFCSInterlockData[u8PageNo][pTagId->fb.id - 1].u8In_Info[4].u8TagId);                    
#endif
	}

}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

S8 FCS_InterlockDevSearch(uint32_t u32DevIdx, uint32_t *pi, uint32_t *pj)
{
    uint32_t i=0, j=0;

    for( i=0; i<u8FCSInterlockPageCnt; i++)
    {
        for( j=0; j<FCS_MAX_NUM_BLOCK_IN_1PAGE; j++)
        {
            if( (gFCSInterlockData[i][j].u8FbId == FCSDEF_IO_CODE)&&(gFCSInterlockData[i][j].u8IOId == u32DevIdx)&&(gFCSInterlockData[i][j].u8In_Num != 0) )
            {
                *pi = i;
                *pj = j;
                
                #if (FCS_FB_DEBUG)
                printf("\n [%d][%d] IOId : %d", i, j, gFCSInterlockData[i][j].u8IOId);
                #endif
                return (1);
            }
        }
    }

    return (-1);
}

void FCS_InterlockDevDataSave(void)
{
    uint32_t i, j, k, u32Idx_x, u32Idx_y;
    unsigned char  u8PageId, u8TagId, u8Dest;
    S8  s8Result;

    u8PageId = 0;
    u8TagId  = 0;

    for( i=0; i<ELMT_COMM_LOGICAL_MAX; i++)
    {
        s8Result = FCS_InterlockDevSearch(i, &u32Idx_x, &u32Idx_y);

        u8FCSInterlockDevCnt[i] = 0;

        if( s8Result > 0 )
        {
            memcpy( &gFCSInterlockDev[i][0], &gFCSInterlockData[u32Idx_x][u32Idx_y], sizeof(FCS_INTERLOCK_DATA_STRUCT)); 
        }
        else
        {
            continue;
        }

        u8Dest   = 0;

        for( j=1; j<FCS_INTERLOCK_DEV_LINE_X*FCS_INTERLOCK_DEV_LINE_Y;)
        {
            if( u8Dest >= ((FCS_INTERLOCK_DEV_LINE_X*FCS_INTERLOCK_DEV_LINE_Y)-1) )
            {
                break;
            }
            if( gFCSInterlockDev[i][u8Dest].u8In_Num == 0 )
            {
                u8Dest++;
                continue;
            }
            
            for( k=0; k<gFCSInterlockDev[i][u8Dest].u8In_Num; k++, j++)
            {
                u8PageId = gFCSInterlockDev[i][u8Dest].u8In_Info[k].u8PageId;
                u8TagId  = gFCSInterlockDev[i][u8Dest].u8In_Info[k].u8TagId;

                memcpy( &gFCSInterlockDev[i][j], &gFCSInterlockData[u8PageId][u8TagId], sizeof(FCS_INTERLOCK_DATA_STRUCT)); 

                gFCSInterlockDev[i][j].u8Out_Idx        = u8Dest;
                gFCSInterlockDev[i][u8Dest].u8In_Idx[k] = j;
                u8FCSInterlockDevCnt[i]++;

                #if (FCS_FB_DEBUG)
                printf("\n [%d][%d] Ininfo [%d][%d] ", i, j, u8PageId, u8TagId);   
                printf("\n          Out %d - [%d][%d].%d In %d ", gFCSInterlockDev[i][j].u8Out_Idx, i, u8Dest, k, gFCSInterlockDev[i][u8Dest].u8In_Idx[k]);   
                #endif
            }

            u8Dest++;
        }
    }
}

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void FCS_InterlockDevOutPosSave(uint32_t i, uint32_t j, unsigned char x, unsigned char y)
{
    uint32_t k, idx_i, idx_j;
    unsigned char u8Pos_x, u8Pos_y;

    idx_i   = i;
    idx_j   = j;
    u8Pos_x = x;
    u8Pos_y = (y+1);
    
    for( k=0; k<FCS_INTERLOCK_DEV_LINE_X; k++)
    {
        if( gFCSInterlockDev[idx_i][gFCSInterlockDev[idx_i][idx_j].u8Out_Idx].u8Flag == false )
        {
            gFCSInterlockDev[idx_i][gFCSInterlockDev[idx_i][idx_j].u8Out_Idx].u8Tag_xy.u8PageId = u8Pos_x;
            gFCSInterlockDev[idx_i][gFCSInterlockDev[idx_i][idx_j].u8Out_Idx].u8Tag_xy.u8TagId  = u8Pos_y;
            gFCSInterlockDev[idx_i][gFCSInterlockDev[idx_i][idx_j].u8Out_Idx].u8Flag            = true;
            
            idx_j = gFCSInterlockDev[idx_i][idx_j].u8Out_Idx;
            u8Pos_y++;            
        }
    }
}

void FCS_InterlockDevPosSave(void)
{
    uint32_t i;
    S32 j;
    unsigned char  x, y;

    
    for( i=0; i<ELMT_COMM_LOGICAL_MAX; i++)
    {
        if( u8FCSInterlockDevCnt[i] != 0 )
        {
            x=0;
            y=0;
            for( j=(u8FCSInterlockDevCnt[i]); j>=0; j--)
            {
                if( gFCSInterlockDev[i][j].u8Flag == false )
                {
                    gFCSInterlockDev[i][j].u8Tag_xy.u8PageId = x;
                    gFCSInterlockDev[i][j].u8Tag_xy.u8TagId  = y;
                    gFCSInterlockDev[i][j].u8Flag            = true;

                    FCS_InterlockDevOutPosSave(i, j, x, y);
                    x++;
                }
            }
        }
    }
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void FCS_ClearAllLogicInfo(void)
{
	int i;

    memset( &gFCSLogicData         , 0, sizeof(FCS_LOGICDATA_VARS)   );
    memset( gFCSLogicExe.u8ExeOrder, 0, FCS_MAX_NUM_PAGE             );
    memset( &gFCSDevSts            , 0, sizeof(FCS_DEVICESTATUS_VAR) );

    ClearWatchdog();
    
    *(uint32_t *)gFCSLogicData.u8Blk = FCSDEF_EOT;

    u8FCSLogicRevA   = false;
    u8FCSLogicRevB   = false;

	for(i=0; i<FCS_MAX_NUM_PAGE; i++)
	{
		gFCSLogicExe.u16TimeToRun[i] = 0;
	}
    ClearWatchdog();

    for( i=0; i<MAX_DVCOPR_IN_INTLCK_LOGIC; i++)
    {
    	gu32BasicMpuOutElmtVal_LogicOut[i] = true;

        EWS_Ctrl_DevInterlockSts(i);
    }
 	gSTBFlag.bMtoSStsUpdated = true;
	gEWSFlag.bMtoHINT_Update = true;

   u8FCS_EdtiEnableFlag = false;
   gEWSFlag.bMon_Update = true;

}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

int FCS_IsAllLogicStop()
{
	if(gFCSLogicExe.u32IsRun == 0) return OK;
	return ERROR;
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void FCS_SetAllLogicToStop(void)
{
	FCS_PAGE_INFO_VARS *pi;
	int i;

    ClearWatchdog();

	for(i=0; i<FCS_MAX_NUM_PAGE; i++)
	{
		pi = (FCS_PAGE_INFO_VARS *)gFCSLogicData.u32pPageInfo[i];
		if( pi != NULL )
		{
			pi->runStopFlag = STOP;
		}
	}
	
    ClearWatchdog();
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void FCS_SetAllLogicToRun(void)
{
	FCS_PAGE_INFO_VARS *pi;
	int i;

	for(i=0; i<FCS_MAX_NUM_PAGE; i++)
	{
		gFCSLogicExe.u16TimeToRun[i] = 1;
		pi = (FCS_PAGE_INFO_VARS *)gFCSLogicData.u32pPageInfo[i];
		if(0 != pi)
		{
			pi->runStopFlag = RUN;
		}
	}
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

int FCS_TransInInfoToAddr(unsigned char u8PageNo)
{
	FCS_FB_HEADER_STRUCT *fbHead;
	FCS_IN_INFO *inInfo;
	int i, j;

	for(i=0; i<FCS_MAX_NUM_BLOCK_IN_1PAGE; i++)
	{
		// point값이 없다면 해당되는 block이 없다는 뜻
		if(gFCSLogicData.u32pBlkSeq[u8PageNo][i] == 0) continue;
		
		fbHead = (FCS_FB_HEADER_STRUCT *)gFCSLogicData.u32pBlkSeq[u8PageNo][i];
		
		for(j=0; j<fbHead->numIn; j++)
		{

		    if( fbHead->numOut == 0 )
		    {
    			inInfo = (FCS_IN_INFO *)((char *)fbHead + sizeof(FCS_FB_HEADER_STRUCT) + 
				      4 + j * sizeof(FCS_IN_INFO));
		    }
		    else
		    {
			inInfo = (FCS_IN_INFO *)((char *)fbHead + sizeof(FCS_FB_HEADER_STRUCT) + 
				      fbHead->numOut * 4 + j * sizeof(FCS_IN_INFO));
		    }

			if(inInfo->inType == FCSDEF_BLOCK_TYPE)
			{
				inInfo->addrOrVal.addr = (int)(gFCSLogicData.u32pBlk[u8PageNo][inInfo->addrOrVal.tagId.fb.id - 1] + 
						                  sizeof(FCS_FB_HEADER_STRUCT) + inInfo->offset * 4);		    // offset은 0부터
			}
		}
	}
	return OK;
}


/******************************************************************************
 **                                                                          **
 **  I/O 설정, Variables, Logic 설정 정보 Read                               **
 **                                                                          **
 ******************************************************************************/

void FCS_ReadConfigData() 
{
    uint32_t i;
    uint32_t u32Offset, u32Length;
    uint32_t *pu32Mark;

    u32Offset = 0;
    
	// read io configuration
	gFCSConfigData.u32IoConfigSize = FCS_PortedReadIoConfig(gFCSConfigData.u8IoBuff);
	// read var configuration
	memset(gFCSConfigData.u8VarBuff, 0, 4);             // Var 개수에 해당되는 내용만 0으로 한다.
	gFCSConfigData.u32VarDataSize = FCS_PortedReadVarData(gFCSConfigData.u8VarBuff);

	gFCSConfigData.u32LifDataSize = FCS_PortedReadLifConfig(gFCSConfigData.u8LifBuff);

	gFCSConfigData.u32LogicConfigSize = 0;
	
    ClearWatchdog();

	for(i=0; i<FCS_MAX_NUM_PAGE; i++)
	{
		u32Length  = FCS_PortedReadLogicConfig(i, gFCSConfigData.u8LogicBuff + u32Offset);
	    ClearWatchdog();

		if(u32Length == 0) continue;
		u32Offset += u32Length;
		
		pu32Mark   = (uint32_t *)&gFCSConfigData.u8LogicBuff[u32Offset];
		*pu32Mark  = FCSDEF_EO_LOOP;

		u32Offset += 4;
	}
	    
	pu32Mark   = (uint32_t *)&gFCSConfigData.u8LogicBuff[u32Offset];
	*pu32Mark  = FCSDEF_EOT;
	u32Offset += 4;
    ClearWatchdog();
	
	gFCSConfigData.u32LogicConfigSize = u32Offset;
}


/******************************************************************************
 **                                                                          **
 **  I/O 설정, Variables, Logic 설정 정보를 메모리로 올림                    **
 **                                                                          **
 ******************************************************************************/

void FCS_LoadConfigData()
{
    uint32_t u32Offset = 0;

	// load io configuration
	gFCSIOData.pIODataHead = (FCS_IO_CONFIG_HEAD *)gFCSConfigData.u8IoBuff;
	
    ClearWatchdog();

	if(strcmp((char *)gFCSIOData.pIODataHead->u8Title, FCS_IO_CONFIG_TITLE) == 0)
	{
		u32Offset = sizeof(FCS_IO_CONFIG_HEAD);
		memcpy(gFCSIOData.u8IoConfig, gFCSConfigData.u8IoBuff + u32Offset, gFCSConfigData.u32IoConfigSize - u32Offset);
		gFCSDevSts.ioConfigOk = true;
	}

	// load gvar data
	gFCSVarData.pVarConfigHead = (FCS_IO_CONFIG_HEAD *)gFCSConfigData.u8VarBuff;
	if(strcmp((char *)gFCSVarData.pVarConfigHead->u8Title, FCS_VAR_TITLE) == 0)
	{
		u32Offset = sizeof(FCS_IO_CONFIG_HEAD);
		memcpy(gFCSVarData.u8VarData, gFCSConfigData.u8VarBuff + u32Offset, gFCSConfigData.u32VarDataSize - u32Offset);
		gFCSDevSts.varConfigOk = true;
	}

	// load function block configuration
	memcpy( gFCSLogicData.u8Blk, gFCSConfigData.u8LogicBuff, gFCSConfigData.u32LogicConfigSize);

    ClearWatchdog();

	gFCSDevSts.logicConfigOk = true;
}


/******************************************************************************
 **                                                                          **
 **  로드된 로직을 수행시킬 수 있는 전단계로 준비                            **
 **                                                                          **
 ******************************************************************************/

int FCS_SetLogicToBeReady(void)
{
	unsigned char  u8PageNo;
	uint32_t u32Seq;
	uint32_t * pu32FbSrc;
	uint32_t i, j, u32TmpVal1, u32TmpVal2;
	FCS_PAGE_INFO_VARS *pPageInfo;
	FCS_TAG_ID_STRUCT  *pTagId;

	pu32FbSrc = (uint32_t *)gFCSLogicData.u8Blk;
	pPageInfo = (FCS_PAGE_INFO_VARS *)pu32FbSrc;

    ClearWatchdog();

	u32Seq    = 0;
    u8FCSInterlockPageCnt = 0;
    
	if(gFCSDevSts.ioConfigOk)
	{
		// I/O data 저장 위치 할당
		gFCSIOData.u32BIPointNum       = gFCSIOData.pIODataHead->u16NumofPoint[0];   // 실제 BI 포인트 수
		gFCSIOData.u32BOPointNum       = gFCSIOData.pIODataHead->u16NumofPoint[1];   // 실제 BO 포인트 수
		gFCSIOData.u32AIPointNum       = gFCSIOData.pIODataHead->u16NumofPoint[2];   // 실제 AI 포인트 수
		gFCSIOData.u32AOPointNum       = gFCSIOData.pIODataHead->u16NumofPoint[3];   // 실제 AO 포인트 수

        // I/O data의 Offset 설정
        gFCSIOData.u32BIPointOffset    = 0;
        gFCSIOData.u32BOPointOffset    = gFCSIOData.u32BIPointOffset + (gFCSIOData.u32BIPointNum/8);

        u32TmpVal1 = gFCSIOData.u32BOPointOffset %4;
        u32TmpVal2 = gFCSIOData.u32BIPointNum %8;
        if     ( u32TmpVal1 ) gFCSIOData.u32BOPointOffset = gFCSIOData.u32BOPointOffset + 4 - u32TmpVal1;
        else if( u32TmpVal2 ) gFCSIOData.u32BOPointOffset = gFCSIOData.u32BOPointOffset + 4;

        gFCSIOData.u32AIPointOffset    = gFCSIOData.u32BOPointOffset + (gFCSIOData.u32BOPointNum/8);
        
        u32TmpVal1 = gFCSIOData.u32AIPointOffset %4;
        u32TmpVal2 = gFCSIOData.u32BOPointNum %8;
        if     ( u32TmpVal1 ) gFCSIOData.u32AIPointOffset = gFCSIOData.u32AIPointOffset + 4 - u32TmpVal1;
        else if( u32TmpVal2 ) gFCSIOData.u32AIPointOffset = gFCSIOData.u32AIPointOffset + 4;
        
        
        gFCSIOData.u32AOPointOffset    = gFCSIOData.u32AIPointOffset + (gFCSIOData.u32AIPointNum*2);
        gFCSIOData.u32IODataSize       = gFCSIOData.u32AOPointOffset + (gFCSIOData.u32AOPointNum*2);

        gFCSIOData.u32AIEngPointOffset = 0;
        gFCSIOData.u32AOEngPointOffset = gFCSIOData.u32AIEngPointOffset + (gFCSIOData.u32AIPointNum*4);
        gFCSIOData.u32IOEngDataSize    = gFCSIOData.u32AOEngPointOffset + (gFCSIOData.u32AOPointNum*4);

        gFCSIOData.pbBIData            = (unsigned char  *) gFCSIOData.u8IoData + gFCSIOData.u32BIPointOffset;
        gFCSIOData.pbBOData            = (unsigned char  *) gFCSIOData.u8IoData + gFCSIOData.u32BOPointOffset;
        gFCSIOData.pwAIData            = (u16int_t *) gFCSIOData.u8IoData + gFCSIOData.u32AIPointOffset;
        gFCSIOData.pwAOData            = (u16int_t *) gFCSIOData.u8IoData + gFCSIOData.u32AOPointOffset;
        
        gFCSIOData.pfAIEngData         = (float *)gFCSIOData.u8AIoEngData + gFCSIOData.u32AIEngPointOffset;
        gFCSIOData.pfAOEngData         = (float *)gFCSIOData.u8AIoEngData + gFCSIOData.u32AOEngPointOffset;

        ClearWatchdog();

		// Engineering data processing을 위해서...
		for(i=0; i<gFCSIOData.u32BIPointNum; i++)
		{
			gFCSIOData.u32PointingIoConfig[0][i] = (int)gFCSIOData.u8IoConfig + sizeof(gFCSIOData.pIODataHead) + gFCSIOData.pIODataHead->u32Offset[0] + sizeof(FCS_BI_VARS) * i;
            ClearWatchdog();
		}
		for(i=0; i<gFCSIOData.u32BOPointNum; i++)
		{
			gFCSIOData.u32PointingIoConfig[1][i] = (int)gFCSIOData.u8IoConfig + sizeof(gFCSIOData.pIODataHead) + gFCSIOData.pIODataHead->u32Offset[1] + sizeof(FCS_BO_VARS) * i;
            ClearWatchdog();
		}
		for(i=0; i<gFCSIOData.u32AIPointNum; i++)
		{
			gFCSIOData.u32PointingIoConfig[2][i] = (int)gFCSIOData.u8IoConfig + sizeof(gFCSIOData.pIODataHead) + gFCSIOData.pIODataHead->u32Offset[2] + sizeof(FCS_AI_VARS) * i;
            ClearWatchdog();
		}
		for(i=0; i<gFCSIOData.u32AOPointNum; i++)
		{
			gFCSIOData.u32PointingIoConfig[3][i] = (int)gFCSIOData.u8IoConfig + sizeof(gFCSIOData.pIODataHead) + gFCSIOData.pIODataHead->u32Offset[3] + sizeof(FCS_AO_VARS) * i;
            ClearWatchdog();
		}
	}

	if(gFCSDevSts.varConfigOk)
	{
	    gFCSVarData.u32BVarPoint   = gFCSVarData.pVarConfigHead->u16NumofPoint[0];
	    gFCSVarData.u32IVarPoint   = gFCSVarData.pVarConfigHead->u16NumofPoint[1];
	    gFCSVarData.u32FVarPoint   = gFCSVarData.pVarConfigHead->u16NumofPoint[2];

	    gFCSVarData.u32BVarOffset  = gFCSVarData.pVarConfigHead->u32Offset[0];
	    gFCSVarData.u32IVarOffset  = gFCSVarData.pVarConfigHead->u32Offset[2];
	    gFCSVarData.u32FVarOffset  = gFCSVarData.pVarConfigHead->u32Offset[3];

	    gFCSVarData.u32VarDataSize = gFCSVarData.u32FVarOffset + (gFCSVarData.u32FVarPoint*4);

	    gFCSVarData.pBVarData      = (unsigned char    *) (gFCSVarData.u8VarData + gFCSVarData.u32BVarPoint);
	    gFCSVarData.pIVarData      = (uint32_t   *) (gFCSVarData.u8VarData + gFCSVarData.u32IVarPoint);
	    gFCSVarData.pFVarData      = (float *) (gFCSVarData.u8VarData + gFCSVarData.u32FVarPoint);
	}

    ClearWatchdog();

	if(gFCSDevSts.logicConfigOk)
	{
	    memset( &gFCSInterlockData[0][0], 0, sizeof(FCS_INTERLOCK_DATA_STRUCT)*FCS_MAX_NUM_PAGE*FCS_MAX_NUM_BLOCK_IN_1PAGE );
        memset( &gFCSInterlockDev[0][0], 0, sizeof(FCS_INTERLOCK_DATA_STRUCT)*ELMT_COMM_LOGICAL_MAX*FCS_INTERLOCK_DEV_LINE_X*FCS_INTERLOCK_DEV_LINE_Y);

	    for( i=0; i<FCS_MAX_SIZE_FB_CONFIG; i++)
	    {
	        if( *pu32FbSrc == FCSDEF_EOT )     break;

	        pPageInfo = (FCS_PAGE_INFO_VARS *) pu32FbSrc;

			// page info data의 건전성 체크
			if( pPageInfo->pageId == 0     || pPageInfo->pageId > FCS_MAX_NUM_PAGE    || pPageInfo->exeOrder == 0   || 
			    pPageInfo->exeOrder > FCS_MAX_NUM_PAGE  || pPageInfo->runStopFlag > 1)
			{
			    #if (FCS_FB_DEBUG)
                AT91F_printf("\n Page Info Data Error");
                #endif
				gFCSDevSts.logicConfigOk = false;
				FCS_ClearAllLogicInfo();

				return ERROR;
			}

            u8FCSInterlockPageCnt++;
			u8PageNo = pPageInfo->pageId - 1;
			pPageInfo->runStopFlag = STOP;

			gFCSLogicExe.u8ExeOrder[pPageInfo->exeOrder - 1] = pPageInfo->pageId;
	        gFCSLogicData.u32pPageInfo[u8PageNo]             = (uint32_t)pu32FbSrc;

	        pu32FbSrc = (uint32_t *)((unsigned char *)pu32FbSrc + (unsigned char)sizeof(FCS_PAGE_INFO_VARS));
            
            ClearWatchdog();

	        for( j=0; j<FCS_MAX_SIZE_FB_CONFIG; j++)
	        {
    	        if( *pu32FbSrc == FCSDEF_EO_LOOP )     break;

 				pTagId = (FCS_TAG_ID_STRUCT *)pu32FbSrc;
 				
				if(u32FCSSizeofFB[pTagId->fb.type] == 0)  // Error
				{
    			    #if (FCS_FB_DEBUG)
                    AT91F_printf("\n FB type %d Null", pTagId->fb.type);
                    #endif
					gFCSDevSts.logicConfigOk = false;
					FCS_ClearAllLogicInfo();

					return ERROR;
				}

				gFCSLogicData.u32pBlkSeq[u8PageNo][u32Seq++]       = (uint32_t)pu32FbSrc;
				gFCSLogicData.u32pBlk[u8PageNo][pTagId->fb.id - 1] = (uint32_t)pu32FbSrc;

                ClearWatchdog();

         		FCS_InterlockDataSave(u8PageNo, pu32FbSrc);

				pu32FbSrc = (uint32_t *)((unsigned char *)pu32FbSrc + (char)u32FCSSizeofFB[pTagId->fb.type]);

	        }

			FCS_TransInInfoToAddr(u8PageNo);
			pu32FbSrc++;
			u32Seq = 0;
		}

        FCS_InterlockDevDataSave();
        FCS_InterlockDevPosSave();
	}
	
	if(pu32FbSrc != (uint32_t *)gFCSLogicData.u8Blk)             // 한개의 페이지라도 등록되어 있다면
	{
		u8FCSLogicRevA = pPageInfo->RevId.a;
		u8FCSLogicRevB = pPageInfo->RevId.b;

		FCS_SetAllLogicToRun();
		gFCSDevSts.logicConfigOk = true;
	}
	else
	{
		u8FCSLogicRevA           = 0;
		u8FCSLogicRevB           = 0;
		gFCSDevSts.logicConfigOk = false;
	}

	return OK;
}


/******************************************************************************
 **                                                                          **
 **   Logic Revision id 저장                                                 **
 **                                                                          **
 ******************************************************************************/

void FCS_SetLogicRevId(void)
{
	unsigned char * data;
	
    if ( ghEWS_PortDirection == EWSDEFS_PORT_SERIAL   )     data = &gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_DATA];
    else                                                    data = (unsigned char *)&ghEth.cRxBuffer[EWSDEFS_COMM_DATA];

	memcpy(&gFCSConfigInfo, data, sizeof(FCS_CONFIG_INFO_VARS));
	
	u8FCSLogicRevA = gFCSConfigInfo.ab.a;
	u8FCSLogicRevB = gFCSConfigInfo.ab.b;

	if( (gFCSConfigInfo.numLogicPage == 0)&&(gFCSDevSts.logicConfigOk == 1) )
	{
	    FCS_RemoveAllSavedLogicConfig();
    	FCS_ClearAllLogicInfo();
    	FCS_SetAllLogicToStop();
    	memset( &gFCSConfigData, 0, sizeof(FCS_CONFIGDATA_VARS) );
	    memset( &gFCSInterlockData[0][0], 0, sizeof(FCS_INTERLOCK_DATA_STRUCT)*FCS_MAX_NUM_PAGE*FCS_MAX_NUM_BLOCK_IN_1PAGE );
        memset( &gFCSInterlockDev[0][0], 0, sizeof(FCS_INTERLOCK_DATA_STRUCT)*ELMT_COMM_LOGICAL_MAX*FCS_INTERLOCK_DEV_LINE_X*FCS_INTERLOCK_DEV_LINE_Y);
        memset( &u8FCSInterlockDevCnt[0], 0, ELMT_COMM_LOGICAL_MAX);

        SetDnpCRC( (unsigned char *)&pNVRAM->NVFCSLogicInfo, (u16int_t) sizeof(IED_FCS_LOGIC_INFO) );
		ClearWatchdog();	

	}

	FCS_Tx_Msg( 0,SET_LOGIC_REV_ID, ACK_OK, PACKET_ONE_TIME, NULL, 0, 0);
}


/******************************************************************************
 **                                                                          **
 **   Logic Revision id 전송                                                 **
 **                                                                          **
 ******************************************************************************/

void FCS_GetLogicRevId(void)
{
	unsigned char data[4] = {0,0,0,0};

	data[0] = u8FCSLogicRevA;
	data[1] = u8FCSLogicRevB;

	FCS_Tx_Msg( 0,GET_LOGIC_REV_ID, ACK_OK, PACKET_ONE_TIME, data, 4, 0);
}


/******************************************************************************
 **                                                                          **
 **   IED로 IO 정보 저장                                                     **
 **                                                                          **
 ******************************************************************************/

void FCS_UploadIoConfig(void)
{
	static uint32_t configSize = 0;

	unsigned char   ctrl     = PACKET_ONE_TIME, seq = 0;
    uint32_t  dataSize = 0;
    unsigned char * data;
    
    if( ghEWS_PortDirection == EWSDEFS_PORT_SERIAL )
    {
        data     = &gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_DATA];
        dataSize =  gdsEWS_Comm_Msg.u16DataSize;
        ctrl     =  gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_CTRL];
        seq      =  gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_SEQ];
    }
    else
    {
        data     = (unsigned char *)&ghEth.cRxBuffer[EWSDEFS_COMM_DATA];
        dataSize =  ghEth.u16Length;
        ctrl     =  ghEth.cRxBuffer[EWSDEFS_COMM_CTRL];
        seq      =  ghEth.cRxBuffer[EWSDEFS_COMM_SEQ];
    }

    switch( ctrl )
	{
	case PACKET_FIRST:
		memcpy(gFCSConfigData.u8IoBuff, data, dataSize);
		configSize = dataSize;
		seq++;
		goto REPLY;
	case PACKET_ING:
		memcpy(gFCSConfigData.u8IoBuff + configSize, data, dataSize);
		configSize += dataSize;
		seq++;
		goto REPLY;
	case PACKET_END:
		memcpy(gFCSConfigData.u8IoBuff + configSize, data, dataSize);
		configSize += dataSize;
		seq++;
		break;
	case PACKET_ONE_TIME:
		memcpy(gFCSConfigData.u8IoBuff, data, dataSize);
		configSize = dataSize;
		break;
	}

	// 비휘발성 메모리에 기록한다.
	FCS_SaveIoConfig(configSize);
	configSize = 0;

REPLY :
	FCS_Tx_Msg( seq,UPLOAD_IO_CONF, ACK_OK, PACKET_ONE_TIME, NULL, 0, 0);
}


/******************************************************************************
 **                                                                          **
 **  EWS로 IO 전송                                                           **
 **                                                                          **
 ******************************************************************************/

void FCS_DownloadIoConfig(void)
{
    if( gFCSConfigData.u32IoConfigSize != 0 )
    {
    	FCS_Tx_Msg( 0,DOWNLOAD_IO_CONF, ACK_OK, PACKET_ONE_TIME, &gFCSConfigData.u8IoBuff, gFCSConfigData.u32IoConfigSize, 0);
    }
    else
    {
    	FCS_Tx_Msg( 0,DOWNLOAD_IO_CONF, ACK_OK, PACKET_ONE_TIME, NULL, 0, 0);
    }
}


/******************************************************************************
 **                                                                          **
 **   IED로 Var 정보 저장                                                    **
 **                                                                          **
 ******************************************************************************/

void FCS_UploadVarConfig(void)
{
	static int configSize = 0;
	unsigned char   ctrl     = PACKET_ONE_TIME, seq = 0;
    uint32_t  dataSize = 0;
    unsigned char * data;

    if( ghEWS_PortDirection == EWSDEFS_PORT_SERIAL )
    {
        data     = &gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_DATA];
        dataSize =  gdsEWS_Comm_Msg.u16DataSize;
        ctrl     =  gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_CTRL];
        seq      =  gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_SEQ];
    }
    else
    {
        data     = (unsigned char *)&ghEth.cRxBuffer[EWSDEFS_COMM_DATA];
        dataSize =  ghEth.u16Length;
        ctrl     =  ghEth.cRxBuffer[EWSDEFS_COMM_CTRL];
        seq      =  ghEth.cRxBuffer[EWSDEFS_COMM_SEQ];
    }

    switch(ctrl)
	{
	case PACKET_FIRST:
		memcpy(gFCSConfigData.u8VarBuff, data, dataSize);
		configSize = dataSize;
		seq++;
		goto REPLY;
	case PACKET_ING:
		memcpy(gFCSConfigData.u8VarBuff + configSize, data, dataSize);
		configSize += dataSize;
		seq++;
		goto REPLY;
	case PACKET_END:
		memcpy(gFCSConfigData.u8VarBuff + configSize, data, dataSize);
		configSize += dataSize;
		seq++;
		break;
	case PACKET_ONE_TIME:
		memcpy(gFCSConfigData.u8VarBuff, data, dataSize);
		configSize = dataSize;
		break;
	}

	// 비휘발성 메모리에 기록한다.
	FCS_SaveVarData(configSize);
	configSize = 0;

REPLY :
	FCS_Tx_Msg( seq,UPLOAD_VAR_CONF, ACK_OK, PACKET_ONE_TIME, NULL, 0, 0);
}


/******************************************************************************
 **                                                                          **
 **  EWS로 Var 전송                                                          **
 **                                                                          **
 ******************************************************************************/

void FCS_DownloadVarConfig(void)
{
    if( gFCSConfigData.u32VarDataSize != 0 )
    {
    	FCS_Tx_Msg( 0,DOWNLOAD_VAR_CONF, ACK_OK, PACKET_ONE_TIME, &gFCSConfigData.u8VarBuff, gFCSConfigData.u32VarDataSize, 0);
    }
    else
    {
    	FCS_Tx_Msg( 0,DOWNLOAD_VAR_CONF, ACK_OK, PACKET_ONE_TIME, NULL, 0, 0);
    }
}


/******************************************************************************
 **                                                                          **
 **  IED로 Logic 저장                                                        **
 **                                                                          **
 ******************************************************************************/

void FCS_UploadLogicConfig(void)
{
	static int configSize = 0;
	unsigned char   ctrl     = PACKET_ONE_TIME, seq = 0;
    uint32_t  dataSize = 0;
    unsigned char * data;

    if( ghEWS_PortDirection == EWSDEFS_PORT_SERIAL )
    {
        data     = &gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_DATA];
        dataSize =  gdsEWS_Comm_Msg.u16DataSize;
        ctrl     =  gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_CTRL];
        seq      =  gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_SEQ];
    }
    else
    {
        data     = (unsigned char *)&ghEth.cRxBuffer[EWSDEFS_COMM_DATA];
        dataSize =  ghEth.u16Length;
        ctrl     =  ghEth.cRxBuffer[EWSDEFS_COMM_CTRL];
        seq      =  ghEth.cRxBuffer[EWSDEFS_COMM_SEQ];
    }

	if(u8FCSDownPageNum == 0)
	{
	    FCS_RemoveAllSavedLogicConfig();
	}

    switch(ctrl)
	{
	case PACKET_FIRST:
		memcpy(gFCSConfigData.u8LogicBuff, data, dataSize);
		configSize = dataSize;
		seq++;
		goto REPLY;
	case PACKET_ING:
		memcpy(gFCSConfigData.u8LogicBuff + configSize, data, dataSize);
		configSize += dataSize;
		seq++;
		goto REPLY;
	case PACKET_END:
		memcpy(gFCSConfigData.u8LogicBuff + configSize, data, dataSize);
		configSize += dataSize;
		u8FCSDownPageNum++;
		seq++;
		break;
	case PACKET_ONE_TIME:
		memcpy(gFCSConfigData.u8LogicBuff, data, dataSize);
		configSize = dataSize;
		u8FCSDownPageNum++;
		break;
    default:
    	memcpy(gFCSConfigData.u8LogicBuff, data, dataSize);
		configSize = dataSize;
		u8FCSDownPageNum++;
		break;
	}

    FCS_SaveLogicConfig(configSize);
	configSize = 0;

	// 전체 업로드가 완료되면 로직을 새로 실행시킨다.
	if(u8FCSDownPageNum == gFCSConfigInfo.numLogicPage)
	{
    	FCS_ClearAllLogicInfo();
    	FCS_SetAllLogicToStop();

    	FCS_ReadConfigData();
    	FCS_LoadConfigData();

    	FCS_SetLogicToBeReady();

		u8FCSDownPageNum = 0;
		u8FCSLogicDownloadFlag = true;
        
		if(gFCSDevSts.logicConfigOk == false)
		{
			FCS_Tx_Msg( 0,UPLOAD_LOGIC_CONF, ACK_FAIL, PACKET_ONE_TIME, NULL, 0, 0);
			return;
		}
	}

REPLY :
	FCS_Tx_Msg( seq,UPLOAD_LOGIC_CONF, ACK_OK, PACKET_ONE_TIME, NULL, 0, 0);
}


/******************************************************************************
 **                                                                          **
 **  EWS로 Logic 전송                                                        **
 **                                                                          **
 ******************************************************************************/

void FCS_DownloadLogicConfig(void)
{
    if( gFCSConfigData.u32LogicConfigSize != 0 )
    {
    	FCS_Tx_Msg( 0,DOWNLOAD_LOGIC_CONF, ACK_OK, PACKET_ONE_TIME, &gFCSConfigData.u8LogicBuff, gFCSConfigData.u32LogicConfigSize, 0);
    }
    else
    {
    	FCS_Tx_Msg( 0,DOWNLOAD_LOGIC_CONF, ACK_OK, PACKET_ONE_TIME, NULL, 0, 0);
    }
}


/******************************************************************************
 **                                                                          **
 **  IED로 편집용 Logic 저장 (좌표정보 포함)                                 **
 **                                                                          **
 ******************************************************************************/

void FCS_UploadLifConfig(void)
{
	static int configSize = 0;
	unsigned char   ctrl     = PACKET_ONE_TIME;
	unsigned char   seq = 0;
    uint32_t  dataSize = 0;
    unsigned char * data;

    if( ghEWS_PortDirection == EWSDEFS_PORT_SERIAL )
    {
        data     = &gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_DATA];
        dataSize =  gdsEWS_Comm_Msg.u16DataSize;
        ctrl     =  gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_CTRL];
        seq      =  gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_SEQ];
    }
    else
    {
        data     = (unsigned char *)&ghEth.cRxBuffer[EWSDEFS_COMM_DATA];
        dataSize =  ghEth.u16Length;
        ctrl     =  ghEth.cRxBuffer[EWSDEFS_COMM_CTRL];
        seq      =  ghEth.cRxBuffer[EWSDEFS_COMM_SEQ];
    }

    switch(ctrl)
	{
	case PACKET_FIRST:
		memcpy(gFCSConfigData.u8LifBuff, data, dataSize);
		configSize = dataSize;
		seq++;
		goto REPLY;
	case PACKET_ING:
		memcpy(gFCSConfigData.u8LifBuff + configSize, data, dataSize);
		configSize += dataSize;
		seq++;
		goto REPLY;
	case PACKET_END:
		memcpy(gFCSConfigData.u8LifBuff + configSize, data, dataSize);
		configSize += dataSize;
		seq++;
		break;
	case PACKET_ONE_TIME:
		memcpy(gFCSConfigData.u8LifBuff, data, dataSize);
		configSize = dataSize;
		break;
    default:
    	memcpy(gFCSConfigData.u8LifBuff, data, dataSize);
		configSize = dataSize;
		break;
	}

    FCS_SaveLifConfig(configSize);
	configSize = 0;

REPLY :
	FCS_Tx_Msg( seq,UPLOAD_LIF_FILE, ACK_OK, PACKET_ONE_TIME, NULL, 0, 0);
}


/******************************************************************************
 **                                                                          **
 **  EWS로 편집용 Logic 전송 (좌표정보 포함)                                 **
 **                                                                          **
 ******************************************************************************/

void FCS_DownloadLifConfig(void)
{
    if( gFCSConfigData.u32LifDataSize != 0 )
    {
    	FCS_Tx_Msg( 0,DOWNLOAD_LIF_FILE, ACK_OK, PACKET_ONE_TIME, &gFCSConfigData.u8LifBuff, gFCSConfigData.u32LifDataSize, 0);
    }
    else
    {
    	FCS_Tx_Msg( 0,DOWNLOAD_LIF_FILE, ACK_OK, PACKET_ONE_TIME, NULL, 0, 0);
    }
}


/******************************************************************************
 **                                                                          **
 **   모든 Var 정보 전송                                                     **
 **                                                                          **
 ******************************************************************************/

void FCS_Svc_GetAllVarVal()
{
	FCS_SVC_VAR_DATA_HEAD *svcVarDataHead;

	svcVarDataHead             = (FCS_SVC_VAR_DATA_HEAD *)u8FCSSvcData;
	svcVarDataHead->bVarNum    = gFCSVarData.u32BVarPoint;
	svcVarDataHead->iVarNum    = gFCSVarData.u32IVarPoint;
	svcVarDataHead->fVarNum    = gFCSVarData.u32FVarPoint;
	svcVarDataHead->bVarOffset = gFCSVarData.u32BVarOffset;
	svcVarDataHead->iVarOffset = gFCSVarData.u32IVarOffset;
	svcVarDataHead->fVarOffset = gFCSVarData.u32FVarOffset;

	memcpy((u8FCSSvcData + sizeof(FCS_SVC_VAR_DATA_HEAD)), gFCSVarData.u8VarData, gFCSVarData.u32VarDataSize);

	FCS_Tx_Msg( 0,GET_ALL_VAR_VAL, ACK_OK, PACKET_ONE_TIME, u8FCSSvcData, gFCSVarData.u32VarDataSize + sizeof(FCS_SVC_VAR_DATA_HEAD), 0);
}


/******************************************************************************
 **                                                                          **
 **   Logic Pages의 현재 상태 전송                                           **
 **                                                                          **
 ******************************************************************************/

void FCS_Svc_GetPagesStatus(FCS_SVC_PAGES_STATUS *pagesSts)
{
	FCS_PAGE_INFO_VARS *pi;
	int i = 0;

	for( i=0; i<FCS_MAX_NUM_PAGE; i++)
	{
	    if( gFCSLogicData.u32pPageInfo[i] == 0 )    break;

		pi = (FCS_PAGE_INFO_VARS *)gFCSLogicData.u32pPageInfo[i];
		
		pagesSts->pageSts[i].pageId = pi->pageId;
		pagesSts->pageSts[i].runStopFlag = pi->runStopFlag;
		pagesSts->pageSts[i].runSeq = pi->exeOrder;
	    
	}

	pagesSts->pageNum = i;
}


void FCS_Svc_GetLogicPagesStatus()
{
	FCS_SVC_PAGES_STATUS pagesSts;

	FCS_Svc_GetPagesStatus(&pagesSts);

	FCS_Tx_Msg( 0,GET_LOGIC_PAGES_STATUS, ACK_OK, PACKET_ONE_TIME,	(unsigned char *)&pagesSts, sizeof(FCS_SVC_PAGES_STATUS), 0);
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void FCS_Svc_SetAllLogicPagesRunStop()
{
    unsigned char * data;

    if( ghEWS_PortDirection == EWSDEFS_PORT_SERIAL ) data = &gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_DATA];
    else                                             data = (unsigned char *)&ghEth.cRxBuffer[EWSDEFS_COMM_DATA];

	if     ( *data == RUN  )    	FCS_SetAllLogicToRun();
	else if( *data == STOP )		FCS_SetAllLogicToStop();
	else
	{
		FCS_Tx_Msg( 0,SET_ALL_LOGIC_PAGES_RUN_STOP, ACK_FAIL, PACKET_ONE_TIME, NULL, 0, 0);
		return;
	}

	FCS_Svc_GetLogicPagesStatus();
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void FCS_Svc_SetALogicPageRunStop()
{
    unsigned char                  * data;
	S8                  logicPageId;
	S8                  runStopFlag;
	FCS_PAGE_INFO_VARS  * pi;
	
    if( ghEWS_PortDirection == EWSDEFS_PORT_SERIAL ) data = &gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_DATA];
    else                                             data = (unsigned char *)&ghEth.cRxBuffer[EWSDEFS_COMM_DATA];

    logicPageId = data[0];
	runStopFlag = data[1];

	pi = (FCS_PAGE_INFO_VARS *)gFCSLogicData.u32pPageInfo[logicPageId - 1];

	if( pi != NULL )
	{
		if( runStopFlag == RUN )
		{
			pi->runStopFlag = RUN;
		}
		else if( runStopFlag == STOP )
		{
			pi->runStopFlag = STOP;
		}
		else
		{
			FCS_Tx_Msg( 0,SET_A_LOGIC_PAGE_RUN_STOP, ACK_FAIL, PACKET_ONE_TIME, NULL, 0, 0);
			return;
		}

	}
	else
	{
		FCS_Tx_Msg( 0,SET_A_LOGIC_PAGE_RUN_STOP, ACK_FAIL, PACKET_ONE_TIME, NULL, 0, 0);
		return;
	}

	FCS_Svc_GetLogicPagesStatus();
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void FCS_Svc_GetLogicPageBlksOut()
{
	unsigned char * pageId;
	unsigned char   pageNo;
	unsigned char   u8TmpSvcData[EWS_MAX_BUFFERSIZE];
	unsigned char   u8BlkNumCnt;
	uint32_t  i, j;	
	uint32_t  u32MaxBuffSize=0;

	FCS_A_PAGE_STATUS      * APageSts;
	FCS_PAGE_INFO_VARS     * pi;
	FCS_A_BLK_OUT_VAL_HEAD * ABlkOutHead;
	FCS_FB_HEADER_STRUCT   * FbHead;
	FCS_IORF               * outVal;

    if( ghEWS_PortDirection == EWSDEFS_PORT_SERIAL )
    {
        pageId = &gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_DATA];
        u32MaxBuffSize = EWS_MAX_BUFFERSIZE;
    }
    else
    {
        pageId = (unsigned char *)&ghEth.cRxBuffer[EWSDEFS_COMM_DATA];
        u32MaxBuffSize = TX_RX_MAX_BUF_SIZE;
    }

    ABlkOutHead = (FCS_A_BLK_OUT_VAL_HEAD *)u8TmpSvcData;

    for( i=0; i<u32MaxBuffSize; i++)
    {
        if( pageId[i] == 0 )    break;

		pageNo = pageId[i] - 1;
		pi     = (FCS_PAGE_INFO_VARS *)gFCSLogicData.u32pPageInfo[pageNo];
		
		// 한 페이지 상태 
		APageSts              = (FCS_A_PAGE_STATUS *)ABlkOutHead; 
		APageSts->pageId      = pi->pageId;
		APageSts->runStopFlag = pi->runStopFlag;
		APageSts->runSeq      = pi->exeOrder;
//AT91F_printf("\n PageId %d", APageSts->pageId);
		ABlkOutHead           = (FCS_A_BLK_OUT_VAL_HEAD *) (APageSts + 1);

        u8BlkNumCnt = 0;
        
		// 페이지 내 블럭값
		// 페이지내 블럭들의 블럭헤더 + 블럭값 구성으로 저장.
		for(j=0; j<FCS_MAX_NUM_BLOCK_IN_1PAGE; j++)
		{
			/* if there's any null ptr in the table then skip */
			if(gFCSLogicData.u32pBlk[pageNo][j] == (int)NULL)
			{
				continue;
			}

			FbHead = (FCS_FB_HEADER_STRUCT *)gFCSLogicData.u32pBlk[pageNo][j];

			if( FbHead->numOut == 0 )
			{
			    continue;
			}

			ABlkOutHead->blkId       = j+1;
			ABlkOutHead->lockMaskVal = FbHead->lockMask;
			ABlkOutHead->outNum      = FbHead->numOut;
			
			outVal = (FCS_IORF *) (ABlkOutHead + 1);

			// Function Block Header 바로 다음에 있는 것이 output이다.
			u8BlkNumCnt++;
			memcpy((char *)outVal, (char *)(FbHead + 1), sizeof(FCS_IORF) * FbHead->numOut);
			ABlkOutHead = (FCS_A_BLK_OUT_VAL_HEAD *)((char *)outVal + sizeof(FCS_IORF) * FbHead->numOut);
		}
		
		APageSts->blkNum = u8BlkNumCnt;		
//AT91F_printf("\n blkNum %d %d", APageSts->blkNum, u8BlkNumCnt);

    }

	FCS_Tx_Msg( 0,GET_LOGIC_PAGE_BLKS_OUT, ACK_OK, PACKET_ONE_TIME, (unsigned char *)u8TmpSvcData, (u16int_t)((char *)ABlkOutHead - (char *)u8TmpSvcData), 0);

}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void FCS_Svc_GetAllIoVal(void)
{
	FCS_SVC_IO_DATA_HEAD *svcIoDataHead;
	char *tmpPos;

	svcIoDataHead              = (FCS_SVC_IO_DATA_HEAD *)u8FCSSvcData;
	svcIoDataHead->biNum       = gFCSIOData.u32BIPointNum;
	svcIoDataHead->boNum       = gFCSIOData.u32BOPointNum;
	svcIoDataHead->aiNum       = gFCSIOData.u32AIPointNum;
	svcIoDataHead->aoNum       = gFCSIOData.u32AOPointNum;
	svcIoDataHead->biOffset    = gFCSIOData.u32BIPointOffset;
	svcIoDataHead->boOffset    = gFCSIOData.u32BOPointOffset;
	svcIoDataHead->aiOffset    = gFCSIOData.u32AIPointOffset;
	svcIoDataHead->aoOffset    = gFCSIOData.u32AOPointOffset;
	svcIoDataHead->aiEngOffset = gFCSIOData.u32AIEngPointOffset;
	svcIoDataHead->aoEngOffset = gFCSIOData.u32AOEngPointOffset;
//	svcIoDataHead->aiEngOffset = gIoRawDataSize + gAInDataEngOffset;
//	svcIoDataHead->aoEngOffset = gIoRawDataSize + gAOutDataEngOffset;

	tmpPos = (char *)(u8FCSSvcData + sizeof(FCS_SVC_IO_DATA_HEAD));
	memcpy( tmpPos, gFCSIOData.u8IoData, gFCSIOData.u32IODataSize );
	tmpPos += gFCSIOData.u32IODataSize;
	memcpy(tmpPos, gFCSIOData.u8AIoEngData, gFCSIOData.u32IOEngDataSize);

	FCS_Tx_Msg( 0,GET_ALL_IO_VAL, ACK_OK, PACKET_ONE_TIME, u8FCSSvcData, sizeof(FCS_SVC_IO_DATA_HEAD)+ gFCSIOData.u32IODataSize + gFCSIOData.u32IOEngDataSize, 0);
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void FCS_Svc_SetOutVal()
{
	FCS_SET_A_BLK_OUTVAL *reqSetBlkOut;

    if( ghEWS_PortDirection == EWSDEFS_PORT_SERIAL ) reqSetBlkOut = (FCS_SET_A_BLK_OUTVAL *)&gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_DATA];
    else                                             reqSetBlkOut = (FCS_SET_A_BLK_OUTVAL *)&ghEth.cRxBuffer[EWSDEFS_COMM_DATA];

	FCS_writeABlkOut(reqSetBlkOut->tagId, reqSetBlkOut->outNo, reqSetBlkOut->lockMaskVal, reqSetBlkOut->val);
	
	FCS_Tx_Msg( 0,SET_OUT_VAL, ACK_OK, PACKET_ONE_TIME, NULL, 0, 0);	
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void FCS_Svc_SetABlkParasVal()
{
	int i;

	FCS_TAG_ID_STRUCT * tagId;
	unsigned char                * setNum;
	FCS_SET_PARA_INFO * setParInfo;
	
    if( ghEWS_PortDirection == EWSDEFS_PORT_SERIAL ) tagId = (FCS_TAG_ID_STRUCT *)&gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_DATA];
    else                                             tagId = (FCS_TAG_ID_STRUCT *)&ghEth.cRxBuffer[EWSDEFS_COMM_DATA];

    setNum     = (unsigned char *) ((char *)tagId + sizeof(FCS_TAG_ID_STRUCT));
    setParInfo = (FCS_SET_PARA_INFO *)(setNum + 4);
    
	for(i=0; i<*setNum; i++)
	{
		FCS_writeTagParameter(*tagId, setParInfo);
		setParInfo ++;
	}

	FCS_Tx_Msg( 0,SET_A_BLK_PARAS_VAL, ACK_OK, PACKET_ONE_TIME, NULL, 0, 0);
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void FCS_Svc_GetABlkParasVal()
{
	FCS_TAG_ID_STRUCT *tagId;
							
	unsigned char  data[512];
	uint32_t size;

    if( ghEWS_PortDirection == EWSDEFS_PORT_SERIAL ) tagId = (FCS_TAG_ID_STRUCT *)&gdsEWS_Comm_Msg.u8Rx_Data[EWSDEFS_COMM_DATA];
    else                                             tagId = (FCS_TAG_ID_STRUCT *)&ghEth.cRxBuffer[EWSDEFS_COMM_DATA];

	size = FCS_readTagParameter( *tagId, (S8 *)data );

	FCS_Tx_Msg( 0,GET_A_BLK_PARAS_VAL, ACK_OK, PACKET_ONE_TIME, data, (u16int_t)size, 0);
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void FCS_Svc_GetAllMonitoringData()
{
	unsigned char *tmpPos;
	uint32_t size;
	
	FCS_SYSTEM_STATUS_STRUCT * systemStatus;                             // System 상태, I/O, Var, ScanBuffer 순으로 data 수집
	FCS_SVC_IO_DATA_HEAD     * svcIoDataHead;
	FCS_SVC_VAR_DATA_HEAD    * svcVarDataHead;
	
	systemStatus = (FCS_SYSTEM_STATUS_STRUCT *)u8FCSSvcData;             // system 상태 정보
	memcpy(systemStatus, &gFCSDevSts, sizeof(FCS_DEVICESTATUS_VAR));

	// I/O data, raw and engineering
	svcIoDataHead              = (FCS_SVC_IO_DATA_HEAD *)(u8FCSSvcData + sizeof(FCS_SYSTEM_STATUS_STRUCT));
	svcIoDataHead->biNum       = gFCSIOData.u32BIPointNum;
	svcIoDataHead->boNum       = gFCSIOData.u32BOPointNum;
	svcIoDataHead->aiNum       = gFCSIOData.u32AIPointNum;
	svcIoDataHead->aoNum       = gFCSIOData.u32AOPointNum;
	svcIoDataHead->biOffset    = gFCSIOData.u32BIPointOffset;
	svcIoDataHead->boOffset    = gFCSIOData.u32BOPointOffset;
	svcIoDataHead->aiOffset    = gFCSIOData.u32AIPointOffset;
	svcIoDataHead->aoOffset    = gFCSIOData.u32AOPointOffset;
	svcIoDataHead->aiEngOffset = gFCSIOData.u32AIEngPointOffset;
	svcIoDataHead->aoEngOffset = gFCSIOData.u32AOEngPointOffset;
//	svcIoDataHead->aiEngOffset = gIoRawDataSize + gAInDataEngOffset;
//	svcIoDataHead->aoEngOffset = gIoRawDataSize + gAOutDataEngOffset;

	tmpPos = (unsigned char *)((unsigned char *)svcIoDataHead + sizeof(FCS_SVC_IO_DATA_HEAD));
	memcpy((char *)tmpPos, gFCSIOData.u8IoData, gFCSIOData.u32IODataSize);
	memcpy((char *)tmpPos + gFCSIOData.u32IODataSize, gFCSIOData.u8AIoEngData, gFCSIOData.u32IOEngDataSize);

	// Variable data
	svcVarDataHead             = (FCS_SVC_VAR_DATA_HEAD *)(tmpPos + gFCSIOData.u32IODataSize + gFCSIOData.u32IOEngDataSize);
	svcVarDataHead->bVarNum    = gFCSVarData.u32BVarPoint;
	svcVarDataHead->iVarNum    = gFCSVarData.u32IVarPoint;
	svcVarDataHead->fVarNum    = gFCSVarData.u32FVarPoint;
	svcVarDataHead->bVarOffset = gFCSVarData.u32BVarOffset;
	svcVarDataHead->iVarOffset = gFCSVarData.u32IVarOffset;
	svcVarDataHead->fVarOffset = gFCSVarData.u32FVarOffset;

	tmpPos = (unsigned char *)((unsigned char *)svcVarDataHead + sizeof(FCS_SVC_VAR_DATA_HEAD));
	memcpy(tmpPos, gFCSVarData.u8VarData, gFCSVarData.u32VarDataSize);

	// Logic data
	tmpPos += gFCSVarData.u32VarDataSize;
	memcpy(tmpPos, gFCSLogicData.u8ScanBuff, FCS_MAX_SIZE_SCAN_BUFF);

	size = (uint32_t)tmpPos + FCS_MAX_SIZE_SCAN_BUFF - (uint32_t)u8FCSSvcData;
	ClearWatchdog();

	FCS_Tx_Msg( 0,GET_ALL_MONITORING_DATA, ACK_OK, PACKET_ONE_TIME, u8FCSSvcData, (u16int_t)size, 0);
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

bool FCS_LogicCheck(void)
{
    bool                    bResult    = false;
    bool                    bResultCRC = false;
    u16int_t                     u16Crc = 0;
    dsTimeInfo              IEDTime;

    #if(DEBUG_CODE)
    printf("\n FCS_LogicInfo Backup Status: ");
    #endif

    u16Crc     = CalculateCRC( &(pNVRAM->NVFCSLogicInfo), (uint32_t)sizeof(IED_FCS_LOGIC_INFO)-4 );
    if( u16Crc == pNVRAM->NVFCSLogicInfo.m_u16CRC )  bResultCRC = true;
    else                                             bResultCRC = false;
    //bResultCRC  = CheckDnpCRC( (unsigned char *)&(pNVRAM->NVFCSLogicInfo), (u16int_t)sizeof(IED_FCS_LOGIC_INFO) );

//    printf("\n LogicCheck %04X:%04X", u16Crc, pNVRAM->NVFCSLogicInfo.m_u16CRC);
    if( bResultCRC == true )
    {
        bResult = true;
    }
    else
    {
        #if(DEBUG_CODE)
        printf("  [FCS_LogicInfo CRC Check ERROR] ");
        #endif

		memset( (unsigned char *)&pNVRAM->NVFCSLogicInfo.m_dsData.u8IOData  , 0, FCS_MAX_SIZE_IO_CONFIG);
	    ClearWatchdog();
		memset( (unsigned char *)&pNVRAM->NVFCSLogicInfo.m_dsData.u8VarData , 0, FCS_MAX_SIZE_IO_CONFIG);
	    ClearWatchdog();
		memset( (unsigned char *)&pNVRAM->NVFCSLogicInfo.m_dsData.u8PageData, 0, FCS_MAX_SIZE_FB_CONFIG);
	    ClearWatchdog();
		memset( (unsigned char *)&pNVRAM->NVFCSLogicInfo.m_dsData.u8LifData , 0, FCS_MAX_SIZE_FB_CONFIG);
	    ClearWatchdog();

    	HISTORY_SaveSettingChange( EVENTNAME_IED_LOGIC_CHANGE, ACTORS_INITIALIZE, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
        IEDTimeStamp(&IEDTime);
        HISTORY_SaveStsEvent(STSEVN_SETMON_LOGIC_ERROR, IED_SET_INITIALIZE, &IEDTime);

        bResult = false;
    }

	SetDnpCRC( (unsigned char *)&pNVRAM->NVFCSLogicInfo, (u16int_t) sizeof(IED_FCS_LOGIC_INFO) );
 //   printf("\n LogicCheckSet %04X", pNVRAM->NVFCSLogicInfo.m_u16CRC);

    #if(DEBUG_CODE)
    if( bResult ) printf(" OK ");
    else          printf(" Fail ");
    #endif

    return( bResult );
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void FCS_InitVar(void)
{
    unsigned char   i, idx=0, position=0, bitposition=0;
    
    u32FCSBitMask[0] = 0x01;
    u32FCSBitMask[1] = 0x02;
    u32FCSBitMask[2] = 0x04;
    u32FCSBitMask[3] = 0x08;
    u32FCSBitMask[4] = 0x10;
    u32FCSBitMask[5] = 0x20;
    u32FCSBitMask[6] = 0x40;
    u32FCSBitMask[7] = 0x80;

    u8FCSLogicRevA   = false;
    u8FCSLogicRevB   = false;
    u8FCSDownPageNum = false;

	memset(  u32FCSSizeofFB, 0, (FCS_MAX_NUM_FB_LIB*sizeof(uint32_t)));
    memset( &gFCSDevSts    , 0, sizeof(FCS_DEVICESTATUS_VAR) );
    memset( &gFCSConfigData, 0, sizeof(FCS_CONFIGDATA_VARS)  );
    memset( &gFCSLogicExe  , 0, sizeof(FCS_LOGICEXE_VARS)    );

    ClearWatchdog();

    /* MJC-110302 : 각 IO type별 Data 저장 */
    gFCSIOData.u32BIPointNum       = FCS_MAX_BI_CHANNEL_NUM;
    gFCSIOData.u32BOPointNum       = FCS_MAX_BO_CHANNEL_NUM;
    gFCSIOData.u32AIPointNum       = FCS_MAX_AI_CHANNEL_NUM;
    gFCSIOData.u32AOPointNum       = FCS_MAX_AO_CHANNEL_NUM;

    gFCSIOData.u32BIPointOffset    = 0; 
    gFCSIOData.u32BOPointOffset    = gFCSIOData.u32BIPointOffset    + FCS_MAX_BI_BYTE_NUM;   
    gFCSIOData.u32AIPointOffset    = gFCSIOData.u32BOPointOffset    + FCS_MAX_BO_BYTE_NUM;
    gFCSIOData.u32AOPointOffset    = gFCSIOData.u32AIPointOffset    + FCS_MAX_AI_CHANNEL_NUM * 2;
    gFCSIOData.u32AIEngPointOffset = 0;
    gFCSIOData.u32AOEngPointOffset = gFCSIOData.u32AIEngPointOffset + FCS_MAX_AI_CHANNEL_NUM * 4;

    gFCSIOData.u32IODataSize       = gFCSIOData.u32AOPointOffset    + FCS_MAX_AO_CHANNEL_NUM * 2;
    gFCSIOData.u32IOEngDataSize    = gFCSIOData.u32AOEngPointOffset + FCS_MAX_AO_CHANNEL_NUM * 4;

    gFCSIOData.pbBIData            = (unsigned char    *)        gFCSIOData.u8IoData     + gFCSIOData.u32BIPointOffset;
    gFCSIOData.pbBOData            = (unsigned char    *)        gFCSIOData.u8IoData     + gFCSIOData.u32BOPointOffset;
    gFCSIOData.pwAIData            = (u16int_t   *)       (gFCSIOData.u8IoData     + gFCSIOData.u32AIPointOffset);
    gFCSIOData.pwAOData            = (u16int_t   *)       (gFCSIOData.u8IoData     + gFCSIOData.u32AOPointOffset);
    gFCSIOData.pfAIEngData         = (float *) ((unsigned char *)gFCSIOData.u8AIoEngData + gFCSIOData.u32AIEngPointOffset);
    gFCSIOData.pfAOEngData         = (float *) ((unsigned char *)gFCSIOData.u8AIoEngData + gFCSIOData.u32AOEngPointOffset);


    /* MJC-110302 : 각 변수 type별 Data 저장 */
    gFCSVarData.u32BVarPoint       = FCS_MAX_bool_VAR_NUM;
    gFCSVarData.u32IVarPoint       = FCS_MAX_INT_VAR_NUM;
    gFCSVarData.u32FVarPoint       = FCS_MAX_FLOAT_VAR_NUM;

    gFCSVarData.u32BVarOffset      = 0;
    gFCSVarData.u32IVarOffset      = gFCSVarData.u32BVarOffset + FCS_MAX_bool_VAR_NUM/8;
    gFCSVarData.u32FVarOffset      = gFCSVarData.u32IVarOffset + FCS_MAX_INT_VAR_NUM * 4;
    gFCSVarData.u32VarDataSize     = gFCSVarData.u32FVarOffset + FCS_MAX_FLOAT_VAR_NUM * 4;

    gFCSVarData.pBVarData          = (unsigned char    *)  gFCSVarData.u8VarData + gFCSVarData.u32BVarOffset;
    gFCSVarData.pIVarData          = (u16int_t   *)( gFCSVarData.u8VarData + gFCSVarData.u32IVarOffset );
    gFCSVarData.pFVarData          = (float *)( gFCSVarData.u8VarData + gFCSVarData.u32FVarOffset );

	/* Metalsky-20110420 : 성산에서 추가한 EWS의 인터락키 처리 플래그 */
	u8FCS_InterlockFlag 	= false;
	u8FCS_InterlockKey_Pre 	= false;
	

    /* MJC-110302 : logic 저장, 현재 NVRAM 저장 된 공간 뒤에. */
    /* MJC-110321 : 현재 Logic을 위한 저장 공간을 사용하지 않음 -> PageSave에 저장 */
    /* MJC-110321 : NVRAM 저장 공간 사이즈와 buffer 배열 사이즈 통일 */
    gFCSSaveLogic.pbIoSaveAddr     = (unsigned char *)&pNVRAM->NVFCSLogicInfo.m_dsData.u8IOData[0];
    gFCSSaveLogic.pbVarSaveAddr    = (unsigned char *)&pNVRAM->NVFCSLogicInfo.m_dsData.u8VarData[0];

	for (i=0; i<FCS_MAX_NUM_PAGE; i++) {
		gFCSSaveLogic.pbPageSaveAddr[i]	= (unsigned char *)&pNVRAM->NVFCSLogicInfo.m_dsData.u8PageData[i][0];
	}
	gFCSSaveLogic.pbLifSaveAddr    = (unsigned char *)&pNVRAM->NVFCSLogicInfo.m_dsData.u8LifData[0];
    ClearWatchdog();

    u8FCSLogicDownloadFlag = false;


    idx = MAX_DO_CHANNEL+MAX_LED+NUM_BASIC_MPU_OUT_ELMT;
    for( i=1; i<=MAX_EWS_OUT_INTERLOCK; i++ )
    {
        FCS_getBitPosition(idx+i, &position, &bitposition);
		gFCSIOData.pbBOData[position] |=  u32FCSBitMask[bitposition];
		u8FCS_BOData_Pre[position] = gFCSIOData.pbBOData[position];
//        gFCSIOData.pbBOData[i] = 0;
    }    


    
    u8FCS_StartFlag = false;
    u8FCS_DIInitFlag = false;
    u8FCS_EdtiEnableFlag = false;


}



/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void GUI_InitSystem(void)
{
    GUI_InitComm();

    GUI_SetSizeofFB();

    GUI_ClearAllLogicInfo();
    GUI_SetAllLogicToStop();

    GUI_ReadConfigData();
    GUI_LoadConfigData();

    GUI_SetLogicToBeReady();
    
    //ClearWatchdog();
}


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void GUI_Init(void)
{
    GUI_InitVar();
    GUI_InitSystem();
}

#endif

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/



/*****************************[ End of Program ]******************************/

