#define ADC_SCALE 4095.0f
#define IL_CONVERSION 0.00464268f
#define IO_CONVERSION 0.00464268f
#define VC_CONVERSION 0.066276f //Calibrated on 12/06/2017
#define VDC_CONVERSION 0.094f //Calibrated on 11/22/2017

#define WC 31.4f

typedef struct{
	float32 VC_dq[2];
	float32 IL_dq[2];
	float32 IO_dq[2];
	float32 Vdc;
	float32 PQ[2];
	float32 theta;
} struct_meas_states;

typedef struct{
	float32 VC_abc[3];
	float32 IL_abc[2];
	float32 IO_abc[2];
} struct_abc_states;

void Measurement_step(const bool enable);
float32 Oscillator(const bool enable, float32 theta, const float32 omega);
void ADC_process(const bool enable, struct_abc_states * states1, struct_meas_states * m_states);
void Power_caculation(float32 PQ[2], const float32 IO_dq[2], const float32 VC_dq[2]);
void Abc2dq_fast(float32 dq[2], const float32 abc[2], const float32 table[2]);
//void Abc2dq(float32 dq[2], const float32 abc[2], const float32 theta);
float32 LPF(const float32 y_past, const float32 a, const float32 in);
