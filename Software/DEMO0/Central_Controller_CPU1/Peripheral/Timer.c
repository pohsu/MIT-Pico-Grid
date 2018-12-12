#include "F28x_Project.h"
#include "Timer.h"

void Timer_init(void)
{
	// Step 4. Initialize the Device Peripheral. This function can be
	//         found in F2837xD_CpuTimers.c
	InitCpuTimers();   // For this example, only initialize the Cpu Timers
	// Configure CPU-Timer 0 to __interrupt every 500 milliseconds:
	// 200MHz CPU Freq, 1 millisecond Period (in uSeconds)
	// Specified we're using timer0 when we set up/initialized interrupts: ISR_init().
	ConfigCpuTimer(&CpuTimer0, 200, 200); // 5KHZ: so last argument is 1/5,000 = 200 uSec

	CpuTimer0Regs.TCR.all = 0x4001; // The "4" bit species timer interrupt enable. Use write-only instruction to set TSS bit = 0
}                                   // Why have the "1" bit? for LSB?  can it just be 0??

