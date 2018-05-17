#include "F28x_Project.h"
#include "Peripheral.h"

void Init_Peripheral(void)
{
    ISR_init();

    Timer_init();

    SCIA_init();
}

void Enable_Peripheral(void)
{
    ISR_enable();
}
