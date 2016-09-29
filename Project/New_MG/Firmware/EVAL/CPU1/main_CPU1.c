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
#include "F2837xD_Ipc_drivers.h"
#include <stdint.h>
#include <math.h>

#include "Peripheral\Peripheral.h"
#include "Comm\Comm.h"

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
    .count_100Hz = 50,
    .count_10Hz  = 500,
    .count_1Hz   = 5000,
};

extern struct_report_data report_data;
extern struct_console_data console_data;

//***********************************************************************//
//                        S h a r e d  M e m o r y                       //                         
//***********************************************************************//
Uint16 c1_r_array[256];   // mapped to GS0 of shared RAM owned by CPU02
Uint16 c1_r_w_array[256]; // mapped to GS1 of shared RAM owned by CPU01
#pragma DATA_SECTION(c1_r_array,"SHARERAMGS0");
#pragma DATA_SECTION(c1_r_w_array,"SHARERAMGS1");

//***********************************************************************//
//                          F u n c t i o n s                            //
//***********************************************************************//
void main(void)
{
	InitSysCtrl(); //Initialize System Control:PLL, WatchDog, enable Peripheral Clocks
   
	Owner_Peripheral(); // Transfering ownership to CPU2 before the Peripheral clock enabled

	Init_Peripheral();

    Enable_Peripheral(); 

    Enable_Comm();

    IPCBootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH); // Send boot command to allow the CPU2 application to begin execution

    EALLOW;
    EPwm1Regs.CMPA.bit.CMPA = 0.4f * PWM_PERIOD;
    EDIS;

    for(;;); //Idle Loop
}

#pragma CODE_SECTION(IPC_isr, "ramfuncs")
__interrupt void IPC_isr(void)
{
    Decoder_Process_IPC(c1_r_array);
    GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
    IpcRegs.IPCACK.bit.IPC0 = 1; //clear the remote CPU interrupt
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; //clear INT Group1
}

#pragma CODE_SECTION(adca1_isr, "ramfuncs")
__interrupt void adca1_isr(void)
{
//    static Uint16 IL_adc[3] = {0};
//
//    IL_adc[0] = AdcaResultRegs.ADCRESULT0;
//    IL_adc[1] = AdcbResultRegs.ADCRESULT0;
//    IL_adc[2] = AdccResultRegs.ADCRESULT0;

    uDACA(99.0f, 100.0f);
    DACB(25.0f, 50.0f);
    DACC(-50.0f, 100.0f);

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; //clear INT Group1
}

#pragma CODE_SECTION(cpu_timer_5kHz, "ramfuncs")
__interrupt void cpu_timer_5kHz(void)
{
    CpuTimer0.InterruptCount++; //Start counting

    task_table(&CpuTimer0.InterruptCount);
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; //clear INT Group1
}

void task_table (Uint32 * counter)
{
    if (*counter % (Uint32)task_period.count_5kHz == 0)
    {

    }

    if (*counter % (Uint32)task_period.count_1kHz == 0)
    {
        report_data.Volt = console_data.Vref;
        report_data.Freq = console_data.Fref;        
        Send_to_CPU2(c1_r_w_array);
    }

    if (*counter % (Uint32)task_period.count_100Hz == 0)
    {
    }

    if (*counter % (Uint32)task_period.count_10Hz == 0)
    {
        //GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
    }

    if (*counter % (Uint32)task_period.count_1Hz == 0)
    {
    	//GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
    }

    if (*counter % (Uint32)TASK_MAX_COUNT == 0)
    {
        *counter = 0; //reset Timer0 counter
    }
}
