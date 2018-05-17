#include "F28x_Project.h"
#include "GPIO.h"

void GPIO_init (void)
{
	//
    // Step 2. Initialize GPIO:
    // This example function is found in the F2837xD_Gpio.c file and
    // illustrates how to set the GPIO to it's default state.
    //

	InitGpio(); // Skipped for this example

	EALLOW;
    
    #ifdef CPU1
    //LED2
	GpioCtrlRegs.GPACSEL4.bit.GPIO31 = 0b00; //CPU1
	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0; //pull-up
    //LED3
    GpioCtrlRegs.GPBCSEL1.bit.GPIO34 = 0b10; //CPU2
	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;
    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 0; //pull-up

    //SCI
    GpioCtrlRegs.GPACSEL4.bit.GPIO28 = 0b10; //CPU2
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1; //Refer to datasheet
    GpioCtrlRegs.GPADIR.bit.GPIO28 = 0; //SCI_A_RX as input
    GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0; //pull-up
    GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 0; //syn

    GpioCtrlRegs.GPACSEL4.bit.GPIO29 = 0b10; //CPU2
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1; //Refer to datasheet
    GpioCtrlRegs.GPADIR.bit.GPIO29 = 1; //SCI_A_TX as output
    GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0; //pull-up
    GpioCtrlRegs.GPAQSEL2.bit.GPIO29 = 3; //ascyn mode
    #endif
    //SCI
   	EDIS;
}



