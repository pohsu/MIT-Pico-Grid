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

	Abc2dq(meas_states1.IL_dq, abc_states1.IL_abc, meas_states1.theta);
	Abc2dq(meas_states1.VC_dq, abc_states1.VC_abc, meas_states1.theta);
	Abc2dq(meas_states1.IO_dq, abc_states1.IO_abc, meas_states1.theta);
	Abc2dq(meas_states2.IL_dq, abc_states2.IL_abc, meas_states2.theta);
	Abc2dq(meas_states2.VC_dq, abc_states2.VC_abc, meas_states2.theta);
	Abc2dq(meas_states2.IO_dq, abc_states2.IO_abc, meas_states2.theta);



	Power_caculation(meas_states1.PQ, meas_states1.IO_dq, meas_states1.VC_dq);
	Power_caculation(meas_states2.PQ, meas_states2.IL_dq, meas_states2.VC_dq);

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

	adc_IL1[0] = AdcaResultRegs.ADCRESULT0 + AdcaResultRegs.ADCRESULT2;
	adc_IL1[1] = AdcbResultRegs.ADCRESULT0 + AdcbResultRegs.ADCRESULT2;
	adc_IL2[0] = AdcaResultRegs.ADCRESULT1 + AdcaResultRegs.ADCRESULT3;
	adc_IL2[1] = AdcbResultRegs.ADCRESULT1 + AdcbResultRegs.ADCRESULT3;

	adc_VC1[0] = AdcaResultRegs.ADCRESULT4;
	adc_VC1[1] = AdcbResultRegs.ADCRESULT4;
	adc_VC2[0] = AdcaResultRegs.ADCRESULT5;
	adc_VC2[1] = AdcbResultRegs.ADCRESULT5;

	adc_IO1[0] = AdcaResultRegs.ADCRESULT6;
	adc_IO1[1] = AdcdResultRegs.ADCRESULT6;
	adc_IO2[0] = AdcaResultRegs.ADCRESULT7;
	adc_IO2[1] = AdcdResultRegs.ADCRESULT7;

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

	for(i=0; i<=1; i++) states1->IL_abc[i] = 0.5f*((float32)(adc_IL1[i]) - IL1_bias[i]) * IL_CONVERSION;
	for(i=0; i<=1; i++) states1->VC_abc[i] = ((float32)(adc_VC1[i]) - VC1_bias[i]) * VC_CONVERSION;
	for(i=0; i<=1; i++) states1->IO_abc[i] = ((float32)(adc_IO1[i]) - IO1_bias[i]) * IO_CONVERSION;

	for(i=0; i<=1; i++) states2->IL_abc[i] = 0.5f*((float32)(adc_IL2[i]) - IL2_bias[i]) * IL_CONVERSION;
	for(i=0; i<=1; i++) states2->VC_abc[i] = ((float32)(adc_VC2[i]) - VC2_bias[i]) * VC_CONVERSION;
	for(i=0; i<=1; i++) states2->IO_abc[i] = ((float32)(adc_IO2[i]) - IO2_bias[i]) * IO_CONVERSION;


}

#pragma CODE_SECTION(Power_caculation, "ramfuncs")
void Power_caculation(float32 PQ[2], const float32 IO_dq[2], const float32 VC_dq[2])
{
	Uint16 i;
	float32 PQ_temp[2];
	PQ_temp[0] =  (VC_dq[0]/V_NOM)*(IO_dq[0]/I_NOM) + (VC_dq[1]/V_NOM)*(IO_dq[1]/I_NOM);
	PQ_temp[1] = (-VC_dq[0]/V_NOM)*(IO_dq[1]/I_NOM) + (VC_dq[1]/V_NOM)*(IO_dq[0]/I_NOM);
	for(i=0; i<=1; i++) PQ[i] = LPF(PQ[i], WC, PQ_temp[i]);
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
