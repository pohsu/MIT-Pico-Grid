#include "F28x_Project.h"
#include <stdint.h>
#include <math.h>
#include "..\Test_setting.h"
#include "..\Peripheral\Peripheral.h"
#include "Meas.h"
#include "control.h"

//***********************************************************************//
//                     G l o b a l  V a r i a b l e s                    //
//***********************************************************************//
extern struct_meas_states meas_states;
struct_control_states control_states;

//***********************************************************************//
//                          F u n c t i o n s                            //
//***********************************************************************//
#pragma CODE_SECTION(Control_step, "ramfuncs")
void Control_step(const float32 dq_ref[2], const bool enable)
{
	VC_control(enable, dq_ref);
	IL_control(enable);
	VINV2Duty();
	DACA(control_states.IL_dq_ref[0], 22.5f);
	DACB(meas_states.IL_dq[0], 22.5f);
	DACC(meas_states.IL_dq[1], 22.5f);
}

#pragma CODE_SECTION(VC_control, "ramfuncs")
void VC_control(const bool enable, const float32 VC_dq_ref[2])
{
	float32 ff_dq[2] = {0}, error[2] = {0};
	static float32 VC_PID_states[2] = {0};
	Uint16 i;

	ff_dq[0] = meas_states.IO_dq[0]*VC_FF_GAIN - 377.0f*CF*meas_states.VC_dq[1];
	ff_dq[1] = meas_states.IO_dq[1]*VC_FF_GAIN + 377.0f*CF*meas_states.VC_dq[0];

	for (i = 0;i<=1;i++) error[i] = VC_dq_ref[i] - meas_states.VC_dq[i];
	PID_dq(control_states.IL_dq_ref, VC_PID_states, error, KPV, KIV);

	// Reset PID integrator
	if (enable == false){
		for (i = 0;i<=1;i++) VC_PID_states[i] = 0;
	}

	for (i = 0;i<=1;i++) control_states.IL_dq_ref[i] += ff_dq[i];
}


#pragma CODE_SECTION(IL_control, "ramfuncs")
void IL_control(const bool enable)
{
	float32 ff_dq[2] = {0}, error[2] = {0};
	static float32 IL_PID_states[2] = {0};
	Uint16 i;

	ff_dq[0] = meas_states.VC_dq[0]*IL_FF_GAIN - 377.0f*LF*meas_states.IL_dq[1];
	ff_dq[1] = meas_states.VC_dq[1]*IL_FF_GAIN + 377.0f*LF*meas_states.IL_dq[0];

	for (i = 0;i<=1;i++) error[i] = control_states.IL_dq_ref[i] - meas_states.IL_dq[i];
	PID_dq(control_states.VINV_dq, IL_PID_states, error, KPC, KIC);

	// Reset PID integrator
	if (enable == false)
	{
		for (i = 0;i<=1;i++) IL_PID_states[i] = 0;
	}

	for (i = 0;i<=1;i++) control_states.VINV_dq[i] += ff_dq[i];

}

#pragma CODE_SECTION(PID_dq, "ramfuncs")
void PID_dq(float32 out[2], float32 PID_states[2], const float32 error[2], const float32 kp, const float32 ki)
{
	Uint16 i;
	for (i = 0;i<=1;i++)
	{
		PID_states[i] += ki*ISR_PERIOD*error[i];
		out[i] = kp*error[i] + PID_states[i];
	}
}

#pragma CODE_SECTION(VINV2Duty, "ramfuncs")
void VINV2Duty (void)
{
	float32 abc[3];
	dq2abc(abc, control_states.VINV_dq, meas_states.theta);

	control_states.Duty[0] = (Uint16)( (abc[0]/VDC + 0.5f) * PWM_PERIOD);
	control_states.Duty[1] = (Uint16)( (abc[1]/VDC + 0.5f) * PWM_PERIOD);
	control_states.Duty[2] = (Uint16)( (abc[2]/VDC + 0.5f) * PWM_PERIOD);

//	control_states.Duty[0] = (Uint16)( 0.5f* (0.8f*sinf(meas_states.theta) + 1.0f) * PWM_PERIOD);
//	control_states.Duty[1] = (Uint16)( 0.5f* (0.8f*sinf(meas_states.theta-PHASE_120) + 1.0f) * PWM_PERIOD);
//	control_states.Duty[2] = (Uint16)( 0.5f* (0.8f*sinf(meas_states.theta+PHASE_120) + 1.0f) * PWM_PERIOD);

}

#pragma CODE_SECTION(dq2abc, "ramfuncs")
void dq2abc(float32 abc[3], const float32 dq[2], const float32 theta)
{
	abc[0] = dq[0]*sinf(theta) + dq[1]*cosf(theta);
	abc[1] = dq[0]*sinf(theta-PHASE_120) + dq[1]*cosf(theta-PHASE_120);
	abc[2] = dq[0]*sinf(theta+PHASE_120) + dq[1]*cosf(theta+PHASE_120);
}
