/******************************************************************************
    FILE NAME   : FCS_Main.H
    AUTHOR      : MetalSky / TechReserch Team
    DATE        : 2009.xx.xx ~ 2009.xx.xx
    REVISION    : Ver0.10
    DESCRIPTION : FCS Main. Header.

 ******************************************************************************
    HISTORY     :


 ******************************************************************************/
#ifndef _GUI_CMD_H_
#define _GUI_CMD_H_



/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

#define ERROR           (-1)
#define OK              1

#define STOP            0
#define RUN             1

#define EXIST           0
#define EMPTY           1

#define FLAG_RESET      0
#define FLAG_SET        1
#define FLAG_FULLSET    0xFF

#define NO_WAIT         (-1)
#define WAIT_FOREVER    0


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

#define FCS_FB_NEWDEF   1
#define FCS_FB_DEBUG    0


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

#if (CURRDIFF_RELAY)
#define FCS_MAX_BI_CHANNEL_NUM		358	// Extended DI 64 + BASICIN 40 + DSPOUT 166 + MPUOUT 20 + EWSOUT 4 + GOOSEIN  64 
#define FCS_MAX_BO_CHANNEL_NUM		136	// Extended DO 32 + LEDOUT  16 + MPUOUT  20 + EWSOUT 4 + GOOSEOUT 64

#define FCS_MAX_BI_BYTE_NUM		    48	// ((MAX_BI_CHANNEL_NUM/8 + 1) + n) % 4 = 0이 되기 위한 값
#define FCS_MAX_BO_BYTE_NUM		    20	// ((MAX_BO_CHANNEL_NUM/8 + 1) + n) % 4 = 0이 되기 위한 값
#endif
#if (DISTANCE_RELAY)
//#define FCS_MAX_BI_CHANNEL_NUM		329	// Extended DI 64 + GOOSEIN  64 + BASICIN 29 + DSPOUT 136 + MPUOUT 20 + EWSOUT 20
//#define FCS_MAX_BO_CHANNEL_NUM		148	// Extended DO 32 + GOOSEOUT 64 + LEDOUT  16 + MPUOUT  8 + EWSOUT 20
/* MetalSky-110530 : DSP 및 MPU 논리요소 추가에 따른 EWS 요소 추가 */
#define FCS_MAX_BI_CHANNEL_NUM		414	// Extended DI 64 + BASICIN 40 + DSPOUT 205 + MPUOUT 25 + EWSOUT 16 + GOOSEIN  64
#define FCS_MAX_BO_CHANNEL_NUM		153	// Extended DO 32 + LEDOUT  16 + MPUOUT  25 + EWSOUT 16 + GOOSEOUT 64

#define FCS_MAX_BI_BYTE_NUM		    52	// ((MAX_BI_CHANNEL_NUM/8 + 1) + n) % 4 = 0이 되기 위한 값
#define FCS_MAX_BO_BYTE_NUM		    20	// ((MAX_BO_CHANNEL_NUM/8 + 1) + n) % 4 = 0이 되기 위한 값
#endif

#define FCS_MAX_AI_CHANNEL_NUM		2
#define FCS_MAX_AO_CHANNEL_NUM		2


#define FCS_MAX_bool_VAR_NUM		512
#define FCS_MAX_INT_VAR_NUM			64
#define FCS_MAX_FLOAT_VAR_NUM		64


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

#define FCS_IO_CONFIG_TITLE			"IO Tag Configuration\0"
#define FCS_VAR_TITLE				"Variable Configuration\0"

#define FCS_MAX_NUM_PAGE			5
#define FCS_MAX_NUM_BLOCK_IN_1PAGE 	100

#define FCS_MAX_NUM_FB_LIB			256
#define FCS_MAX_NUM_IO_MDL			8

