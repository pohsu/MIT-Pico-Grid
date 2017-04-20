#define SIZEOFDATA_TX 2 * (Uint16)(sizeof(struct_report_data) / sizeof(float32))
#define START 111
#define STOP 222
#define FREF 100
#define VREF 101

typedef struct{
	float32 volt;
	float32 freq;
} struct_report_data;

typedef struct{
	Uint16 fref;
	Uint16 vref;
} struct_console_data;

void Send_to_CPU2(Uint16 * array);
void Decoder_Process_IPC (Uint16 * array);
void Reset_remote_IPC(void);
void Grant_meomory_access(void);
