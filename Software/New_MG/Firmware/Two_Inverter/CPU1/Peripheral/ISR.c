#include "F28x_Project.h"
#include "ISR.h"

//***********************************************************************//
//                     G l o b a l  V a r i a b l e s                    //
//***********************************************************************//


void ISR_init(void)
{
	EALLOW;
    //
    // Step 3. Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    //
    DINT;

    //
    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    // This function is found in the F2837xD_PieCtrl.c file.
    //
    InitPieCtrl();

    //
    // Disable CPU interrupts and clear all CPU interrupt flags:
    //
    IER = 0x0000;
    IFR = 0x0000;

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    // The shell ISR routines are found in F2837xD_DefaultIsr.c.
    // This function is found in F2837xD_PieVect.c.
    //
    InitPieVectTable();    

    //
	// Map ISR functions
	//
    EALLOW;
    PieVectTable.ADCA1_INT = &adca1_isr;
    PieVectTable.TIMER0_INT = &cpu_timer_5kHz;
    PieVectTable.IPC0_INT = &IPC_isr;
    EDIS;
}

void ISR_enable(void)
{
    EALLOW;
    //
    // Enable global Interrupts and higher priority real-time debug events:
    //
    IER |= M_INT1; //Enable group 1 interrupts
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

    //
    // enable PIE interrupt
    //
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;  //Enable ADCA in PIE
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;  //Enable Cpu timer0 in PIE
    PieCtrlRegs.PIEIER1.bit.INTx13 = 1;  //Enable IPC0 in PIE
    EDIS;
}
