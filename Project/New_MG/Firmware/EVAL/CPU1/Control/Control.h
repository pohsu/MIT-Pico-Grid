#define LF 1.5e-3f
#define CF 30e-6f
#define IL_FF_GAIN 1.0f
#define VC_FF_GAIN 0.75f
#define KPC 10.0f * 0.5f
#define KIC 16000.0f * 0.5f
#define KPV 0.05f * 0.5f
#define KIV 100.0f * 0.5f
#define VDC 900.0f
#define SN 10e3
#define KP  1e-2
#define KQ  1e-2
#define W_NOM 314.16f
#define V_NOM 380.0f

typedef struct{
  float32 omega;
  float32 VC_dq_ref[2];
  float32 IL_dq_ref[2];
  float32 VINV_dq[2];
  Uint16 Duty[3];
}struct_control_states;

void Control_step(const float32 dq_ref[2], const bool enable);
void Droop_control(const bool enable, const float32 FV_ref[2]);
void VC_control(const bool enable);
void IL_control(const bool enable);
void PID_dq(float32 out[2], float32 PID_states[2], const float32 error[2], const float32 kp, const float32 ki);
void VINV2Duty (void);
void dq2abc(float32 abc[3], const float32 dq[2], const float32 theta);
