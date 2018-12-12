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
#include <stdint.h>
#include <math.h>

#include "Peripheral/Peripheral.h"
#include "Comm/Comm.h"


//***********************************************************************//
//                          P r o t o t y p e                            //
//***********************************************************************//
void Init_Peripheral(void);
void Enable_Peripheral(void);
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
    .count_100Hz = 50,
    .count_10Hz  = 500,
    .count_1Hz   = 5000,
};

extern Uint16 data_rx[SIZEOFDATA_RX];
extern Uint16 data_tx[SIZEOFDATA_TX_MAX/2];
extern Uint16 sizeofdata_tx;

//***********************************************************************//
//                        S h a r e d  M e m o r y                       //                         
//***********************************************************************//
Uint16 c2_r_w_array[256];   // mapped to GS1 of shared RAM owned by CPU02
Uint16 c2_r_array[256];     // mapped to GS0 of shared RAM owned by CPU01
#pragma DATA_SECTION(c2_r_array,"SHARERAMGS1");
#pragma DATA_SECTION(c2_r_w_array,"SHARERAMGS0");

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

#pragma CODE_SECTION(IPC_isr, "ramfuncs")
__interrupt void IPC_isr(void)
{
    Recieve_from_CPU1(c2_r_array, data_tx, &sizeofdata_tx);
    Report_to_USB();
    EALLOW;
    GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1; //Toggle LED3
    EDIS;
    IpcRegs.IPCACK.bit.IPC0 = 1; //clear the remote CPU interrupt
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; //clear INT Group1
}

#pragma CODE_SECTION(SCIA_RX_isr, "ramfuncs")
__interrupt void SCIA_RX_isr(void)
{
    Receive_from_USB();

    Send_to_CPU1(c2_r_w_array, data_rx);

    SciaRegs.SCIFFRX.bit.RXFIFORESET=1; //RESET RX FIFO

    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9; //clear INT Group9
}

#pragma CODE_SECTION(cpu_timer_5kHz, "ramfuncs")
__interrupt void cpu_timer_5kHz(void)
{
    CpuTimer0.InterruptCount++; //Start counting

    task_table(&CpuTimer0.InterruptCount);
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; //clear INT Group1
}
//
// End of file
//
void task_table (Uint32 * counter)
{
    if (*counter % (Uint32)task_period.count_5kHz == 0)
    {

    }

    if (*counter % (Uint32)task_period.count_1kHz == 0)
    {
        //Report_to_USB();
    }

    if (*counter % (Uint32)task_period.count_100Hz == 0)
    {
    	
    }

    if (*counter % (Uint32)task_period.count_10Hz == 0)
    {

    }

    if (*counter % (Uint32)task_period.count_1Hz == 0)
    {
    	EALLOW;
    	//GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
    	EDIS;
    }

    if (*counter % (Uint32)TASK_MAX_COUNT == 0)
    {
        *counter = 0; //reset Timer0 counter
    }
}
