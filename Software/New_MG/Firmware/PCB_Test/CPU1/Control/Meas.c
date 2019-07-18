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
extern struct_control_states control_states1;
struct_meas_states meas_states1;

//***********************************************************************//
//                          F u n c t i o n s                            //
//***********************************************************************//
#pragma CODE_SECTION(Measurement_step, "ramfuncs")
void Measurement_step(const bool enable)
{

	meas_states1.theta = Oscillator(enable, meas_states1.theta, control_states1.omega);

	static struct_abc_states abc_states1;
	ADC_process(enable, &abc_states1, &meas_states1);

	float32 table[2]; //sin_theta, cos_theta, sin_theta120, cos_theta120;
	table[0] = sinf(meas_states1.theta);
	table[1] = cosf(meas_states1.theta);

//	Abc2dq(meas_states1.IL_dq,  abc_states1.IL_abc, meas_states1.theta);
//	Abc2dq(meas_states1.VC_dq,  abc_states1.VC_abc, meas_states1.theta);
//	Abc2dq(meas_states1.IO_dq,  abc_states1.IO_abc, meas_states1.theta);
	Abc2dq_fast(meas_states1.IL_dq, abc_states1.IL_abc, table);
	Abc2dq_fast(meas_states1.VC_dq, abc_states1.VC_abc, table);
	Abc2dq_fast(meas_states1.IO_dq, abc_states1.IO_abc, table);


	Power_caculation(meas_states1.PQ, meas_states1.IO_dq, meas_states1.VC_dq);

//    DACA(meas_states1.PQ[0], 1.0f);
//    DACB(meas_states1.PQ[1], 1.0f);
}

#pragma CODE_SECTION(Oscillator, "ramfuncs")
float32 Oscillator(const bool enable, float32 theta, const float32 omega)
{
	theta = (enable? theta + omega*ISR_PERIOD : 0);
	theta = (theta >= 2*PI? theta - 2*PI: theta);
	return theta;
}

#pragma CODE_SECTION(ADC_process, "ramfuncs")
void ADC_process(const bool enable, struct_abc_states * states1, struct_meas_states * m_states)
{
	Uint16 i;
	static float32 IL1_bias[2] = {4096.0f, 4096.0f}, VC1_bias[3] = {2048.0f, 2048.0f, 2048.0f}, IO1_bias[2] = {2048.0f, 2048.0f};
	static Uint16 adc_IL1[2], adc_VC1[3], adc_IO1[2], adc_Vdc, dummy;
	static float32 VN = 0.0f;

	adc_IL1[0] = AdcaResultRegs.ADCRESULT1 + AdcaResultRegs.ADCRESULT1;
	adc_IL1[1] = AdccResultRegs.ADCRESULT1 + AdccResultRegs.ADCRESULT1;

	adc_VC1[0] = AdcaResultRegs.ADCRESULT2;
	adc_VC1[1] = AdccResultRegs.ADCRESULT2;
	adc_VC1[2] = AdcdResultRegs.ADCRESULT2;

	adc_IO1[0] = AdcbResultRegs.ADCRESULT3;
	adc_IO1[1] = AdccResultRegs.ADCRESULT3;

	adc_Vdc =  AdccResultRegs.ADCRESULT4;
	dummy = AdcaResultRegs.ADCRESULT4;

	//Auto-calibrations
	if(enable == false)
	{
		for(i=0; i<=1; i++) IL1_bias[i] = LPF(IL1_bias[i], 5.0f, (float32)(adc_IL1[i]));
		for(i=0; i<=2; i++) VC1_bias[i] = LPF(VC1_bias[i], 5.0f, (float32)(adc_VC1[i]));
		for(i=0; i<=1; i++) IO1_bias[i] = LPF(IO1_bias[i], 5.0f, (float32)(adc_IO1[i]));
	}

	for(i=0; i<=1; i++) states1->IL_abc[i] = 0.5f*((float32)(adc_IL1[i]) - IL1_bias[i]) * IL_CONVERSION;
	for(i=0; i<=2; i++) states1->VC_abc[i] = ((float32)(adc_VC1[i]) - VC1_bias[i]) * VC_CONVERSION;
	for(i=0; i<=1; i++) states1->IO_abc[i] = ((float32)(adc_IO1[i]) - IO1_bias[i]) * IO_CONVERSION;

	m_states->Vdc = (float32)(adc_Vdc) * VDC_CONVERSION;

//	Correction for zero-sequence bias
	VN = (states1->VC_abc[0]+states1->VC_abc[1]+states1->VC_abc[2])*0.333333333333f;
	for(i=0; i<=1; i++) states1->VC_abc[i] -= VN;

//    DACA(states1->VC_abc[0], 25.0f);
//    DACB(states1->VC_abc[1], 25.0f);
//	DACC(m_states->Vdc, 75.0f);


}

#pragma CODE_SECTION(Power_caculation, "ramfuncs")
void Power_caculation(float32 PQ[2], const float32 IO_dq[2], const float32 VC_dq[2])
{
	Uint16 i;
	float32 PQ_temp[2];
	PQ_temp[0] =  (VC_dq[0])*(IO_dq[0]) + (VC_dq[1])*(IO_dq[1]);
	PQ_temp[1] = (-VC_dq[0])*(IO_dq[1]) + (VC_dq[1])*(IO_dq[0]);
	for(i=0; i<=1; i++) PQ[i] = LPF(PQ[i], WC, PQ_temp[i]/25.0f);
}

//#pragma CODE_SECTION(Abc2dq, "ramfuncs")
//void Abc2dq(float32 dq[2], const float32 abc[2], const float32 theta)
//{
//	dq[0] = (abc[0]*(sinf(theta)-sinf(theta+PHASE_120)) - abc[1]*cosf(theta)*2.0f*SIN120)*0.66667f;
//	dq[1] = (abc[0]*(cosf(theta)-cosf(theta+PHASE_120)) + abc[1]*sinf(theta)*2.0f*SIN120)*0.66667f;
//}

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
