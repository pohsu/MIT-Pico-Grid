#include "F28x_Project.h"
#include "Peripheral.h"

//void Owner_Peripheral(void)
//{
//    EALLOW;
//    DevCfgRegs.CPUSEL5.bit.SCI_A = 1; ////Transfer ownership of SCI_A to cpu2
//    EDIS;
//}

void Init_Peripheral(void)
{
    GPIO_init();

    ISR_init();

    Timer_init();

    SCIA_init();
}

void Enable_Peripheral(void)
{
    ISR_enable();
}
