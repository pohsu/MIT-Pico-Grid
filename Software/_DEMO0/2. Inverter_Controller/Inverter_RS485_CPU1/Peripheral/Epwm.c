#include "F28x_Project.h"
#include "Epwm.h"
#include "../Test_setting.h"

//
// ConfigureEPWM - Configure EPWM SOC and compare values
//

void EPWM_init(void)
{
    EALLOW;
    // enable PWM1 to PWM6 Clocks
    CpuSysRegs.PCLKCR2.bit.EPWM1 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM2 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM3 = 1;


    // Disable the Epwm syn clock now
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;

    InitEPwm1(); // Init Epwm
    InitEPwm2(); // Init Epwm
    InitEPwm3(); // Init Epwm

    InitEPwm1Gpio(); // Init. Epwm GPIO pins
    InitEPwm2Gpio(); // Init. Epwm GPIO pins
    InitEPwm3Gpio(); // Init. Epwm GPIO pins


}

void InitEPwm1()
{
    EALLOW;

     //Generate EPWM enabled SOC (start of conversion)
    EPwm1Regs.ETSEL.bit.SOCAEN = 0;  //Disable SOCA for now
    EPwm1Regs.ETSEL.bit.SOCBEN = 0;  //Disable SOCB for now
    EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTRD_CMPA;  //Downcount CMPC
    EPwm1Regs.ETSEL.bit.SOCBSEL = ET_CTRU_CMPB;  //Upcount CMPD
    EPwm1Regs.ETSEL.bit.SOCASELCMP = 1;  //CMPC
    EPwm1Regs.ETSEL.bit.SOCBSELCMP = 1;  //CMPD
    EPwm1Regs.ETPS.bit.SOCAPRD = 1;             // Generate pulse on 1st event
    EPwm1Regs.ETPS.bit.SOCBPRD = 1;             // Generate pulse on 1st event

    // Setup TBCLK
    EPwm1Regs.TBPRD = PWM_PERIOD;          // Set timer period 801 TBCLKs
    EPwm1Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm1Regs.TBCTR = 0x0000;                     // Clear counter

    // Set Compare values for 0 now
    EPwm1Regs.CMPA.bit.CMPA = 0;    // Set compare A value
    EPwm1Regs.CMPB.bit.CMPB = 0;    // Set Compare B value

    #if HIL
    EPwm1Regs.CMPC = 4300;  // Set Compare C value 4300 0.1uF Tuning done on 1/21/2018
    EPwm1Regs.CMPD = 700;   // Set Compare D value 700 0.1uF
    #else
    EPwm1Regs.CMPC = 4200;  // Set Compare CD value 4200/800 Tuning done on 8/22/2018 blue board
    EPwm1Regs.CMPD = 800;  // Set Compare D value 800
    #endif
    //on TI control card
    // IL1 A1  R29 100ohm C20 0.01uF new
    // IL2 A2  R41 100ohm C32 0.01uF new
    // VC1 A4  R31 100ohm C22 0.1uF
    // VC2 A5  R43 100ohm C34 0.1uF
    // VC3 A6  R47 100ohm C38 0.1uF
    // IO  A7  R39 100ohm C30 1uF
    // IO  A8  R35 100ohm C26 1uF
    // IO  A9  R42 100ohm C33 1uF
    // Vdc A11 R44 100ohm C35 0.1uF

    // Setup counter mode
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up and down
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;       // Disable phase loading
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;      // Clock ratio to SYSCLKOUT
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    // Setup shadowing
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW; //Load immediately
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW; //Load immediately
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD; // no effect
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD; // no effect
    EPwm1Regs.CMPCTL2.bit.LOADCMODE = CC_IMMEDIATE;
    EPwm1Regs.CMPCTL2.bit.LOADDMODE = CC_IMMEDIATE;

    // Set actions
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Set PWM1A on event A, up
                                                  // count
    EPwm1Regs.AQCTLA.bit.CAD = AQ_SET;            // Clear PWM1A on event A,
                                                  // down count

    EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;          // Set PWM1B on event B, up
                                                  // count
    EPwm1Regs.AQCTLB.bit.CBD = AQ_SET;          // Clear PWM1B on event B,
                                                  // down count
    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;

	#if HIL
    //Dead-Band
    EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;			  		// Active high complementary (AHC)
    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;	      		// Full dead-band enabled
    EPwm1Regs.DBFED.bit.DBFED = 0x5;	                        // 0.25us
    EPwm1Regs.DBRED.bit.DBRED = 0x5;	                        // 0.25us
	#endif

    EDIS;
}

