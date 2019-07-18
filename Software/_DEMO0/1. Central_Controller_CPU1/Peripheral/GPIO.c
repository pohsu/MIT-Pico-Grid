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

	//GPIO0:
	// GPAMUX1: GPIO 0-15, GPAMUX2: 16-31, GPBMUX1: 32-47, GPBMUX2: 48-63
	GpioCtrlRegs.GPACSEL1.bit.GPIO0 = 0b00; // CPU1 controls GPIO0 pins.
	GpioCtrlRegs.GPAMUX1.bit.GPIO0  = 0;    // pg. 842, Table 7-8 use pin as GPIO GPAMUX1: GPIO0 within GPIO 0-15 group
	GpioCtrlRegs.GPADIR.bit.GPIO0   = 1;    // configured as Output
	GpioCtrlRegs.GPAPUD.bit.GPIO0   = 0;    // pull-up enabled
	GpioDataRegs.GPASET.bit.GPIO0   = 1;    // force gpio data to latch to 1.

    //GPIO1
    GpioCtrlRegs.GPACSEL1.bit.GPIO1 = 0b00; //CPU1
    GpioCtrlRegs.GPAMUX1.bit.GPIO1  = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO1   = 1;    // configured as Output
    GpioCtrlRegs.GPAPUD.bit.GPIO1   = 0;    //pull-up
    GpioDataRegs.GPASET.bit.GPIO1   = 1;

    //GPIO2
    GpioCtrlRegs.GPACSEL1.bit.GPIO2 = 0b00; //CPU1
    GpioCtrlRegs.GPAMUX1.bit.GPIO2  = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO2   = 1;    // configured as Output
    GpioCtrlRegs.GPAPUD.bit.GPIO2   = 0;    //pull-up
    GpioDataRegs.GPASET.bit.GPIO2   = 1;

    //GPIO3
    GpioCtrlRegs.GPACSEL1.bit.GPIO3 = 0b00; //CPU1
    GpioCtrlRegs.GPAMUX1.bit.GPIO3  = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO3   = 1;    // configured as Output
    GpioCtrlRegs.GPAPUD.bit.GPIO3   = 0;    //pull-up
    GpioDataRegs.GPASET.bit.GPIO3   = 1;

    //LED2 on Control Card
	GpioCtrlRegs.GPACSEL4.bit.GPIO31 = 0b00; //CPU1
	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0; //pull-up

    //LED3 on Control Card
    GpioCtrlRegs.GPBCSEL1.bit.GPIO34 = 0b00; //CPU1
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

    //SCIA
    GpioCtrlRegs.GPACSEL4.bit.GPIO28 = 0b00; //CPU1
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1; //Technical Ref. Manual: Specify that pin's function is SCI_A_RX
    GpioCtrlRegs.GPADIR.bit.GPIO28 = 0; //SCI_A_RX as input
    GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0; //pull-up
    GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 0; //syn

    GpioCtrlRegs.GPACSEL4.bit.GPIO29 = 0b00; //CPU1
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1; //Refer to datasheet
    GpioCtrlRegs.GPADIR.bit.GPIO29 = 1; //SCI_A_TX as output
    GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0; //pull-up
    GpioCtrlRegs.GPAQSEL2.bit.GPIO29 = 3; //Asyn mode

    //SCIB
    GpioCtrlRegs.GPACSEL2.bit.GPIO15 = 0b00; //CPU1
    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 2; //Refer to datasheet
    GpioCtrlRegs.GPADIR.bit.GPIO15 = 0; //SCI_B_RX as input
    GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0; //pull-up
    GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 0; //syn

    GpioCtrlRegs.GPACSEL2.bit.GPIO14 = 0b00; //CPU1
    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 2; //Refer to datasheet
    GpioCtrlRegs.GPADIR.bit.GPIO14 = 1; //SCI_B_TX as output
    GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0; //pull-up
    GpioCtrlRegs.GPAQSEL1.bit.GPIO14 = 3; //Asyn mode

    //RS485 DEN
    GpioCtrlRegs.GPACSEL2.bit.GPIO13 = 0b00; //CPU1
    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO13 = 1; //output
    GpioCtrlRegs.GPAPUD.bit.GPIO13 = 1; //disable pull-up, low/high voltages are different.

    //RS485 ~RE
    GpioCtrlRegs.GPACSEL2.bit.GPIO12 = 0b00; //CPU1
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO12 = 1; //output
    GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0; //pull-up

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