#define FCS_MAX_SIZE_OF_FILE_NAME	32
#define FCS_MAX_SIZE_LOGICPAGE_DATA	4096
#define FCS_MAX_SIZE_FB_CONFIG		FCS_MAX_SIZE_LOGICPAGE_DATA*FCS_MAX_NUM_PAGE
#define FCS_MAX_SIZE_IO_CONFIG		1024
#define FCS_MAX_SIZE_SCAN_BUFF		512

#define FCS_MAX_CHANNEL_IN_1MODULE	FCS_MAX_BI_CHANNEL_NUM


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

/* logic revision id */
typedef struct {

    unsigned char a;
    unsigned char b;
    
} FCS_LOGIC_REVISION_ID;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

/* page info */
typedef struct {

	unsigned char	fileName[FCS_MAX_SIZE_OF_FILE_NAME];

    FCS_LOGIC_REVISION_ID RevId;
	unsigned char	pageId;
	unsigned char	exeOrder;
	uint16_t	period;
	unsigned char	runStopFlag;
	unsigned char	res;

} FCS_PAGE_INFO_VARS;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

#if (0)
typedef union {
	struct {
		unsigned char	type;
		unsigned char	hwId;
		unsigned char	pageId;
		unsigned char  res1;
		uint16_t	id;
		unsigned char  res2[2];
	} fb;
	struct {
		unsigned char	type;
		unsigned char	hwid;
		unsigned char	res1;
		unsigned char  res2;
		uint16_t	id;
		unsigned char  res3[2];
	} io;
	struct {
		unsigned char	type;
		unsigned char	hwid;
		unsigned char	res1;
		unsigned char  res2;
		uint16_t	id;
		unsigned char  res3[2];
	} var;
} FCS_TAG_ID_STRUCT;
#else
typedef union {
	struct {
		unsigned char	type;
		unsigned char	pageId;
		uint16_t	id;
	} fb;
	struct {
		unsigned char	type;
		unsigned char	res1;
		uint16_t	id;
	} io;
	struct {
		unsigned char	type;
		unsigned char	res1;
		uint16_t	id;
	} var;
} FCS_TAG_ID_STRUCT;
#endif


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

typedef struct {
	unsigned char	    normal			:1,
			run				:1,
			ioConfigOk		:1,
			varConfigOk		:1,
			logicConfigOk	:1,
			res				:3;
} FCS_DEVICESTATUS_VAR;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

typedef struct {

	unsigned char  u8Title[32];
	uint16_t u16NumofPoint[8];   // 0:bi 1:bo 2:ai 3:ao
	uint32_t u32Offset[8];  // 0:bi 1:bo 2:ai 3:ao
	
} FCS_IO_CONFIG_HEAD;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

typedef struct {

	FCS_TAG_ID_STRUCT	tagId;
	unsigned char	lockMask;
	unsigned char	numOut;
	unsigned char	numIn;
	unsigned char	res;
	
} FCS_FB_HEADER_STRUCT;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/
#if(0)
typedef struct {

    unsigned char                      u8IoData[FCS_MAX_BI_CHANNEL_NUM/8 + FCS_MAX_BO_CHANNEL_NUM/8 + (FCS_MAX_AI_CHANNEL_NUM + FCS_MAX_AO_CHANNEL_NUM) * 2];
    unsigned char                     *pbBIData;
    unsigned char                     *pbBOData;
    uint16_t                    *pwAIData;
    uint16_t                    *pwAOData;
    
    unsigned char                      u8AIoEngData[(FCS_MAX_AI_CHANNEL_NUM + FCS_MAX_AO_CHANNEL_NUM) * 4];
    float                  *pfAIEngData;
    float                  *pfAOEngData;

    unsigned char                      u8IoConfig[FCS_MAX_SIZE_IO_CONFIG];
    uint32_t                     u32PointingIoConfig[FCS_MAX_NUM_IO_MDL][FCS_MAX_CHANNEL_IN_1MODULE];
    FCS_IO_CONFIG_HEAD     *pIODataHead;

    uint32_t                     u32BIPointNum;
    uint32_t                     u32BOPointNum;
    uint32_t                     u32AIPointNum;
    uint32_t                     u32AOPointNum;

    uint32_t                     u32BIPointOffset;
    uint32_t                     u32BOPointOffset;
    uint32_t                     u32AIPointOffset;
    uint32_t                     u32AOPointOffset;
    
    uint32_t                     u32AIEngPointOffset;
    uint32_t                     u32AOEngPointOffset;

    uint32_t                     u32IODataSize;
    uint32_t                     u32IOEngDataSize;

} FCS_IO_DATA_VARS;
#endif

