//###########################################################################
// $TI Release: F2837xD Support Library v200 $
// $Release Date: Tue Jun 21 13:00:02 CDT 2016 $
// $Copyright: Copyright (C) 2013-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

//
// Included Files
//
#include "F28x_Project.h"
#include "Peripheral\ISR.h"
#include "Peripheral\Adc.h"
#include "Peripheral\EPWM.h"

//
// Function Prototypes
//
void Init_Sys_all(void);
void Init_Peripheral(void);
void Init_Peripheral(void);
interrupt void adca1_isr(void);
//
// Defines
//
#define RESULTS_BUFFER_SIZE 256

//
// Globals
//
Uint16 AdcaResults[RESULTS_BUFFER_SIZE];
Uint16 resultsIndex;
volatile Uint16 bufferFull;

void main(void)
{
    Init_Sys_all(); // Init. all the Sys conf.
   
    Init_Peripheral(); // Init. Peripheral

    EALLOW;
    EPwm1Regs.CMPA.bit.CMPA = 2048;
    EDIS;
    //
    // Initialize results buffer
    //
    // for(resultsIndex = 0; resultsIndex < RESULTS_BUFFER_SIZE; resultsIndex++)
    // {
    //     AdcaResults[resultsIndex] = 0;
    // }
    // resultsIndex = 0;
    // bufferFull = 0;

    // //
    // //start ePWM
    // //
    // EPwm1Regs.ETSEL.bit.SOCAEN = 1;  //enable SOCA
    // EPwm1Regs.TBCTL.bit.CTRMODE = 0; //unfreeze, and enter up count mode



    // //
    // //take conversions indefinitely in loop
    // //
    // do
    // {


    //     //
    //     //wait while ePWM causes ADC conversions, which then cause interrupts,
    //     //which fill the results buffer, eventually setting the bufferFull
    //     //flag
    //     //
    //     while(!bufferFull);
    //     bufferFull = 0; //clear the buffer full flag

    //     //
    //     //stop ePWM
    //     //
    //     EPwm1Regs.ETSEL.bit.SOCAEN = 0;  //disable SOCA
    //     EPwm1Regs.TBCTL.bit.CTRMODE = 3; //freeze counter

    //     //
    //     //at this point, AdcaResults[] contains a sequence of conversions
    //     //from the selected channel
    //     //

    //     //
    //     //software breakpoint, hit run again to get updated conversions
    //     //
    //     asm("   ESTOP0");
    // }while(1);
    for(;;)
    {
        //
        // Turn on LED
        //
        GPIO_WritePin(31, 0);

        //
        // Delay for a bit.
        //
        DELAY_US(1000*500);

        //
        // Turn off LED
        //
        GPIO_WritePin(31, 1);

        //
        // Delay for a bit.
        //
        DELAY_US(1000*500);
    }

}


void Init_Sys_all(void)
{
    //
    // Step 1. Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    // This example function is found in the F2837xD_SysCtrl.c file.
    //
    InitSysCtrl();

    //
    // Step 2. Initialize GPIO:
    // This example function is found in the F2837xD_Gpio.c file and
    // illustrates how to set the GPIO to it's default state.
    //
    InitGpio(); // Skipped for this example
    GPIO_SetupPinMux(31, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(31, GPIO_OUTPUT, GPIO_PUSHPULL);
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
}

void Init_Peripheral(void)
{
    ISR_init(); 

    EPWM_init();

    ADC_init(); 
}

//
// adca1_isr - Read ADC Buffer in ISR
//
interrupt void adca1_isr(void)
{
    //static bool toggle = 0;
    // toggle = ~toggle;
    // GPIO_WritePin(31, toggle);

    AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT0;
    if(RESULTS_BUFFER_SIZE <= resultsIndex)
    {
        resultsIndex = 0;
        bufferFull = 1;
    }

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

//
// End of file
//
