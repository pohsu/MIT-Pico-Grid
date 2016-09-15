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
#include "Peripheral/Adc.h"
#include "Peripheral/EPWM.h"
#include "Peripheral/GPIO.h"
#include "Peripheral/ISR.h"
#include "Peripheral/Timer.h"

//
// Function Prototypes
//
void Init_Peripheral(void);
void Enable_Peripheral(void);
//
// Defines
//

//
// Globals
//
Uint16 AdcaResults;

void main(void)
{
    InitSysCtrl(); //Initialize System Control:PLL, WatchDog, enable Peripheral Clocks
   
    Init_Peripheral(); // Init. Peripheral

    Enable_Peripheral(); // Enable Peripheral

    EALLOW;
    EPwm1Regs.CMPA.bit.CMPA = 0.4f * PWM_PERIOD;
    EDIS;

//    for(;;)
//    {
//        //
//        // Turn on LED
//        //
//        GPIO_WritePin(31, 0);
//
//        //
//        // Delay for a bit.
//        //
//        DELAY_US(1000*500);
//
//        //
//        // Turn off LED
//        //
//        GPIO_WritePin(31, 1);
//
//        //
//        // Delay for a bit.
//        //
//        DELAY_US(1000*500);
//    }

}


void Init_Peripheral(void)
{
    GPIO_init(); 

    ISR_init();

    ADC_init(); 

    EPWM_init();

    Timer_init();

}

void Enable_Peripheral(void)
{
    SetupADCEpwm(0); //Setup the ADC for ePWM triggered conversions on channel 0

    ISR_enable();

    EPWM_enable();
}


#pragma CODE_SECTION(adca1_isr, "ramfuncs")
__interrupt void adca1_isr(void)
{
    GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;

    AdcaResults = AdcaResultRegs.ADCRESULT0;

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; //clear INT Group1
}

#pragma CODE_SECTION(cpu_timer_1kHz, "ramfuncs")
__interrupt void cpu_timer_1kHz(void)
{
    GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; //clear INT Group1
}
//
// End of file
//