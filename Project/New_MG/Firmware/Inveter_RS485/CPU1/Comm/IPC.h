#define SIZEOFIPC_TX 5
#define CMD_START 111
#define CMD_STOP 222
#define CMD_RM 99
#define CMD_XM 100
#define CMD_KP 101
#define CMD_KQ 102
#define CMD_VREF 103
#define CMD_SI 104
#define CMD_DT 199
#define CMD_DAC 200
#define CMD_SC 201
#define CMD_NTH 0

typedef struct{
    Uint16 P;
    Uint16 Q;
    Uint16 V;
    Uint16 w;
} struct_IPC_tx;

typedef struct{
    Uint16 vref;
	Uint16 kp;
	Uint16 kq;
	Uint16 xm;
	Uint16 rm;
	Uint16 si;
	Uint16 dt;
	Uint16 sc_enable;
	Uint16 dac;
	Uint16 V_avg;
	Uint16 w_avg;
	Uint16 P_ref;
	Uint16 Q_ref;
} struct_IPC_rx;

void IPC_TX(Uint16 * array);
void Decoder_Process_IPC (Uint16 * array);
void Reset_remote_IPC(void);
void Grant_meomory_access(void);
