#include "F28x_Project.h"
#include <stdint.h>
#include <math.h>
#include "..\Test_setting.h"
#include "..\Peripheral\Peripheral.h"
#include "..\Comm\Comm.h"
#include "Meas.h"
#include "control.h"

//***********************************************************************//
//                     G l o b a l  V a r i a b l e s                    //
//***********************************************************************//
extern struct_meas_states meas_states1, meas_states2;
struct_control_states control_states1, control_states2;
extern struct_console_data console_data;

//***********************************************************************//
//                          F u n c t i o n s                            //
//***********************************************************************//
//#pragma CODE_SECTION(Control_step, "ramfuncs")
void Control_step(const float32 Droop[2], const float32 Xm[2], const bool enable)
{
	static float32 IL_PID_states1[2] = {0, 0};
//	static float32 VC_PID_states1[2] = {0, 0};
	static float32 LPF_damper_states1[2] = {0};


	Droop_control(enable, Droop, S1, &control_states1, &meas_states1);
	IIM(enable, Xm, S1, &control_states1, &meas_states1);
	Damper(enable, &control_states1, &meas_states1, LPF_damper_states1);

//	Virtual_component(enable, Xm[0], S1, &control_states1, &meas_states1);
//	VC_control(enable, &control_states1, &meas_states1, VC_PID_states1);
//
//	control_states1.IL_dq_ref[0] = 10.0f;
//	control_states1.IL_dq_ref[1] = 0.0f;
	IL_control(enable, &control_states1, &meas_states1, IL_PID_states1);

//    control_states1.VINV_dq[0] = control_states1.VC_dq_ref[0];
//    control_states1.VINV_dq[1] = control_states1.VC_dq_ref[1];
	uDACA((control_states1.omega - 0.95*W_NOM)/W_NOM, 0.1f);
	DACB(meas_states1.PQ[0], 3.0f);


	VINV2Duty(&control_states1, &meas_states1);

}
#pragma CODE_SECTION(Droop_control, "ramfuncs")
void Droop_control(const bool enable, const float32 Droop[2], const float32 Sn, struct_control_states * c_states, struct_meas_states * m_states)
{
	c_states->omega = W_NOM - (enable? Droop[0] / Sn* W_NOM * m_states->PQ[0] : 0);
	c_states->VC_dq_ref[0] = V_NOM - (enable? Droop[1] / Sn * V_NOM * m_states->PQ[1]: 0);
	c_states->VC_dq_ref[1] = 0;

//	c_states->omega= (c_states->omega >  W_NOM*1.2?  W_NOM*1.2 : c_states->omega);
//	c_states->omega= (c_states->omega <  W_NOM*0.8?  W_NOM*0.8 : c_states->omega);
}

#pragma CODE_SECTION(IIM, "ramfuncs")
void IIM(const bool enable, const float32 Xm[2], const float32 Sn, struct_control_states * c_states, struct_meas_states * m_states)
{
	float32 IIM_dq[2] = {0}, X = 0, R = 0, L = 0;
	Uint16 i = 0;
	X = Xm[0] / Sn;
	R = Xm[1] / Sn;
	L = X / W_NOM ;

	// Reset
	if (enable == false){
		for (i = 0;i<=1;i++){
			c_states->LPF_IIM[i] = 0;
			c_states->IL_dq_ref[i] = 0;
		}
	}

	//LPF
//	IIM_dq[0] =  c_states->VC_dq_ref[0] - m_states->VC_dq[0] + X * c_states->IL_dq_ref[1];
//	IIM_dq[1] =  c_states->VC_dq_ref[1] - m_states->VC_dq[1] - X * c_states->IL_dq_ref[0];

	IIM_dq[0] =  c_states->VC_dq_ref[0] - m_states->VC_dq[0] + X * c_states->LPF_IIM[1]- R * c_states->LPF_IIM[0];
	IIM_dq[1] =  c_states->VC_dq_ref[1] - m_states->VC_dq[1] - X * c_states->LPF_IIM[0]- R * c_states->LPF_IIM[1];

	for (i = 0;i<=1;i++){
//		c_states->LPF_IIM[i] = LPF(c_states->LPF_IIM[i], ETA * W_NOM, IIM_dq[i]);
		c_states->LPF_IIM[i] += ISR_PERIOD*IIM_dq[i]/L;
	}

	// Iref
//	for (i = 0;i<=1;i++) c_states->IL_dq_ref[i] = (1-FV) * 1/R * c_states->LPF_IIM[i];
	for (i = 0;i<=1;i++) c_states->IL_dq_ref[i] = (1-FV) * c_states->LPF_IIM[i];

	c_states->IL_dq_ref[0] += m_states->IO_dq[0]*FV - c_states->omega*CF*m_states->VC_dq[1];
	c_states->IL_dq_ref[1] += m_states->IO_dq[1]*FV + c_states->omega*CF*m_states->VC_dq[0];

//	for (i = 0;i<=1;i++){
//		c_states->IL_dq_ref[i] = (c_states->IL_dq_ref[i] >  25.0f?  25.0f : c_states->IL_dq_ref[i]);
//		c_states->IL_dq_ref[i] = (c_states->IL_dq_ref[i] < -25.0f? -25.0f : c_states->IL_dq_ref[i]);
//	}

}

