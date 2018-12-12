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
    PieVectTable.SCIA_RX_INT = &SCIA_RX_isr;
    PieVectTable.SCIB_RX_INT = &SCIB_RX_isr;
    PieVectTable.TIMER0_INT = &cpu_timer_5kHz;
    EDIS;
}

void ISR_enable(void)
{
    EALLOW;
    //
    // Enable global Interrupts and higher priority real-time debug events:
    //
    IER |= M_INT1; //Enable group 1 interrupts
    IER |= M_INT9; //Enable group 9 interrupts
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

    //
    // enable PIE interrupt
    //
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;  //Enable Cpu timer0 in PIE
    PieCtrlRegs.PIEIER9.bit.INTx1 = 1;  //Enable SCIA RX interrupt
    PieCtrlRegs.PIEIER9.bit.INTx3 = 1;  //Enable SCIB RX interrupt
    EDIS;
}
