void ISR_init(void);
void ISR_enable(void);
__interrupt void cpu_timer_5kHz(void);
__interrupt void SCIB_RX_isr(void);
__interrupt void IPC_isr(void);

typedef struct{
	Uint16 count_5kHz;
	Uint16 count_1kHz;
	Uint16 count_100Hz;
	Uint16 count_10Hz;
	Uint16 count_1Hz;
} struct_task_period;