void InitEPwm2()
{
    EALLOW;

    // Setup TBCLK
    EPwm2Regs.TBPRD = PWM_PERIOD;          // Set timer period 801 TBCLKs
    EPwm2Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm2Regs.TBCTR = 0x0000;                     // Clear counter

    // Set Compare values for 0 now
    EPwm2Regs.CMPA.bit.CMPA = 0;    // Set compare A value
    EPwm2Regs.CMPB.bit.CMPB = 0;    // Set Compare B value

    // Setup counter mode
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up and down
    EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;       // Disable phase loading
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;      // Clock ratio to SYSCLKOUT
    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    // Setup shadowing
    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW; //Load immediately
    EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW; //Load immediately
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD; // no effect
    EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD; // no effect

    // Set actions
    EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Set PWM1A on event A, up
                                                  // count
    EPwm2Regs.AQCTLA.bit.CAD = AQ_SET;            // Clear PWM1A on event A,
                                                  // down count

    EPwm2Regs.AQCTLB.bit.CBU = AQ_CLEAR;          // Set PWM1B on event B, up
                                                  // count
    EPwm2Regs.AQCTLB.bit.CBD = AQ_SET;          // Clear PWM1B on event B,
                                                  // down count

    EPwm2Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;

	#if HIL
    //Dead-Band
    EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;			  		// Active high complementary (AHC)
    EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;	      		// Full dead-band enabled
    EPwm2Regs.DBFED.bit.DBFED = 0x5;	                        // 0.25us
    EPwm2Regs.DBRED.bit.DBRED = 0x5;	                        // 0.25us
	#endif

    EDIS;
}

void InitEPwm3()
{
    EALLOW;

    // Setup TBCLK
    EPwm3Regs.TBPRD = PWM_PERIOD;          // Set timer period 801 TBCLKs
    EPwm3Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm3Regs.TBCTR = 0x0000;                     // Clear counter

    // Set Compare values for 0 now
    EPwm3Regs.CMPA.bit.CMPA = 0;    // Set compare A value
    EPwm3Regs.CMPB.bit.CMPB = 0;    // Set Compare B value

    // Setup counter mode
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up and down
    EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;       // Disable phase loading
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;      // Clock ratio to SYSCLKOUT
    EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    // Setup shadowing
    EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW; //Load immediately
    EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW; //Load immediately
    EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD; // no effect
    EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD; // no effect

    // Set actions
    EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Set PWM1A on event A, up
                                                  // count
    EPwm3Regs.AQCTLA.bit.CAD = AQ_SET;            // Clear PWM1A on event A,
                                                  // down count

    EPwm3Regs.AQCTLB.bit.CBU = AQ_CLEAR;            // Set PWM1B on event B, up
                                                  // count
    EPwm3Regs.AQCTLB.bit.CBD = AQ_SET;          // Clear PWM1B on event B,
                                                  // down count

    EPwm3Regs.DBCTL.bit.OUT_MODE = DB_DISABLE;

	#if HIL
    //Dead-Band
    EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;			  		// Active high complementary (AHC)
    EPwm3Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;	      		// Full dead-band enabled
    EPwm3Regs.DBFED.bit.DBFED = 0x5;	                        // 0.25us
    EPwm3Regs.DBRED.bit.DBRED = 0x5;	                        // 0.25us
	#endif

    EDIS;
}

void InitEPwm4()
{
    EALLOW;

    // Setup TBCLK
    EPwm4Regs.TBPRD = PWM_PERIOD;          // Set timer period 801 TBCLKs
    EPwm4Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm4Regs.TBCTR = 0x0000;                     // Clear counter

    // Set Compare values for 0 now
    EPwm4Regs.CMPA.bit.CMPA = 0;    // Set compare A value
    EPwm4Regs.CMPB.bit.CMPB = 0;    // Set Compare B value

    // Setup counter mode
    EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up and down
    EPwm4Regs.TBCTL.bit.PHSEN = TB_DISABLE;       // Disable phase loading
    EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;      // Clock ratio to SYSCLKOUT
    EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    // Setup shadowing
    EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW; //Load immediately
    EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW; //Load immediately
    EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD; // no effect
    EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD; // no effect

    // Set actions
    EPwm4Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Set PWM1A on event A, up
                                                  // count
    EPwm4Regs.AQCTLA.bit.CAD = AQ_SET;            // Clear PWM1A on event A,
                                                  // down count

    EPwm4Regs.AQCTLB.bit.CBU = AQ_SET;            // Set PWM1B on event B, up
                                                  // count
    EPwm4Regs.AQCTLB.bit.CBD = AQ_CLEAR;          // Clear PWM1B on event B,
                                                  // down count
	#if HIL
    //Dead-Band
    EPwm4Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;			  		// Active high complementary (AHC)
    EPwm4Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;	      		// Full dead-band enabled
    EPwm4Regs.DBFED.bit.DBFED = 0x5;	                        // 0.25us
    EPwm4Regs.DBRED.bit.DBRED = 0x5;	                        // 0.25us
	#endif

    EDIS;
}

