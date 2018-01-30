//===========================================================================

extern volatile Uint16 CtrlSrcFlag;
extern volatile Uint16 EnableFlag;
extern volatile Uint16 RunBreakFlag;
extern Uint16 Direction;
extern Uint16 SensorlessFlag;
extern Uint16 SinusoidalFlag;
extern Uint16 gu16CtrlMode;
extern _iq SpeedRef;
extern _iq CurrentSet;
extern _iq DCbus_current;
extern _iq current[3];
extern _iq DcVoltage;
extern _iq DCcurrent_Fbk;

extern SPEED_MEAS_CAP speed1;

extern float32 T;
extern PID_GRANDO_CONTROLLER pid1_spd;
extern PID_GRANDO_CONTROLLER pid1_idc;

extern Uint16 gu16AccTime;
extern Uint16 gu16DecTime;
extern Uint16 gu16Pole;
extern float  gfBaseFreq;
extern _iq giqBaseCurrent;
extern _iq giqCurrentScaler;

void Update_Ctrl_parameter ();
void Update_Cfg_parameter();
void WDogEnable(void);

extern dsSystem gdsSystem;
extern dsIMDCtrlIfc gdsIn;
extern dsIMDCtrlIfc gdsGui;