#pragma CODE_SECTION(Damper, "ramfuncs")
void Damper(const bool enable, struct_control_states * c_states, struct_meas_states * m_states, float32 LPF_state[2])
{
	Uint16 i = 0;

	// Reset damper
	if (enable == false){
		for (i = 0;i<=1;i++){
			LPF_state[i] = 0;
		}
	}
	for (i = 0;i<=1;i++){
		LPF_state[i] = LPF(LPF_state[i], WDAMP, m_states->VC_dq[i]);
		c_states->damper_dq[i] = (m_states->VC_dq[i] - LPF_state[i]) * GDAMP;
	}
	c_states->IL_dq_ref[0] -= (1-FV) * c_states->damper_dq[0];//*(1-FV);// + m_states->VC_dq[0] / RNL;
	c_states->IL_dq_ref[1] -= (1-FV) * c_states->damper_dq[1];//*(1-FV);// + m_states->VC_dq[1] / RNL;

//	for (i = 0;i<=1;i++){
//		c_states->IL_dq_ref[i] = (c_states->IL_dq_ref[i] >  18.0f?  18.0f : c_states->IL_dq_ref[i]);
//		c_states->IL_dq_ref[i] = (c_states->IL_dq_ref[i] < -18.0f? -18.0f : c_states->IL_dq_ref[i]);
//	}

}

