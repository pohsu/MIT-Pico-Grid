#include "F28x_Project.h"
#include <stdint.h>
#include <math.h>
#include "..\Test_setting.h"
#include "..\Peripheral\Peripheral.h"
#include "Meas.h"
#include "Control.h"

//***********************************************************************//
//                     G l o b a l  V a r i a b l e s                    //
//***********************************************************************//
extern struct_control_states control_states1, control_states2;
struct_meas_states meas_states1, meas_states2;

//***********************************************************************//
//                          F u n c t i o n s                            //
//***********************************************************************//
#pragma CODE_SECTION(Measurement_step, "ramfuncs")
void Measurement_step(const bool enable)
{

	meas_states1.theta = Oscillator(enable, meas_states1.theta, control_states1.omega);
	meas_states2.theta = Oscillator(enable, meas_states2.theta, control_states2.omega);

	struct_abc_states abc_states1, abc_states2;
	ADC_process(enable, &abc_states1, &abc_states2);

	float32 table[2]; //sin_theta, cos_theta, sin_theta120, cos_theta120;
	table[0] = sinf(meas_states1.theta);
	table[1] = cosf(meas_states1.theta);

	Abc2dq_fast(meas_states1.IL_dq, abc_states1.IL_abc, table);
	Abc2dq_fast(meas_states1.VC_dq, abc_states1.VC_abc, table);
	Abc2dq_fast(meas_states1.IO_dq, abc_states1.IO_abc, table);

	table[0] = sinf(meas_states2.theta);
	table[1] = cosf(meas_states2.theta);
	Abc2dq_fast(meas_states2.IL_dq, abc_states2.IL_abc, table);
	Abc2dq_fast(meas_states2.VC_dq, abc_states2.VC_abc, table);
	Abc2dq_fast(meas_states2.IO_dq, abc_states2.IO_abc, table);

	Power_caculation(meas_states1.PQ, meas_states1.PQ_raw, meas_states1.IO_dq, meas_states1.VC_dq);
	Power_caculation(meas_states2.PQ, meas_states2.PQ_raw, meas_states2.IO_dq, meas_states2.VC_dq);

//	static float32 LPF_IO_dq1[2] = {0,0},LPF_IO_dq2[2] = {0,0};
//	Uint16 i;
//	for(i=0; i<=1; i++){
//		LPF_IO_dq1[i] = LPF(LPF_IO_dq1[i], 500.0f, meas_states1.IO_dq[i]);
//		meas_states1.IO_dq[i] = LPF_IO_dq1[i];
//	}
//	for(i=0; i<=1; i++){
//		LPF_IO_dq2[i] = LPF(LPF_IO_dq2[i], 500.0f, meas_states2.IO_dq[i]);
//		meas_states2.IO_dq[i] = LPF_IO_dq2[i];
//	}

}

#pragma CODE_SECTION(Oscillator, "ramfuncs")
float32 Oscillator(const bool enable, float32 theta, const float32 omega)
{
	theta = (enable? theta + omega*ISR_PERIOD : 0);
	theta = (theta >= 2*PI? theta - 2*PI: theta);
	return theta;
}

