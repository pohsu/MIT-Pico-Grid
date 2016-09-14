#include "F28x_Project.h"
#include "ISR.h"

void ISR_init(void)
{
	//
	// Map ISR functions
	//
    EALLOW;
    PieVectTable.ADCA1_INT = &adca1_isr; //function for ADCA interrupt 1
    EDIS;

    //
	// Enable global Interrupts and higher priority real-time debug events:
	//
    IER |= M_INT1; //Enable group 1 interrupts
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

    //
	// enable PIE interrupt
	//
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;
}
