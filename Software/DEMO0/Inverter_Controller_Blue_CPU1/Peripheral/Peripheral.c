#include "F28x_Project.h"
#include "Peripheral.h"

void Owner_Peripheral(void)
{
    EALLOW;
    DevCfgRegs.CPUSEL5.bit.SCI_A = 1; ////Transfer ownership of SCI_A to cpu2
    DevCfgRegs.CPUSEL5.bit.SCI_B = 1; ////Transfer ownership of SCI_B to cpu2
    EDIS;
}

void Init_Peripheral(void)
{
    GPIO_init();

    ISR_init();

    ADC_init();

    EPWM_init();

    Timer_init();

    DAC_init();

}

void Enable_Peripheral(void)
{
    SetupADCEpwm(); //Setup the ADC

    ISR_enable();

    EPWM_enable();
}