#pragma CODE_SECTION(ADC_process, "ramfuncs")
void ADC_process(const bool enable, struct_abc_states * states1, struct_abc_states * states2)
{
	Uint16 i;
	static float32 IL1_bias[2] = {4096.0f, 4096.0f}, VC1_bias[2] = {2048.0f, 2048.0f}, IO1_bias[2] = {2048.0f, 2048.0f};
	static float32 IL2_bias[2] = {4096.0f, 4096.0f}, VC2_bias[2] = {2048.0f, 2048.0f}, IO2_bias[2] = {2048.0f, 2048.0f};
	static Uint16 adc_IL1[2], adc_VC1[2], adc_IO1[2];
	static Uint16 adc_IL2[2], adc_VC2[2], adc_IO2[2];

	adc_IL1[0] = AdcaResultRegs.ADCRESULT0;
	adc_IL1[1] = AdcbResultRegs.ADCRESULT0;
	adc_IL2[0] = AdcaResultRegs.ADCRESULT1;
	adc_IL2[1] = AdcbResultRegs.ADCRESULT1;

	adc_VC1[0] = AdcaResultRegs.ADCRESULT2;
	adc_VC1[1] = AdcbResultRegs.ADCRESULT2;
	adc_VC2[0] = AdcaResultRegs.ADCRESULT3;
	adc_VC2[1] = AdcbResultRegs.ADCRESULT3;

	adc_IO1[0] = AdcaResultRegs.ADCRESULT4;
	adc_IO1[1] = AdcdResultRegs.ADCRESULT4;
	adc_IO2[0] = AdcaResultRegs.ADCRESULT5;
	adc_IO2[1] = AdcdResultRegs.ADCRESULT5;

	//Auto-calibrations
	if(enable == false)
	{
		for(i=0; i<=1; i++) IL1_bias[i] = LPF(IL1_bias[i], 5.0f, (float32)(adc_IL1[i]));
		for(i=0; i<=1; i++) VC1_bias[i] = LPF(VC1_bias[i], 5.0f, (float32)(adc_VC1[i]));
		for(i=0; i<=1; i++) IO1_bias[i] = LPF(IO1_bias[i], 5.0f, (float32)(adc_IO1[i]));
		for(i=0; i<=1; i++) IL2_bias[i] = LPF(IL2_bias[i], 5.0f, (float32)(adc_IL2[i]));
		for(i=0; i<=1; i++) VC2_bias[i] = LPF(VC2_bias[i], 5.0f, (float32)(adc_VC2[i]));
		for(i=0; i<=1; i++) IO2_bias[i] = LPF(IO2_bias[i], 5.0f, (float32)(adc_IO2[i]));
	}

	for(i=0; i<=1; i++) states1->IL_abc[i] = ((float32)(adc_IL1[i]) - IL1_bias[i]) * IL_CONVERSION1;
	for(i=0; i<=1; i++) states1->VC_abc[i] = ((float32)(adc_VC1[i]) - VC1_bias[i]) * VC_CONVERSION;
	for(i=0; i<=1; i++) states1->IO_abc[i] = ((float32)(adc_IO1[i]) - IO1_bias[i]) * IO_CONVERSION1;

	for(i=0; i<=1; i++) states2->IL_abc[i] = ((float32)(adc_IL2[i]) - IL2_bias[i]) * IL_CONVERSION2;
	for(i=0; i<=1; i++) states2->VC_abc[i] = ((float32)(adc_VC2[i]) - VC2_bias[i]) * VC_CONVERSION;
	for(i=0; i<=1; i++) states2->IO_abc[i] = ((float32)(adc_IO2[i]) - IO2_bias[i]) * IO_CONVERSION2;


}

#pragma CODE_SECTION(Power_caculation, "ramfuncs")
void Power_caculation(float32 PQ[2], float32 PQ_raw[2],  const float32 IO_dq[2], const float32 VC_dq[2])
{
	Uint16 i;
	PQ_raw[0] = ( VC_dq[0]*IO_dq[0] + VC_dq[1]*IO_dq[1])/6692.9f;
	PQ_raw[1] = (-VC_dq[0]*IO_dq[1] + VC_dq[1]*IO_dq[0])/6692.9f;
	for(i=0; i<=1; i++) PQ[i] = LPF(PQ[i], WC, PQ_raw[i]);
}


#pragma CODE_SECTION(Abc2dq_fast, "ramfuncs")
void Abc2dq_fast(float32 dq[2], const float32 abc[2], const float32 table[2])
{

//	dq[0] = (abc[0]*(table[0]-table[2]) - abc[1]*table[1]*2.0f*SIN120)*0.66667f;
	dq[0] = abc[0]*table[0] - table[1]*0.577350269189626f*(abc[0]+ 2*abc[1]);

//	dq[1] = (abc[0]*(table[1]-table[3]) + abc[1]*table[0]*2.0f*SIN120)*0.66667f;
	dq[1] = abc[0]*table[1] + table[0]*0.577350269189626f*(abc[0]+ 2*abc[1]);
}

//#pragma CODE_SECTION(notch, "ramfuncs")
// Filter design is based on http://dsp.stackexchange.com/questions/31028/transfer-function-of-second-order-notch-filter/31030
//float32 notch(float32 u[2], float32 y[2], const float32 omega, const float32 a, const float32 in)
//{
//	float32 wn, out;
//	wn = omega*ISR_PERIOD  ;
//	out = in - 2.0f*cosf(wn)*u[1] + u[0] + 2.0f*a*cosf(wn)*y[1] - a*a*y[0];
//
//	u[0] = u[1];
//	u[1] = in;
//	y[0] = y[1];
//	y[1] = out;
//
//	return out;
//}

//LPF: a/(s+a)
#pragma CODE_SECTION(LPF, "ramfuncs")
float32 LPF(const float32 y_past, const float32 a, const float32 in)
{
	float32 out;
//	expAT = expf(-a*ISR_PERIOD);
//	expAT = 1 - a*ISR_PERIOD;
	out = in*(a*ISR_PERIOD) + y_past*(1 - a*ISR_PERIOD);

	return out;
}
