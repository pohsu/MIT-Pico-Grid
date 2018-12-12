#include "F28x_Project.h"
#include "Adc.h"
#include "../Test_setting.h"
#define ACQPS_SIZE 20 //15=75ns for single-ended mode (good : 20)

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

	//Inductor Current
	AdcaRegs.ADCSOC0CTL.bit.CHSEL = 2;  //SOC0 will convert ADCINA2
	AdcaRegs.ADCSOC0CTL.bit.ACQPS = ACQPS_SIZE; //sample window:adding buffer since ADAC is the source for interrupt
	AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA

	AdccRegs.ADCSOC0CTL.bit.CHSEL = 2;  //SOC0 will convert ADCINC2
	AdccRegs.ADCSOC0CTL.bit.ACQPS = ACQPS_SIZE; //sample window
	AdccRegs.ADCSOC0CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA

	AdcaRegs.ADCSOC1CTL.bit.CHSEL = 2;  //SOC0 will convert ADCINA2
	AdcaRegs.ADCSOC1CTL.bit.ACQPS = ACQPS_SIZE; //sample window:adding buffer since ADAC is the source for interrupt
	AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB

	AdccRegs.ADCSOC1CTL.bit.CHSEL = 2;  //SOC0 will convert ADCINC2
	AdccRegs.ADCSOC1CTL.bit.ACQPS = ACQPS_SIZE; //sample window
	AdccRegs.ADCSOC1CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB

	//Output Voltage
	AdcaRegs.ADCSOC2CTL.bit.CHSEL = 4;  //SOC1 will convert ADCINA4
	AdcaRegs.ADCSOC2CTL.bit.ACQPS = ACQPS_SIZE; //sample window:adding buffer since ADAC is the source for interrupt
	AdcaRegs.ADCSOC2CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB

	AdccRegs.ADCSOC2CTL.bit.CHSEL = 4;  //SOC1 will convert ADCINC4
	AdccRegs.ADCSOC2CTL.bit.ACQPS = ACQPS_SIZE; //sample window
	AdccRegs.ADCSOC2CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB

	AdcdRegs.ADCSOC2CTL.bit.CHSEL = 2;  //SOC1 will convert ADCIND2
	AdcdRegs.ADCSOC2CTL.bit.ACQPS = ACQPS_SIZE; //sample window
	AdcdRegs.ADCSOC2CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB

	//Output Current
    #if VERSION
	AdcaRegs.ADCSOC3CTL.bit.CHSEL = 14;  //SOC4 will convert ADCINA14
	AdcaRegs.ADCSOC3CTL.bit.ACQPS = ACQPS_SIZE; //sample window
	AdcaRegs.ADCSOC3CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB

    AdcbRegs.ADCSOC3CTL.bit.CHSEL = 2;  //SOC4 will convert ADCINB2
    AdcbRegs.ADCSOC3CTL.bit.ACQPS = ACQPS_SIZE; //sample window
    AdcbRegs.ADCSOC3CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB
    #else
    AdcbRegs.ADCSOC3CTL.bit.CHSEL = 2;  //SOC4 will convert ADCINB2
    AdcbRegs.ADCSOC3CTL.bit.ACQPS = ACQPS_SIZE; //sample window
    AdcbRegs.ADCSOC3CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB

    AdccRegs.ADCSOC3CTL.bit.CHSEL = 3;  //SOC4 will convert ADCINC3
    AdccRegs.ADCSOC3CTL.bit.ACQPS = ACQPS_SIZE; //sample window
    AdccRegs.ADCSOC3CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB
    #endif

	//Vdc
	AdccRegs.ADCSOC4CTL.bit.CHSEL = 5;  //SOC5 will convert ADCINC5 (A11)
	AdccRegs.ADCSOC4CTL.bit.ACQPS = ACQPS_SIZE; //sample window
	AdccRegs.ADCSOC4CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB

	//Dummy
	AdcaRegs.ADCSOC4CTL.bit.CHSEL = 5;  //SOC5 will convert ADCINA5
	AdcaRegs.ADCSOC4CTL.bit.ACQPS = ACQPS_SIZE; //sample window
	AdcaRegs.ADCSOC4CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB

	AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0x4; //end of SOC4 will set INT1 flag
	AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
	AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared

	EDIS;

}
