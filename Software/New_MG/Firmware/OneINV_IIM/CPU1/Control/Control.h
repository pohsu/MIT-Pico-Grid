#define Zb 29.04f
#define LF 1.0e-3f
#define CF 30e-6f
#define FV 0.75f
#define KPC 6.0f
#define KIC 20000.0f
#define KPV 0.05f
#define KIV 300.0f
#define VDC 1000.0f
#define Sb 5e3
#define W_NOM 314.16f
#define V_NOM 311.12f
#define I_NOM 10.7137f
#define S1 1.0f
#define ETA 1.0f
#define WDAMP 800.0f // 1/Cd/Rd
#define GDAMP 0.2f   // 1/Rd
#define RNL 100.0f

typedef struct{
  float32 omega;
  float32 VC_dq_ref[2];
  float32 IL_dq_ref[2];
  float32 VINV_dq[2];
  float32 LPF_IIM[2];
  float32 damper_dq[2];
  float32 LPF_outL[2];
  Uint16 Duty[3];
}struct_control_states;

void Control_step(const float32 Droop[2], const float32 Xm[2], const bool enable);
void Droop_control(const bool enable, const float32 Droop[2], const float32 Sn, struct_control_states * c_states, struct_meas_states * m_states);
void IIM(const bool enable, const float32 Xm[2], const float32 Sn, struct_control_states * c_states, struct_meas_states * m_states);
void Damper(const bool enable, struct_control_states * c_states, struct_meas_states * m_states, float32 LPF_state[2]);
void IL_control(const bool enable, struct_control_states * c_states, struct_meas_states * m_states, float32 IL_PID_states[2]);
void PID_dq(float32 out[2], float32 PID_states[2], const float32 error[2], const float32 kp, const float32 ki);
void VINV2Duty (struct_control_states * c_states, struct_meas_states * m_states);
void dq2abc_fast(float32 abc[3], const float32 dq[2], const float32 table[2]);
void Virtual_component(const bool enable, const float32 Xm, const float32 Sn, struct_control_states * c_states, struct_meas_states * m_states);
void VC_control(const bool enable, struct_control_states * c_states, struct_meas_states * m_states, float32 VC_PID_states[2]);

