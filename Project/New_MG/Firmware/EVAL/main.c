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
#include "Peripheral/Adc.h"
#include "Peripheral/EPWM.h"
#include "Peripheral/GPIO.h"
#include "Peripheral/ISR.h"
#include "Peripheral/Timer.h"
#include "Peripheral/DAC.h"
#include "Peripheral/SCI.h"
#include "Comm/Comm_USB.h"
#include <stdint.h>
#include <math.h>

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

extern struct_report_data report_data;
extern struct_console_data console_data;

//***********************************************************************//
//                       L o c a l  V a r i a b l e s                    //                         
//***********************************************************************//
Uint16 maxcount;
Uint16 address1, address2, address3;
Uint16 AdcaResults;

char *msg;
Uint16 *msg_unit;
Uint16 ReceivedChar;
Uint16 loopcount = 128;
//***********************************************************************//
//                          F u n c t i o n s                            //
//***********************************************************************//
void main(void)
{
    InitSysCtrl(); //Initialize System Control:PLL, WatchDog, enable Peripheral Clocks
   
    Init_Peripheral(); // Init. Peripheral

    Enable_Peripheral(); // Enable Peripheral

    EALLOW;
    EPwm1Regs.CMPA.bit.CMPA = 0.4f * PWM_PERIOD;
    EDIS;

    for(;;); //Idle Loop
}


void Init_Peripheral(void)
{
    GPIO_init();

    ISR_init();

    ADC_init(); 

    EPWM_init();

    Timer_init();

    DAC_init();

    SCIA_init();

}

void Enable_Peripheral(void)
{
    SetupADCEpwm(); //Setup the ADC 

    ISR_enable();

    EPWM_enable();
}

#pragma CODE_SECTION(SCIA_RX_isr, "ramfuncs")
__interrupt void SCIA_RX_isr(void)
{
	// while(SciaRegs.SCIFFRX.bit.RXFFST != 0)
 //    {
        Decoder_Process_USB();
    // }

	loopcount++;
	loopcount = (loopcount>256)? 0:loopcount;

    SciaRegs.SCIFFRX.bit.RXFIFORESET=1; //RESET RX FIFO

    GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1; //Toggle LED3

    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9; //clear INT Group9
}

#pragma CODE_SECTION(adca1_isr, "ramfuncs")
__interrupt void adca1_isr(void)
{
    //GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;

    static Uint16 IL_adc[3] = {0};

    IL_adc[0] = AdcaResultRegs.ADCRESULT0;
    IL_adc[1] = AdcbResultRegs.ADCRESULT0;
    IL_adc[2] = AdccResultRegs.ADCRESULT0;

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
        report_data.Volt = console_data.Vref;
        report_data.Freq = console_data.Fref;        
        report_USB();
    }

    if (*counter % (Uint32)task_period.count_100Hz == 0)
    {
    }

    if (*counter % (Uint32)task_period.count_10Hz == 0)
    {
        GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
    }

    if (*counter % (Uint32)task_period.count_1Hz == 0)
    {
    }

    if (*counter % (Uint32)TASK_MAX_COUNT == 0)
    {
        *counter = 0; //reset Timer0 counter
    }
}
