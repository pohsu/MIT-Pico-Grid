#define Zb 29.04f
#define LF 1.0e-3f
#define CF 30e-6f
#define LC 3.5e-4f
#define RC 8e-2f
#define FV 0.5f
#define KPC 8.0f //8.0f
#define KIC 18000.0f //18000.0f
#define KPV 0.15f //0.15
#define KIV 350.0f //350
#define VDC 120.0f
#define Sb 174.24f
#define W_NOM 314.15926f
#define V_NOM 58.08f
#define I_NOM 2.0f
#define P_NOM 116.16f
#define WF 2000.0f
#define WDAMP 400.0f // 1/Cd/Rd 800.0f
#define GDAMP 0.2f   // 1/Rd 0.2f
#define I_LIMIT 8.0f
#define RNL 100.0f
#define KI_SEC_W 1000.0f
#define KI_SEC_V 500.0f
#define R_P 0.05f
#define R_Q 0.05f


typedef struct{
  float32 omega;
  float32 VC_dq_ref[2];
  float32 IL_dq_ref[2];
  float32 VINV_dq[2];
  float32 LPF_outL[2];
  float32 LPF_IIM[2];
  float32 damper_dq[2];
  Uint16 Duty[3];
}struct_control_states;

void Control_step(float32 Droop[2], float32 XRm[2], float32 vref, float32 Si, const bool enable, const Uint16 dac);
void Droop_control(const bool enable, const float32 Droop[2], const float32 vref, const float32 Si, struct_control_states * c_states, struct_meas_states * m_states);
//void Virtual_component(const bool enable, const float32 Xm, const float32 Sn, struct_control_states * c_states, struct_meas_states * m_states);
//void VC_control(const bool enable, struct_control_states * c_states, struct_meas_states * m_states, float32 VC_PID_states[2]);
void IIM(const bool enable, const float32 XRm[2], const float32 Si, struct_control_states * c_states, struct_meas_states * m_states);
void Damper(const bool enable, struct_control_states * c_states, struct_meas_states * m_states, float32 LPF_state[2]);
void limiter(float32 dq[2], const float32 limit);
void IL_control(const bool enable, struct_control_states * c_states, struct_meas_states * m_states, float32 IL_PID_states[2]);
void PID_dq(float32 out[2], float32 PID_states[2], const float32 error[2], const float32 kp, const float32 ki);
void VINV2Duty (struct_control_states * c_states, struct_meas_states * m_states);
void dq2abc_fast(float32 abc[3], const float32 dq[2], const float32 table[2]);
void dac_report(const Uint16 dac, const struct_control_states * c_states, const struct_meas_states * m_states);

