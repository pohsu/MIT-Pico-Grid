#define PWM_PERIOD 5000 // base freq 100Mhz UpDown counting Tpwm = 2 * PWM_PERIOD * 1/(100e6)

void EPWM_init(void);
void InitEPwm1(void);
void InitEPwm2(void);
void InitEPwm3(void);
void InitEPwm4(void);
void InitEPwm5(void);
void InitEPwm6(void);
void EPWM_enable(void);
