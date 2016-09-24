#include "F28x_Project.h"
#include "Adc.h"

#define ACQPS_SIZE 14 //75ns for single-ended mode

void ADC_init(void)
{
    EALLOW;

    //
    //write configurations
    //
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcbRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdccRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCB, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCC, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

    //
    //Set pulse positions to late
    //
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdcbRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdccRegs.ADCCTL1.bit.INTPULSEPOS = 1;

    //
    //power up the ADC
    //
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1;

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
    // Simultaneous sampling configuration from the C2837x Manuel, Page 1319
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 2;  //SOC0 will convert ADCINA2
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = ACQPS_SIZE+1; //sample window:adding buffer since ADAC is the source for interrupt
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB

    AdcbRegs.ADCSOC0CTL.bit.CHSEL = 2;  //SOC0 will convert ADCINB2
    AdcbRegs.ADCSOC0CTL.bit.ACQPS = ACQPS_SIZE; //sample window
    AdcbRegs.ADCSOC0CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB

    AdccRegs.ADCSOC0CTL.bit.CHSEL = 2;  //SOC0 will convert ADCINC2
    AdccRegs.ADCSOC0CTL.bit.ACQPS = ACQPS_SIZE; //sample window
    AdccRegs.ADCSOC0CTL.bit.TRIGSEL = 6; //trigger on ePWM1 SOCB


    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0; //end of SOC0 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
    EDIS;

}