/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

typedef struct {

    unsigned char      u8VarData[FCS_MAX_bool_VAR_NUM/8 + FCS_MAX_INT_VAR_NUM * 4 + FCS_MAX_FLOAT_VAR_NUM * 4];
    unsigned char     *pBVarData;
    uint32_t    *pIVarData;
    float  *pFVarData;

    uint32_t     u32BVarPoint;
    uint32_t     u32IVarPoint;
    uint32_t     u32FVarPoint;

    uint32_t     u32BVarOffset;
    uint32_t     u32IVarOffset;
    uint32_t     u32FVarOffset;

    uint32_t     u32VarDataSize;

    FCS_IO_CONFIG_HEAD *pVarConfigHead;

} FCS_VARIABLEDATA_VARS;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

typedef struct {

	unsigned char  u8IoBuff[FCS_MAX_SIZE_IO_CONFIG];
	unsigned char  u8VarBuff[FCS_MAX_SIZE_IO_CONFIG];
	unsigned char  u8LogicBuff[FCS_MAX_SIZE_LOGICPAGE_DATA*FCS_MAX_NUM_PAGE];
	unsigned char  u8LifBuff[FCS_MAX_SIZE_LOGICPAGE_DATA*FCS_MAX_NUM_PAGE];
	
	uint32_t u32LogicConfigSize;
	uint32_t u32IoConfigSize;
	uint32_t u32VarDataSize;
	uint32_t u32LifDataSize;

} FCS_CONFIGDATA_VARS;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

typedef struct {

    unsigned char * pbLogicSaveAddr;
    unsigned char * pbIoSaveAddr;
    unsigned char * pbVarSaveAddr;
    unsigned char * pbPageSaveAddr[FCS_MAX_NUM_PAGE];
    unsigned char * pbLifSaveAddr;

} FCS_LOGICSAVE_VARS;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

typedef struct {

    uint32_t u32pPageInfo[FCS_MAX_NUM_PAGE];
    uint32_t u32pBlk[FCS_MAX_NUM_PAGE][FCS_MAX_NUM_BLOCK_IN_1PAGE];
    uint32_t u32pBlkSeq[FCS_MAX_NUM_PAGE][FCS_MAX_NUM_BLOCK_IN_1PAGE];

    unsigned char  u8Blk[FCS_MAX_SIZE_FB_CONFIG];
    unsigned char  u8ScanBuff[FCS_MAX_SIZE_SCAN_BUFF];
} FCS_LOGICDATA_VARS;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

typedef struct {
	unsigned char  u8ExeOrder[FCS_MAX_NUM_PAGE];
	uint16_t u16TimeToRun[FCS_MAX_NUM_PAGE];
	uint32_t u32ExecuterPeriod;
	uint32_t	u32IsRun;
} FCS_LOGICEXE_VARS;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

typedef union {
	int 		addr;
	int			iVal;
	float		fVal;
	FCS_TAG_ID_STRUCT tagId;
} FCS_ADDR_OR_VAL;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

typedef struct {
	unsigned char		inType;		/* input type(CONST:2, BLOCK:1) */
	unsigned char		valType;	/* bool:1, int:4, float:5, tagId:6 */
	unsigned char 		offset;		/* 블럭인 경우 몇번째 출력인지 */
	unsigned char		res;
	FCS_ADDR_OR_VAL 	addrOrVal;
} FCS_IN_INFO;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