#pragma CODE_SECTION(IL_control, "ramfuncs")
void IL_control(const bool enable, struct_control_states * c_states, struct_meas_states * m_states, float32 PID_states[2])
{
	float32 ff_dq[2] = {0}, error[2] = {0};
	Uint16 i;

	ff_dq[0] = m_states->VC_dq[0] - c_states->omega*LF*m_states->IL_dq[1];
	ff_dq[1] = m_states->VC_dq[1] + c_states->omega*LF*m_states->IL_dq[0];

	for (i = 0;i<=1;i++) error[i] = c_states->IL_dq_ref[i] - m_states->IL_dq[i];
	PID_dq(c_states->VINV_dq, PID_states, error, KPC, KIC);

	// Reset PID integrator
	if (enable == false)
	{
		for (i = 0;i<=1;i++) PID_states[i] = 0;
	}

	for (i = 0;i<=1;i++) c_states->VINV_dq[i] += ff_dq[i];

//	for (i = 0;i<=1;i++){
//		c_states->VINV_dq[i] = (c_states->VINV_dq[i] >  V_NOM*1.2f?  V_NOM*1.2f : c_states->VINV_dq[i]);
//		c_states->VINV_dq[i] = (c_states->VINV_dq[i] <  -V_NOM*1.2f?  -V_NOM*1.2f : c_states->VINV_dq[i]);
//	}

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
void VINV2Duty (struct_control_states * c_states, struct_meas_states * m_states)
{
	float32 abc[3];
//	c_states->VINV_dq[0] = notch(c_states->Notch_ud, c_states->Notch_yd, c_states->omega, 0.9f, c_states->VINV_dq[0]);
//	c_states->VINV_dq[1] = notch(c_states->Notch_uq, c_states->Notch_yq, c_states->omega, 0.9f, c_states->VINV_dq[1]);
	float32 table[2] = {0,0};
	table[0] = sinf(m_states->theta);
	table[1] = cosf(m_states->theta);

	dq2abc_fast(abc, c_states->VINV_dq, table);

	c_states->Duty[0] = (Uint16)( (abc[0]/VDC + 0.5f) * PWM_PERIOD);
	c_states->Duty[1] = (Uint16)( (abc[1]/VDC + 0.5f) * PWM_PERIOD);
	c_states->Duty[2] = (Uint16)( (abc[2]/VDC + 0.5f) * PWM_PERIOD);

}

#pragma CODE_SECTION(dq2abc_fast, "ramfuncs")
void dq2abc_fast(float32 abc[3], const float32 dq[2], const float32 table[2])
{
//	abc[0] = dq[0]*sinf(theta) + dq[1]*cosf(theta);
//	abc[1] = dq[0]*sinf(theta-PHASE_120) + dq[1]*cosf(theta-PHASE_120);
//	abc[2] = dq[0]*sinf(theta+PHASE_120) + dq[1]*cosf(theta+PHASE_120);
	abc[0] = dq[0]*table[0] + dq[1]*table[1];
	abc[1] = 0.5f*( table[0]*(-dq[0]+1.732050807568f*dq[1]) + table[1]*(-1.732050807568f*dq[0] - dq[1]));
	abc[2] = 0.5f*( table[0]*(-dq[0]-1.732050807568f*dq[1]) + table[1]*(+1.732050807568f*dq[0] - dq[1]));
}


#pragma CODE_SECTION(Virtual_component, "ramfuncs")
void Virtual_component(const bool enable, const float32 Xm, const float32 Sn, struct_control_states * c_states, struct_meas_states * m_states)
{
    float32 X = 0, L = 0, R = 0;
    X = Xm /Sn;
    L = X / W_NOM*1;
    R = 0.0*X;
    if (enable)
    {
        // Virtual X
        c_states->VC_dq_ref[0] = c_states->VC_dq_ref[0] + X * m_states->IO_dq[1] - R * m_states->IO_dq[0];
        c_states->VC_dq_ref[1] = c_states->VC_dq_ref[1] - X * m_states->IO_dq[0] - R * m_states->IO_dq[1];
        //Filter X
//      c_states->VC_dq_ref[0] += X * LPF(c_states->LPF_outL[1], WF, m_states->IO_dq[1]);
//      c_states->VC_dq_ref[1] -= X * LPF(c_states->LPF_outL[0], WF, m_states->IO_dq[0]);

        // Virtual L
//      c_states->LPF_outL[0] = LPF(c_states->LPF_outL[0], WF, m_states->IO_dq[0]);
//      c_states->LPF_outL[1] = LPF(c_states->LPF_outL[1], WF, m_states->IO_dq[1]);
//      c_states->VC_dq_ref[0] = c_states->VC_dq_ref[0] - L * WF*(m_states->IO_dq[0]-c_states->LPF_outL[0]);
//      c_states->VC_dq_ref[1] = c_states->VC_dq_ref[1] - L * WF*(m_states->IO_dq[1]-c_states->LPF_outL[1]);
    }
    else
    {
        c_states->LPF_outL[0] = 0;
        c_states->LPF_outL[1] = 0;
    }
}


#pragma CODE_SECTION(VC_control, "ramfuncs")
void VC_control(const bool enable, struct_control_states * c_states, struct_meas_states * m_states, float32 PID_states[2])
{
    float32 ff_dq[2] = {0}, error[2] = {0};
    Uint16 i;

    ff_dq[0] = m_states->IO_dq[0]*FV - c_states->omega*CF*m_states->VC_dq[1];
    ff_dq[1] = m_states->IO_dq[1]*FV + c_states->omega*CF*m_states->VC_dq[0];

    for (i = 0;i<=1;i++) error[i] = c_states->VC_dq_ref[i] - m_states->VC_dq[i];

    PID_dq(c_states->IL_dq_ref, PID_states, error, KPV, KIV);

    // Reset PID integrator
    if (enable == false){
        for (i = 0;i<=1;i++) PID_states[i] = 0;
    }

    for (i = 0;i<=1;i++) c_states->IL_dq_ref[i] += ff_dq[i];

//  c_states->IL_dq_ref[0] = I_NOM;
//  c_states->IL_dq_ref[1] = 0;

//  for (i = 0;i<=1;i++){
//      c_states->IL_dq_ref[i] = (c_states->IL_dq_ref[i] >  25.0f?  25.0f : c_states->IL_dq_ref[i]);
//      c_states->IL_dq_ref[i] = (c_states->IL_dq_ref[i] < -25.0f? -25.0f : c_states->IL_dq_ref[i]);
//  }
}
