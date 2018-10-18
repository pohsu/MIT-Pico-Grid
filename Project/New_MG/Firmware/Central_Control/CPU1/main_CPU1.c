#include "F28x_Project.h"
#include "F2837xD_Ipc_drivers.h"
#include <stdint.h>
#include <math.h>

#include "Peripheral\Peripheral.h"
#include "Comm\Comm.h"
#include "Test_setting.h"

//***********************************************************************//
//                          P r o t o t y p e                            //
//***********************************************************************//
void task_table (Uint32 * counter);

//***********************************************************************//
//                             D e f i n e                               //
//***********************************************************************//

#define NUM_TASK_MEMBER (Uint16)(sizeof(struct_task_period) / sizeof(Uint16))
#define TASK_MAX_COUNT *((Uint16 *)&task_period + (NUM_TASK_MEMBER-1))

//***********************************************************************//
//                     G l o b a l  V a r i a b l e s                    //
//***********************************************************************//
const struct_task_period task_period = {
    .count_5kHz  = 1,
    .count_1kHz  = 5,
    .count_200Hz = 25,
    .count_10Hz  = 500,
    .count_1Hz   = 5000,
};

Uint16 device = 0, device_rx = 0;

extern Uint16 usb_tx[SIZEOFUSB_TX];
extern Uint16 usb_rx[SIZEOFUSB_RX];
extern Uint16 RX_ctr;


//***********************************************************************//
//                        S h a r e d  M e m o r y                       //
//***********************************************************************//

//***********************************************************************//
//                          F u n c t i o n s                            //
//***********************************************************************//
void main(void)
{
	InitSysCtrl(); //Initialize System Control:PLL, WatchDog, enable Peripheral Clocks

	Init_Peripheral();

	Enable_Peripheral();

	Enable_Comm();

	for(;;); //Idle Loop
}

#pragma CODE_SECTION(SCIA_RX_isr, "ramfuncs")
__interrupt void SCIA_RX_isr(void)
{
    USB_RX();
    USB_TO_RS485Interpreter();
    GpioDataRegs.GPCTOGGLE.bit.GPIO75 = 1; //LED2
    SciaRegs.SCIFFRX.bit.RXFIFORESET=1; //RESET RX FIFO
    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9; //clear INT Group9
}

#pragma CODE_SECTION(SCIB_RX_isr, "ramfuncs")
__interrupt void SCIB_RX_isr(void)
{
//    if (device_rx == 0) GpioDataRegs.GPCTOGGLE.bit.GPIO81 = 1; //LED5
//    if (device_rx == 2) GpioDataRegs.GPCTOGGLE.bit.GPIO83 = 1; //LED6
    RS485_RX(device_rx);
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9; //clear INT Group9
}


//#pragma CODE_SECTION(cpu_timer_5kHz, "ramfuncs")
__interrupt void cpu_timer_5kHz(void)
{
	CpuTimer0.InterruptCount++; //Start counting

    task_table(&CpuTimer0.InterruptCount);
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; //clear INT Group1
}

void task_table (Uint32 * counter)
{
//    if (*counter % (Uint32)task_period.count_5kHz == 0)
//    {
//
//    }

    if (*counter % (Uint32)task_period.count_1kHz == 0)
    {
        USB_TX();
        // even if receive fails the next call is still processing by forcing a entire reset
        ScibRegs.SCICTL1.bit.SWRESET = 0;
        ScibRegs.SCICTL1.bit.SWRESET = 1;
        ScibRegs.SCICTL1.bit.SLEEP = 1; //back to sleep
        RX_ctr = 0; //ctr reset
        device_rx = device; //device_rx is global
        Uint16 i;
        for (i = 0; i < SIZEOFRS485_TX; i++) usb_tx[4*device_rx+i] = 0; //reset usb_tx for the device
        RS485_TX(device);
        device++;
        if (device == NUMOFDEVICE) device = 0;
    }

    if (*counter % (Uint32)task_period.count_200Hz == 0)
    {


    }
//
    if (*counter % (Uint32)task_period.count_10Hz == 0)
    {
        GpioDataRegs.GPCTOGGLE.bit.GPIO73 = 1;
    }
//
    if (*counter % (Uint32)task_period.count_1Hz == 0)
    {

//        GpioDataRegs.GPATOGGLE.bit.GPIO0 = 1;
//        GpioDataRegs.GPATOGGLE.bit.GPIO1 = 1;
//        GpioDataRegs.GPATOGGLE.bit.GPIO2 = 1;
//        GpioDataRegs.GPATOGGLE.bit.GPIO3 = 1;
    }

    if (*counter % (Uint32)TASK_MAX_COUNT == 0)
    {
        *counter = 0; //reset Timer0 counter
    }
}