typedef struct {

	FCS_LOGIC_REVISION_ID ab;
	unsigned char sysConfigFlag;
	unsigned char ioConfigFlag;
	unsigned char numLogicPage;
	unsigned char reserved[3];
	
} FCS_CONFIG_INFO_VARS;


/******************************************************************************
 **                                                                          **
 **  Var data 모니터링 서비스 header                                         **
 **                                                                          **
 ******************************************************************************/

typedef struct {
	unsigned char      bVarNum;
	unsigned char      iVarNum;
	unsigned char      fVarNum;
	unsigned char      reserved;
	int32_t		bVarOffset;
	int32_t		iVarOffset;
	int32_t     fVarOffset;
} FCS_SVC_VAR_DATA_HEAD;


/******************************************************************************
 **                                                                          **
 **   I/O data 모니터링 서비스 header                                        **
 **                                                                          **
 ******************************************************************************/

typedef struct {
	unsigned char       biNum;
	unsigned char       boNum;
	unsigned char       aiNum;
	unsigned char       aoNum;
	unsigned char       res1[4];	// 향후 사용 가능
	int32_t		 biOffset;
	int32_t	 	 boOffset;
	int32_t	 	 aiOffset;
	int32_t	 	 aoOffset;
	int32_t	 	 res2[4];	// 향후 사용 가능
	int32_t		 aiEngOffset;
	int32_t		 aoEngOffset;
	int32_t		 res3[2];	// 향후 사용 가능
} FCS_SVC_IO_DATA_HEAD;


/******************************************************************************
 **                                                                          **
 **   로직 페이지 상태정보 서비스 데이터 구조                                **
 **                                                                          **
 ******************************************************************************/

// 한 페이지 상태 정보 구조
typedef struct {
	unsigned char      pageId;
	unsigned char      runStopFlag;
	unsigned char      runSeq;
	unsigned char      blkNum;
} FCS_A_PAGE_STATUS;

typedef struct {
	unsigned char                  pageNum;
	unsigned char                  reserved[3];
	FCS_A_PAGE_STATUS   pageSts[FCS_MAX_NUM_PAGE];
} FCS_SVC_PAGES_STATUS;


/******************************************************************************
 **                                                                          **
 **   출력값 모니터링 서비스 데이터 구조                                     **
 **   한개의 block은 여러개의 출력값을 가질수 있다.                          **
 **   이 head뒤에 outNum 만큼 값들이 기록된다.                               **
 **                                                                          **
 ******************************************************************************/

typedef struct {
	unsigned char      blkId;
	unsigned char      outNum;
	unsigned char      lockMaskVal;
	unsigned char      reserved;
} FCS_A_BLK_OUT_VAL_HEAD;


/******************************************************************************
 **                                                                          **
 **    int or float                                                          **
 **                                                                          **
 ******************************************************************************/

typedef union {
	int32_t   i;
	float f;
} FCS_IORF;


/******************************************************************************
 **                                                                          **
 **    출력값 지정 요청 데이터 구조                                          **
 **                                                                          **
 ******************************************************************************/

typedef struct {
	FCS_TAG_ID_STRUCT tagId;
	unsigned char                outNo;
	unsigned char                lockMaskVal;
	unsigned char                reserved[2];
	FCS_IORF          val;
} FCS_SET_A_BLK_OUTVAL;


/******************************************************************************
 **                                                                          **
 **    set parameter info                                                    **
 **                                                                          **
 ******************************************************************************/

typedef struct {
	unsigned char       parOffset;
	unsigned char       subOffset;
	unsigned char       parType;			/* bool:1, byte:2, word:3, int:4, float:5 */
	unsigned char       reserved;
	FCS_IORF val;
} FCS_SET_PARA_INFO;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

