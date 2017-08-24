#define ADC_SCALE 4095.0f
#define IL_CONVERSION1 3.0f/4095.0f*30.0f
#define IO_CONVERSION1 3.0f/4095.0f*30.0f
#define IL_CONVERSION2 3.0f/4095.0f*30.0f
#define IO_CONVERSION2 3.0f/4095.0f*30.0f
#define VC_CONVERSION 3.0f/4095.0f*350.0f

#define WC 31.4f

typedef struct{
	float32 VC_dq[2];
	float32 IL_dq[2];
	float32 IO_dq[2];
	float32 PQ[2];
	float32 theta;
} struct_meas_states;

typedef struct{
	float32 VC_abc[2];
	float32 IL_abc[2];
	float32 IO_abc[2];
} struct_abc_states;

void Measurement_step(const bool enable);
float32 Oscillator(const bool enable, float32 theta, const float32 omega);
void ADC_process(const bool enable, struct_abc_states * states1, struct_abc_states * states2);
void Power_caculation(float32 PQ[2], const float32 IO_dq[2], const float32 VC_dq[2]);
void Abc2dq_fast(float32 dq[2], const float32 abc[2], const float32 table[2]);
float32 LPF(const float32 y_past, const float32 a, const float32 in);
