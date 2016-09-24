
#define SIZEOFDATA_TX 2 * (Uint16)(sizeof(struct_report_data) / sizeof(float32))
#define SIZEOFDATA_RX 4
#define BEGIN 111
#define END 222
#define START 111
#define STOP 222
#define FREF 100
#define VREF 101

typedef struct{
	float32 Volt;
	float32 Freq;
} struct_report_data;

typedef struct{
	Uint16 Fref;
	Uint16 Vref;
} struct_console_data;

void report_USB(void);

void Decoder_Process_USB (void);