typedef struct {
	FCS_DEVICESTATUS_VAR	devSts;
	char                      reserved[3];
} FCS_SYSTEM_STATUS_STRUCT;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

#define FCS_MAX_IN_NUM              8
#define FCS_INTERLOCK_DEV_LINE_X    8
#define FCS_INTERLOCK_DEV_LINE_Y    24

enum {
    #if (CURRDIFF_RELAY)
    FCS_DEVICE_43CA_ON,
    FCS_DEVICE_43CA_OFF,
    FCS_DEVICE_43RC_ON,
    FCS_DEVICE_43RC_OFF,
    FCS_DEVICE_43CA_ON_P,
    FCS_DEVICE_43CA_OFF_P,
    FCS_DEVICE_43RC_ON_P,
    FCS_DEVICE_43RC_OFF_P,
    FCS_DEVICE_43PDA_AUTO,
    FCS_DEVICE_43PDA_MANUAL,
    FCS_DEVICE_43PDA_OFF_MANUAL,
    FCS_DEVICE_43PDA_A_BUS_MANUAL,
    FCS_DEVICE_43PDA_B_BUS_MANUAL,
    FCS_DEVICE_43PDA_A_BUS_AUTO,
    FCS_DEVICE_43PDA_B_BUS_AUTO,
    FCS_DEVICE_GOOSE_SUBFAIL,
    FCS_DEVICE_REPORT_OVERFLOW,  
    FCS_DEVICE_IED_ALARM,
    FCS_DEVICE_LOGIC_OUT_MAX,
    FCS_DEVICE_CB_CLOSE = FCS_DEVICE_LOGIC_OUT_MAX,
    FCS_DEVICE_CB_OPEN,
    FCS_DEVICE_CB_CLOSE_INT,
    FCS_DEVICE_CB_OPEN_INT,
    #endif
    #if (DISTANCE_RELAY)
    FCS_DEVICE_43PDA_AUTO,
    FCS_DEVICE_43PDA_MANUAL,
    FCS_DEVICE_43PDA_OFF_MANUAL,
    FCS_DEVICE_43PDA_A_BUS_MANUAL,
    FCS_DEVICE_43PDA_B_BUS_MANUAL,
    FCS_DEVICE_43PDA_A_BUS_AUTO,
    FCS_DEVICE_43PDA_B_BUS_AUTO,
    FCS_DEVICE_GOOSE_SUBFAIL,
    FCS_DEVICE_REPORT_OVERFLOW,
    FCS_DEVICE_IED_ALARM,
    FCS_DEVICE_21_RESERVED1,
    FCS_DEVICE_21_RESERVED2,
    FCS_DEVICE_LOGIC_OUT_MAX,
    FCS_DEVICE_CB_CLOSE = FCS_DEVICE_LOGIC_OUT_MAX,
    FCS_DEVICE_CB_OPEN,
    FCS_DEVICE_1DS_CLOSE,
    FCS_DEVICE_1DS_OPEN,
    FCS_DEVICE_2DS_CLOSE,
    FCS_DEVICE_2DS_OPEN,
    FCS_DEVICE_6DS_CLOSE,
    FCS_DEVICE_6DS_OPEN,
    FCS_DEVICE_CB_CLOSE_INT,
    FCS_DEVICE_CB_OPEN_INT,
    FCS_DEVICE_1DS_CLOSE_INT,
    FCS_DEVICE_1DS_OPEN_INT,
    FCS_DEVICE_2DS_CLOSE_INT,
    FCS_DEVICE_2DS_OPEN_INT,
    FCS_DEVICE_LINEDS_CLOSE_INT,
    FCS_DEVICE_LINEDS_OPEN_INT,
    #endif

    FCS_DEVICE_MAX
};


typedef struct {
    unsigned char u8PageId;
    unsigned char u8TagId;
} FCS_INTERLOCK_IO_INFO;

