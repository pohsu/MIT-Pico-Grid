#include "F28x_Project.h"
#include "Adc.h"
#include "../Test_setting.h"
#define ACQPS_SIZE 15 //15=75ns for single-ended mode

void ADC_init(void)
{
    EALLOW;

    //
    //write configurations
    //
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcbRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdccRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcdRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCB, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCC, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCD, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

    //
    //Set pulse positions to late
    //
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdcbRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdccRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdcdRegs.ADCCTL1.bit.INTPULSEPOS = 1;

    //
    //power up the ADC
    //
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    AdcdRegs.ADCCTL1.bit.ADCPWDNZ = 1;

    //
    //delay for 1ms to allow ADC time to power up
    //
    DELAY_US(1000);

    EDIS;
}

//
// SetupADCEpwm - Setup ADC EPWM acquisition window
//
void SetupADCEpwm(void)
{

	//
	//Select the channels to convert and end of conversion flag
	//
	EALLOW;
	// Simultaneous sampling configuration from the C2837x Manual, Page 1319
//	AdcaRegs.ADCSOC0CTL.bit.CHSEL = 2;  //SOC0 will convert ADCINA2
//	AdcaRegs.ADCSOC0CTL.bit.ACQPS = ACQPS_SIZE; //sample window:adding buffer since ADAC is the source for interrupt
//	AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA
//
//	AdcbRegs.ADCSOC0CTL.bit.CHSEL = 2;  //SOC0 will convert ADCINB2
//	AdcbRegs.ADCSOC0CTL.bit.ACQPS = ACQPS_SIZE; //sample window
//	AdcbRegs.ADCSOC0CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA
//
//	AdcaRegs.ADCSOC1CTL.bit.CHSEL = 2;  //SOC2 will convert ADCINA2
//	AdcaRegs.ADCSOC1CTL.bit.ACQPS = ACQPS_SIZE; //sample window:adding buffer since ADAC is the source for interrupt
//	AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB
//
//	AdcbRegs.ADCSOC1CTL.bit.CHSEL = 2;  //SOC2 will convert ADCINB2
//	AdcbRegs.ADCSOC1CTL.bit.ACQPS = ACQPS_SIZE; //sample window
//	AdcbRegs.ADCSOC1CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB
//
//	AdcaRegs.ADCSOC2CTL.bit.CHSEL = 4;  //SOC4 will convert ADCINA4
//	AdcaRegs.ADCSOC2CTL.bit.ACQPS = ACQPS_SIZE; //sample window
//	AdcaRegs.ADCSOC2CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB
//
//	AdcbRegs.ADCSOC2CTL.bit.CHSEL = 4;  //SOC4 will convert ADCINB4
//	AdcbRegs.ADCSOC2CTL.bit.ACQPS = ACQPS_SIZE; //sample window
//	AdcbRegs.ADCSOC2CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB
//
//	AdcaRegs.ADCSOC3CTL.bit.CHSEL = 14;  //SOC6 will convert ADCIN14
//	AdcaRegs.ADCSOC3CTL.bit.ACQPS = ACQPS_SIZE; //sample window
//	AdcaRegs.ADCSOC3CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB
//
//	AdcdRegs.ADCSOC3CTL.bit.CHSEL = 0;  //SOC6 will convert ADCIND0
//	AdcdRegs.ADCSOC3CTL.bit.ACQPS = ACQPS_SIZE; //sample window
//	AdcdRegs.ADCSOC3CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB
//
//	AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0x3; //end of SOC3 will set INT1 flag
//	AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
//	AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared

	#if HIL
	//Inductor Current
	AdcaRegs.ADCSOC0CTL.bit.CHSEL = 2;  //SOC0 will convert ADCINA2
	AdcaRegs.ADCSOC0CTL.bit.ACQPS = ACQPS_SIZE; //sample window:adding buffer since ADAC is the source for interrupt
	AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA

	AdcbRegs.ADCSOC0CTL.bit.CHSEL = 2;  //SOC0 will convert ADCINB2
	AdcbRegs.ADCSOC0CTL.bit.ACQPS = ACQPS_SIZE; //sample window
	AdcbRegs.ADCSOC0CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA

	AdcaRegs.ADCSOC1CTL.bit.CHSEL = 3;  //SOC1 will convert ADCINA3
	AdcaRegs.ADCSOC1CTL.bit.ACQPS = ACQPS_SIZE; //sample window:adding buffer since ADAC is the source for interrupt
	AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA

	AdcbRegs.ADCSOC1CTL.bit.CHSEL = 3;  //SOC1 will convert ADCINB3
	AdcbRegs.ADCSOC1CTL.bit.ACQPS = ACQPS_SIZE; //sample window
	AdcbRegs.ADCSOC1CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA

	//Output Voltage
	AdcaRegs.ADCSOC2CTL.bit.CHSEL = 4;  //SOC2 will convert ADCINA4
	AdcaRegs.ADCSOC2CTL.bit.ACQPS = ACQPS_SIZE; //sample window
	AdcaRegs.ADCSOC2CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA

	AdcbRegs.ADCSOC2CTL.bit.CHSEL = 4;  //SOC2 will convert ADCINB4
	AdcbRegs.ADCSOC2CTL.bit.ACQPS = ACQPS_SIZE; //sample window
	AdcbRegs.ADCSOC2CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA

	AdcaRegs.ADCSOC3CTL.bit.CHSEL = 5;  //SOC3 will convert ADCINA5
	AdcaRegs.ADCSOC3CTL.bit.ACQPS = ACQPS_SIZE; //sample window
	AdcaRegs.ADCSOC3CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA

	AdcbRegs.ADCSOC3CTL.bit.CHSEL = 5;  //SOC3 will convert ADCINB5
	AdcbRegs.ADCSOC3CTL.bit.ACQPS = ACQPS_SIZE; //sample window
	AdcbRegs.ADCSOC3CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA

	//Output Current
	AdcaRegs.ADCSOC4CTL.bit.CHSEL = 14;  //SOC4 will convert ADCIN14
	AdcaRegs.ADCSOC4CTL.bit.ACQPS = ACQPS_SIZE; //sample window
	AdcaRegs.ADCSOC4CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA

	AdcdRegs.ADCSOC4CTL.bit.CHSEL = 0;  //SOC4 will convert ADCIND0
	AdcdRegs.ADCSOC4CTL.bit.ACQPS = ACQPS_SIZE; //sample window
	AdcdRegs.ADCSOC4CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA

	AdcaRegs.ADCSOC5CTL.bit.CHSEL = 15;  //SOC5 will convert ADCIN15
	AdcaRegs.ADCSOC5CTL.bit.ACQPS = ACQPS_SIZE; //sample window
	AdcaRegs.ADCSOC5CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA

	AdcdRegs.ADCSOC5CTL.bit.CHSEL = 1;  //SOC5 will convert ADCIND1
	AdcdRegs.ADCSOC5CTL.bit.ACQPS = ACQPS_SIZE; //sample window
	AdcdRegs.ADCSOC5CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA

	AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0x5; //end of SOC5 will set INT1 flag
	AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
	AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
	#endif

	EDIS;

}
