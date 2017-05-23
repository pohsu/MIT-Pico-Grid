#define LF 1.5e-3f
#define CF 30e-6f
#define IL_FF_GAIN 1.0f
#define VC_FF_GAIN 0.75f
#define KPC 10.0f * 1.0f
#define KIC 16000.0f * 1.0f
#define KPV 0.05f * 1.0f
#define KIV 100.0f * 1.0f
#define VDC 1800.0f
#define SN 10e3
#define KP  1e-2
#define KQ  1e-2
#define W_NOM 314.16f
#define V_NOM 380.0f
#define I_NOM 17.54f
#define XM 0.5f
#define LM 0
#define WF 5000.0f
#define CS 0.008f
#define RS 0.5f
#define WRC 1/(CS*RS)

typedef struct{
  float32 omega;
  float32 VC_dq_ref[2];
  float32 IL_dq_ref[2];
  float32 VINV_dq[2];
  float32 LPF_outL[2];
  float32 LPF_outC[2];
  Uint16 Duty[3];
}struct_control_states;

void Control_step(const float32 dq_ref[2], const bool enable);
void Droop_control(const bool enable, const float32 FV_ref[2], struct_control_states * c_states, struct_meas_states * m_states);
void Virtual_component(const bool enable, struct_control_states * c_states, struct_meas_states * m_states);
void VC_control(const bool enable, struct_control_states * c_states, struct_meas_states * m_states, float32 VC_PID_states[2]);
void IL_control(const bool enable, struct_control_states * c_states, struct_meas_states * m_states, float32 IL_PID_states[2]);
void PID_dq(float32 out[2], float32 PID_states[2], const float32 error[2], const float32 kp, const float32 ki);
void VINV2Duty (struct_control_states * c_states, struct_meas_states * m_states);
void dq2abc(float32 abc[3], const float32 dq[2], const float32 theta);