typedef struct {
    unsigned char u8FbId;
    uint16_t u8IOId;
    unsigned char u8In_Num;
    unsigned char u8Flag;
    unsigned char u8In_Idx[FCS_MAX_IN_NUM];
    unsigned char u8Out_Idx;
    
    FCS_INTERLOCK_IO_INFO   u8Tag_Info;
    FCS_INTERLOCK_IO_INFO   u8Tag_xy;
    
    FCS_INTERLOCK_IO_INFO   u8In_Info[FCS_MAX_IN_NUM];
    
} FCS_INTERLOCK_DATA_STRUCT;

extern const uint32_t   u32FCSInterlockDevIdx[FCS_DEVICE_MAX];


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

typedef struct
{
	unsigned char		u8IOData[FCS_MAX_SIZE_IO_CONFIG];
	unsigned char		u8VarData[FCS_MAX_SIZE_IO_CONFIG];
	unsigned char		u8PageData[FCS_MAX_NUM_PAGE][FCS_MAX_SIZE_LOGICPAGE_DATA];
	unsigned char		u8LifData[FCS_MAX_SIZE_FB_CONFIG];
} dsFCSLogicInfo;

typedef struct
{
	dsFCSLogicInfo	m_dsData;

	uint16_t				m_u16CRC;
	uint16_t				m_u16Dummy;
	
} IED_FCS_LOGIC_INFO;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

extern uint32_t u32FCSBitMask[8];
extern unsigned char  u8FCSLogicRevA, u8FCSLogicRevB;
extern unsigned char  u8FCSDownPageNum;
extern uint32_t u32FCSSizeofFB[FCS_MAX_NUM_FB_LIB];

extern FCS_DEVICESTATUS_VAR      gFCSDevSts;
extern FCS_VARIABLEDATA_VARS     gFCSVarData;

extern FCS_LOGICSAVE_VARS        gFCSSaveLogic;
extern FCS_CONFIGDATA_VARS       gFCSConfigData;
extern FCS_LOGICDATA_VARS        gFCSLogicData;
extern FCS_LOGICEXE_VARS         gFCSLogicExe;

extern FCS_PAGE_INFO_VARS        gFCSPageInfo;


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void FCS_Svc_GetAllVarVal(void);
void FCS_Svc_GetPagesStatus(FCS_SVC_PAGES_STATUS *pagesSts);
void FCS_Svc_GetLogicPagesStatus(void);
void FCS_Svc_SetAllLogicPagesRunStop(void);
void FCS_Svc_SetALogicPageRunStop(void);
void FCS_Svc_GetLogicPageBlksOut(void);
void FCS_Svc_GetAllIoVal(void);
void FCS_Svc_SetOutVal(void);
void FCS_Svc_SetABlkParasVal(void);
void FCS_Svc_GetABlkParasVal(void);
void FCS_Svc_GetAllMonitoringData(void);

void FCS_ClearAllLogicInfo(void);
void FCS_SetAllLogicToStop(void);


/******************************************************************************
 **                                                                          **
 **                                                                          **
 **                                                                          **
 ******************************************************************************/

void FCS_SetFBLib(unsigned char u8Type, unsigned char * pbData);

void FCS_SetLogicRevId(void);
void FCS_GetLogicRevId(void);
void FCS_UploadIoConfig(void);
void FCS_DownloadIoConfig(void);
void FCS_UploadVarConfig(void);
void FCS_DownloadVarConfig(void);
void FCS_UploadLogicConfig(void);
void FCS_DownloadLogicConfig(void);
void FCS_UploadLifConfig(void);
void FCS_DownloadLifConfig(void);

bool FCS_LogicCheck(void);

void FCS_InitVar(void);
void FCS_InitSystem(void);
void FCS_Init(void);


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

#endif

/*****************************[ End of Program ]*****************************/



