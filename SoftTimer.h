 extern  volatile uint32_t         glSoftwareTimer;

 #define GET_MSECTIME()            (glSoftwareTimer)
 #define GLOBAL_DATETIME()         ((DATETIME*)&ghDateTime)


/*****************************************************************************
 **                                                                         **
 **  소프트웨어 타이머 시간단위 값을 밀리초로 변환하는 매크로 선언          **
 **                                                                         **
 *****************************************************************************/

 #define MACRO_SECONDS_STIMER(s)           ((s)            *1000L)
 #define MACRO_MINUTES_STIMER(m)           ((m)        *60L*1000L)
 #define MACRO_HOURS_STIMER(h)             ((h)    *60L*60L*1000L)
 #define MACRO_DAYS_STIMER(d)              ((d)*24L*60L*60L*1000L)

 #define MACRO_MAX_DELAY_STIMER            MACRO_DAYS_STIMER(24)


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

typedef struct{
	uint32_t    lStimerTimeout;
    bool    	bIsElapsed;
    bool    	bIsActive;
    uint16_t    u16Dummy;
} SOFT_TIMER;


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

 typedef struct{
    uint16_t       u16Year;
    unsigned char  u8Month;
    unsigned char  u8Day;
    unsigned char  u8Hour;
    unsigned char  u8Min;
    unsigned char  u8Sec;
    unsigned char  u8Week;
    unsigned char  u16Msec;
 } DATETIME;


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

 extern DATETIME     ghDateTime;


/******************************************************************************
 **                                                                          **
 **  시각정보 구조체                                                         **
 **                                                                          **
 ******************************************************************************/

typedef struct {
    // 연/월/일/시/분/초/밀리초
    uint16_t m_u16Year;
    uint16_t m_u16Month;     // 1 ~ 12

    uint16_t m_u16Day;       // 1 ~ 31
    uint16_t m_u16Hour;      // 0 ~ 23

    uint16_t m_u16Min;       // 0 ~ 59
    uint16_t m_u16Sec;       // 0 ~ 59

    uint16_t m_u16Millisec;  // 0 ~ 999
    uint16_t m_u16Week;      // 0~6 & 1~7
} dsTimeInfo;


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

typedef struct
{
    uint32_t   mostSignificant;
    uint16_t   leastSignificant;
    uint16_t   u16Dummy;
} DNPTIME;


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

typedef struct {

    SOFT_TIMER  SecondScheduleTimer;
    SOFT_TIMER  MinuteScheduleTimer;

    SOFT_TIMER  SerialPortReInitTimer;
    SOFT_TIMER  DebugModeClearTimer;
    SOFT_TIMER  GuiInUseTimer;

    SOFT_TIMER  XmodemTimer;

} GLOBAL_HANDLE_STIMER_VAR;


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

extern GLOBAL_HANDLE_STIMER_VAR    ghSTIMER;


/*****************************************************************************
 **                                                                         **
 **                                                                         **
 **                                                                         **
 *****************************************************************************/

 void SoftTimer_InitVariable(void);
 void STimer_Deactivate( SOFT_TIMER *pSoftTimer );
 void STimer_Start( SOFT_TIMER *pSoftTimer, uint32_t lMsTimeout );
 void STimer_Reset(SOFT_TIMER *pSoftTimer, uint32_t lMsTimeout);
 bool STimer_IsElapsed( SOFT_TIMER *pSoftTimer );
 bool STimer_IsActive( SOFT_TIMER *pSoftTimer );
 bool STimer_IsActElapsed( SOFT_TIMER *pSoftTimer );
 uint32_t STimer_ReMainingTime( SOFT_TIMER *pSoftTimer );
 void STimer_SetElapsed( SOFT_TIMER *pSoftTimer );
