#define PWM_PERIOD 2500 // base freq 100Mhz UpDown counting Tpwm = 2 * PWM_PERIOD * 1/(100e6)

void EPWM_init(void);
void InitEPwm1(void);
void EPWM_enable(void);
