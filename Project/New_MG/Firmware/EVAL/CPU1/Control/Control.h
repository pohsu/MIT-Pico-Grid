#define LF 1.5e-3f
#define CF 30e-6f
#define IL_FF_GAIN 1.0f
#define VC_FF_GAIN 1.0f
#define KPC 10.0f
#define KIC 16000.0f
#define KPV 0.05f
#define KIV 100.0f
#define VDC 900.0f

typedef struct{
  float32 IL_dq_ref[2];
  float32 VINV_dq[2];
  Uint16 Duty[3];
}struct_control_states;

void Control_step(const float32 dq_ref[2], const bool enable);
void VC_control(const bool enable, const float32 VC_dq_ref[2]);
void IL_control(const bool enable);
void PID_dq(float32 out[2], float32 PID_states[2], const float32 error[2], const float32 kp, const float32 ki);
void VINV2Duty (void);
void dq2abc(float32 abc[3], const float32 dq[2], const float32 theta);
