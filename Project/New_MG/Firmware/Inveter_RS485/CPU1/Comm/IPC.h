#define SIZEOFIPC_TX 2
#define CMD_START 111
#define CMD_STOP 222
#define CMD_FREF 100
#define CMD_VREF 101

typedef struct{
    Uint16 volt;
    Uint16 freq;
} struct_IPC_tx;

typedef struct{
	Uint16 vref;
	Uint16 fref;
} struct_IPC_rx;

void IPC_TX(Uint16 * array);
void Decoder_Process_IPC (Uint16 * array);
void Reset_remote_IPC(void);
void Grant_meomory_access(void);
