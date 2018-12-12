#include "F28x_Project.h"
#include "F2837xD_Ipc_drivers.h"
#include <stdint.h>
#include <math.h>

#include "Peripheral\Peripheral.h"
#include "Comm\Comm.h"
#include "Control\Meas.h"
#include "Control\control.h"
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
    .count_100Hz = 50,
    .count_10Hz  = 500,
    .count_1Hz   = 5000,
};

extern struct_report_data report_data;
extern struct_console_data console_data;
extern bool enable;

extern struct_meas_states meas_states1;
extern struct_control_states control_states1;


//static Uint16 DutyA, DutyB, DutyC = 0;

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

	Owner_Peripheral(); // Transferring ownership to CPU2 before the Peripheral clock enabled

	Init_Peripheral();

	Enable_Peripheral();

	Enable_Comm();

	IPCBootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH); // Send boot command to allow the CPU2 application to begin execution

	for(;;); //Idle Loop
}

#pragma CODE_SECTION(IPC_isr, "ramfuncs")
__interrupt void IPC_isr(void)
{
    Decoder_Process_IPC(c1_r_array);


    IpcRegs.IPCACK.bit.IPC0 = 1; //clear the remote CPU interrupt
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; //clear INT Group1
}

#pragma CODE_SECTION(adca1_isr, "ramfuncs")
__interrupt void adca1_isr(void)
{
    GpioDataRegs.GPASET.bit.GPIO31 = 1; //LED2 on Control Card
//    uDACB(2.99f, 1.0f);
	static float32 Droop[2], Xm;

	Droop[0] = (float32)console_data.vref/1000.0f;
	Droop[1] = (float32)console_data.vref/1000.0f;
	Xm = (float32)console_data.fref;
	Measurement_step(enable);
	Control_step(Droop, Xm, enable);

//	EPwm1Regs.CMPA.bit.CMPA = control_states1.Duty[0];
//	EPwm1Regs.CMPB.bit.CMPB = control_states1.Duty[0];
//	EPwm2Regs.CMPA.bit.CMPA = control_states1.Duty[1];
//	EPwm2Regs.CMPB.bit.CMPB = control_states1.Duty[1];
//	EPwm3Regs.CMPA.bit.CMPA = control_states1.Duty[2];
//	EPwm3Regs.CMPB.bit.CMPB = control_states1.Duty[2];

    EPwm1Regs.CMPA.bit.CMPA = PWM_PERIOD * (float32)console_data.vref/100.0f;
    EPwm1Regs.CMPB.bit.CMPB = PWM_PERIOD * (float32)console_data.vref/100.0f;
    EPwm2Regs.CMPA.bit.CMPA = PWM_PERIOD * (float32)console_data.vref/100.0f;
    EPwm2Regs.CMPB.bit.CMPB = PWM_PERIOD * (float32)console_data.vref/100.0f;
    EPwm3Regs.CMPA.bit.CMPA = PWM_PERIOD * (float32)console_data.vref/100.0f;
    EPwm3Regs.CMPB.bit.CMPB = PWM_PERIOD * (float32)console_data.vref/100.0f;

//	EPwm1Regs.CMPA.bit.CMPA = 2500;

	if (enable)
	{
		GpioDataRegs.GPCSET.bit.GPIO85 = 1; //EMSTOP
		GpioDataRegs.GPCSET.bit.GPIO87 = 1; //NTCBYPASS
		EALLOW;
		EPwm1Regs.TZCLR.bit.OST = 1;
		EPwm2Regs.TZCLR.bit.OST = 1;
		EPwm3Regs.TZCLR.bit.OST = 1;
		EPwm4Regs.TZCLR.bit.OST = 1;
		EPwm5Regs.TZCLR.bit.OST = 1;
		EPwm6Regs.TZCLR.bit.OST = 1;
		EDIS;
	}
	else
	{
		GpioDataRegs.GPCCLEAR.bit.GPIO85 = 1; //EMSTOP
		GpioDataRegs.GPCCLEAR.bit.GPIO87 = 1; //NTCBYPASS
		EALLOW;
		EPwm1Regs.TZFRC.bit.OST = 1;
		EPwm2Regs.TZFRC.bit.OST = 1;
		EPwm3Regs.TZFRC.bit.OST = 1;
		EPwm4Regs.TZFRC.bit.OST = 1;
		EPwm5Regs.TZFRC.bit.OST = 1;
		EPwm6Regs.TZFRC.bit.OST = 1;
		EDIS;
	}

	AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; //clear INT Group1
//    uDACB(0.0f, 1.0f);
    GpioDataRegs.GPACLEAR.bit.GPIO31 = 1; //LED2 on Control Card
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
        report_data.volt = console_data.vref;
        report_data.freq = console_data.fref;
        Send_to_CPU2(c1_r_w_array);
    }

//    if (*counter % (Uint32)task_period.count_100Hz == 0)
//    {
//    }
//
//    if (*counter % (Uint32)task_period.count_10Hz == 0)
//    {
//        GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
//    }
//
    if (*counter % (Uint32)task_period.count_1Hz == 0)
    {
//    	GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
    	GpioDataRegs.GPCTOGGLE.bit.GPIO73 = 1;
    }

    if (*counter % (Uint32)TASK_MAX_COUNT == 0)
    {
        *counter = 0; //reset Timer0 counter
    }
}
