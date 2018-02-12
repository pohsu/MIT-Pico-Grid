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
    
    //LED2 on Control Card
	GpioCtrlRegs.GPACSEL4.bit.GPIO31 = 0b00; //CPU1
	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0; //pull-up
    //LED3 on Control Card
    GpioCtrlRegs.GPBCSEL1.bit.GPIO34 = 0b10; //CPU2
	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;
    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 0; //pull-up

    //EMSTOP
	GpioCtrlRegs.GPCCSEL3.bit.GPIO85 = 0b00; //CPU1
	GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 0;
	GpioCtrlRegs.GPCDIR.bit.GPIO85 = 1; //output
	GpioCtrlRegs.GPCPUD.bit.GPIO85 = 1; // disable pull-up

    //NTC Bypass
	GpioCtrlRegs.GPCCSEL3.bit.GPIO87 = 0b00; //CPU1
	GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 0;
	GpioCtrlRegs.GPCDIR.bit.GPIO87 = 1; //output
	GpioCtrlRegs.GPCPUD.bit.GPIO87 = 1; // disable pull-up

    //SW Brake
	GpioCtrlRegs.GPCCSEL4.bit.GPIO89 = 0b00; //CPU1
	GpioCtrlRegs.GPCMUX2.bit.GPIO89 = 0;
	GpioCtrlRegs.GPCDIR.bit.GPIO89 = 1; //output
	GpioCtrlRegs.GPCPUD.bit.GPIO89 = 1; //disable pull-up

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

    //LED1 on PCB
    GpioCtrlRegs.GPCCSEL2.bit.GPIO73 = 0b00; //CPU1
	GpioCtrlRegs.GPCMUX1.bit.GPIO73 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO73 = 1;
    GpioCtrlRegs.GPCPUD.bit.GPIO73 = 0; //pull-up

    //LED2 on PCB
    GpioCtrlRegs.GPCCSEL2.bit.GPIO75 = 0b00; //CPU1
	GpioCtrlRegs.GPCMUX1.bit.GPIO75 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO75 = 1;
    GpioCtrlRegs.GPCPUD.bit.GPIO75 = 0; //pull-up

    //LED3 on PCB
    GpioCtrlRegs.GPCCSEL2.bit.GPIO77 = 0b00; //CPU1
	GpioCtrlRegs.GPCMUX1.bit.GPIO77 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO77 = 1;
    GpioCtrlRegs.GPCPUD.bit.GPIO77 = 0; //pull-up

    //LED4 on PCB
    GpioCtrlRegs.GPCCSEL2.bit.GPIO79 = 0b00; //CPU1
	GpioCtrlRegs.GPCMUX1.bit.GPIO79 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO79 = 1;
    GpioCtrlRegs.GPCPUD.bit.GPIO79 = 0; //pull-up

    //LED5 on PCB
    GpioCtrlRegs.GPCCSEL3.bit.GPIO81 = 0b00; //CPU1
	GpioCtrlRegs.GPCMUX2.bit.GPIO81 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO81 = 1;
    GpioCtrlRegs.GPCPUD.bit.GPIO81 = 0; //pull-up

    //LED6 on PCB
    GpioCtrlRegs.GPCCSEL3.bit.GPIO83 = 0b00; //CPU1
	GpioCtrlRegs.GPCMUX2.bit.GPIO83 = 0;
    GpioCtrlRegs.GPCDIR.bit.GPIO83 = 1;
    GpioCtrlRegs.GPCPUD.bit.GPIO83 = 0; //pull-up

    EDIS;
}



