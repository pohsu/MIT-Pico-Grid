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
extern struct_control_states control_states;
struct_meas_states meas_states;

//***********************************************************************//
//                          F u n c t i o n s                            //
//***********************************************************************//
#pragma CODE_SECTION(Measurement_step, "ramfuncs")
void Measurement_step(const bool enable)
{
	struct_abc_states abc_states;
	Oscillator();
	ADC_process(enable, &abc_states);
	Abc2dq(meas_states.IL_dq, abc_states.IL_abc, meas_states.theta);
	Abc2dq(meas_states.VC_dq, abc_states.VC_abc, meas_states.theta);
	Abc2dq(meas_states.IO_dq, abc_states.IO_abc, meas_states.theta);
	Power_caculation(meas_states.PQ, meas_states.IO_dq, meas_states.VC_dq);
}

#pragma CODE_SECTION(Oscillator, "ramfuncs")
void Oscillator(void)
{
	meas_states.theta = (meas_states.theta >= 2*PI? 0.0f: meas_states.theta);
	meas_states.theta = meas_states.theta + control_states.omega*ISR_PERIOD;
}

#pragma CODE_SECTION(ADC_process, "ramfuncs")
void ADC_process(const bool enable, struct_abc_states * states)
{
	Uint16 i;
	static float32 IL_bias[2] = {4096.0f, 4096.0f}, VC_bias[2] = {2048.0f, 2048.0f}, IO_bias[2] = {2048.0f, 2048.0f};
	static Uint16 adc_IL[2], adc_VC[2], adc_IO[2];

	adc_IL[0] = AdcaResultRegs.ADCRESULT0 + AdcaResultRegs.ADCRESULT1;
	adc_IL[1] = AdcbResultRegs.ADCRESULT0 + AdcbResultRegs.ADCRESULT1;
	adc_VC[0] = AdcaResultRegs.ADCRESULT2;
	adc_VC[1] = AdcbResultRegs.ADCRESULT2;
	adc_IO[0] = AdcaResultRegs.ADCRESULT3;
	adc_IO[1] = AdcbResultRegs.ADCRESULT3;

	//Auto-calibrations
	if(enable == false)
	{
		for(i=0; i<=1; i++) IL_bias[i] = LPF(IL_bias[i], 5.0f, (float32)(adc_IL[i]));
		for(i=0; i<=1; i++) VC_bias[i] = LPF(VC_bias[i], 5.0f, (float32)(adc_VC[i]));
		for(i=0; i<=1; i++) IO_bias[i] = LPF(IO_bias[i], 5.0f, (float32)(adc_IO[i]));
	}

	for(i=0; i<=1; i++) states->IL_abc[i] = 0.5f*((float32)(adc_IL[i]) - IL_bias[i]) * IL_CONVERSION;
	for(i=0; i<=1; i++) states->VC_abc[i] = ((float32)(adc_VC[i]) - VC_bias[i]) * VC_CONVERSION;
	for(i=0; i<=1; i++) states->IO_abc[i] = ((float32)(adc_IO[i]) - IO_bias[i]) * IO_CONVERSION;

}

#pragma CODE_SECTION(Power_caculation, "ramfuncs")
void Power_caculation(float32 PQ[2], const float32 IO_dq[2], const float32 VC_dq[2])
{
	Uint16 i;
	float32 PQ_temp[2];
	PQ_temp[0] = VC_dq[0]*IO_dq[0] + VC_dq[1]*IO_dq[1];
	PQ_temp[1] = -VC_dq[0]*IO_dq[1] + VC_dq[1]*IO_dq[0];
	for(i=0; i<=1; i++) PQ[i] = LPF(PQ[i], WC, 1.5*PQ_temp[i]);
}



#pragma CODE_SECTION(Abc2dq, "ramfuncs")
void Abc2dq(float32 dq[2], const float32 abc[2], const float32 theta)
{
	dq[0] = (abc[0]*(sinf(theta)-sinf(theta+PHASE_120)) - abc[1]*cosf(theta)*2.0f*SIN120)*0.66667f;
	dq[1] = (abc[0]*(cosf(theta)-cosf(theta+PHASE_120)) + abc[1]*sinf(theta)*2.0f*SIN120)*0.66667f;
}

#pragma CODE_SECTION(notch, "ramfuncs")
// Filter design is based on http://dsp.stackexchange.com/questions/31028/transfer-function-of-second-order-notch-filter/31030
float32 notch(float32 u[2], float32 y[2], const float32 omega, const float32 a, const float32 in)
{
	float32 wn, out;
	wn = omega*ISR_PERIOD  ;
	out = in - 2.0f*cosf(wn)*u[1] + u[0] + 2.0f*a*cosf(wn)*y[1] - a*a*y[0];

	u[0] = u[1];
	u[1] = in;
	y[0] = y[1];
	y[1] = out;

	return out;
}

#pragma CODE_SECTION(LPF, "ramfuncs")
float32 LPF(const float32 y_past, const float32 a, const float32 in)
{
	float32 expAT, out;
	expAT = expf(-a*ISR_PERIOD);
	out = in*(1-expAT) + y_past*expAT;

	return out;
}
