#include "F28x_Project.h"
#include "F2837xD_GlobalPrototypes.h"
#include "Epwm.h"


//
// ConfigureEPWM - Configure EPWM SOC and compare values
//

#define PWM_PERIOD 4096

void EPWM_init(void)
{
    // enable PWM1, PWM2 and PWM3 Clocks
    CpuSysRegs.PCLKCR2.bit.EPWM1 = 1;

    // Disable the Epwm syn clock now
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;

    InitEPwm1Gpio(); // Init. Epwm GPIO pins

    EALLOW;   
    InitEPwm1(); // Init Epwm1
    EDIS;

    //
    // sync ePWM
    //
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
}

void InitEPwm1()
{

    // Setup TBCLK
    EPwm1Regs.TBPRD = PWM_PERIOD;          // Set timer period 801 TBCLKs
    EPwm1Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm1Regs.TBCTR = 0x0000;                     // Clear counter

    // Set Compare values for 0 now
    EPwm1Regs.CMPA.bit.CMPA = 0;    // Set compare A value
    EPwm1Regs.CMPB.bit.CMPB = 0;    // Set Compare B value

    // Setup counter mode
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up and down
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;       // Disable phase loading
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;      // Clock ratio to SYSCLKOUT
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    // Setup shadowing
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_IMMEDIATE; //Load immediately
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_IMMEDIATE; //Load immediately
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD; // no effect
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD; // no effect

    // Set actions
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;            // Set PWM1A on event A, up
                                                  // count
    EPwm1Regs.AQCTLA.bit.CAD = AQ_SET;          // Clear PWM1A on event A,
                                                  // down count

    EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;          // Set PWM1B on event B, up
                                                  // count
    EPwm1Regs.AQCTLB.bit.CBD = AQ_CLEAR;          // Clear PWM1B on event B,
                                                  // down count

    // Interrupt where we will change the Compare Values
    EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
    EPwm1Regs.ETSEL.bit.INTEN = 0;                // Disable INT
    EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;           // Generate INT on 3rd event

}