void InitEPwm5()
{
    EALLOW;

    // Setup TBCLK
    EPwm5Regs.TBPRD = PWM_PERIOD;          // Set timer period 801 TBCLKs
    EPwm5Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm5Regs.TBCTR = 0x0000;                     // Clear counter

    // Set Compare values for 0 now
    EPwm5Regs.CMPA.bit.CMPA = 0;    // Set compare A value
    EPwm5Regs.CMPB.bit.CMPB = 0;    // Set Compare B value

    // Setup counter mode
    EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up and down
    EPwm5Regs.TBCTL.bit.PHSEN = TB_DISABLE;       // Disable phase loading
    EPwm5Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;      // Clock ratio to SYSCLKOUT
    EPwm5Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    // Setup shadowing
    EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW; //Load immediately
    EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW; //Load immediately
    EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD; // no effect
    EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD; // no effect

    // Set actions
    EPwm5Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Set PWM1A on event A, up
                                                  // count
    EPwm5Regs.AQCTLA.bit.CAD = AQ_SET;            // Clear PWM1A on event A,
                                                  // down count

    EPwm5Regs.AQCTLB.bit.CBU = AQ_SET;            // Set PWM1B on event B, up
                                                  // count
    EPwm5Regs.AQCTLB.bit.CBD = AQ_CLEAR;          // Clear PWM1B on event B,
                                                  // down count
	#if HIL
    //Dead-Band
    EPwm5Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;			  		// Active high complementary (AHC)
    EPwm5Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;	      		// Full dead-band enabled
    EPwm5Regs.DBFED.bit.DBFED = 0x5;	                        // 0.25us
    EPwm5Regs.DBRED.bit.DBRED = 0x5;	                        // 0.25us
	#endif

    EDIS;
}

void InitEPwm6()
{
    EALLOW;

    // Setup TBCLK
    EPwm6Regs.TBPRD = PWM_PERIOD;          // Set timer period 801 TBCLKs
    EPwm6Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm6Regs.TBCTR = 0x0000;                     // Clear counter

    // Set Compare values for 0 now
    EPwm6Regs.CMPA.bit.CMPA = 0;    // Set compare A value
    EPwm6Regs.CMPB.bit.CMPB = 0;    // Set Compare B value

    // Setup counter mode
    EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up and down
    EPwm6Regs.TBCTL.bit.PHSEN = TB_DISABLE;       // Disable phase loading
    EPwm6Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;      // Clock ratio to SYSCLKOUT
    EPwm6Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    // Setup shadowing
    EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW; //Load immediately
    EPwm6Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW; //Load immediately
    EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD; // no effect
    EPwm6Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD; // no effect

    // Set actions
    EPwm6Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Set PWM1A on event A, up
                                                  // count
    EPwm6Regs.AQCTLA.bit.CAD = AQ_SET;            // Clear PWM1A on event A,
                                                  // down count

    EPwm6Regs.AQCTLB.bit.CBU = AQ_SET;            // Set PWM1B on event B, up
                                                  // count
    EPwm6Regs.AQCTLB.bit.CBD = AQ_CLEAR;          // Clear PWM1B on event B,
                                                  // down count
	#if HIL
    //Dead-Band
    EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;			  		// Active high complementary (AHC)
    EPwm6Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;	      		// Full dead-band enabled
    EPwm6Regs.DBFED.bit.DBFED = 0x5;	                        // 0.25us
    EPwm6Regs.DBRED.bit.DBRED = 0x5;	                        // 0.25us
	#endif

    EDIS;
}

void EPWM_enable(void)
{
    //
    // sync ePWM
    //
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1; //sync ePWM

    EPwm1Regs.ETSEL.bit.SOCAEN = 1;  //Enable SOCA
    EPwm1Regs.ETSEL.bit.SOCBEN = 1;  //Enable SOCB
    EDIS;
}
