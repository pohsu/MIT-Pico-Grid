#include "F28x_Project.h"
#include "DAC.h"
#include <stdint.h>
#include <math.h>

#define DAC_RESOLUTION 4096

volatile struct DAC_REGS* DAC_PTR[4] = {0x0,&DacaRegs,&DacbRegs,&DaccRegs};

void DAC_init(void)
{
	CpuSysRegs.PCLKCR16.bit.DAC_A = 1; // Enable DAC_A clocking
	CpuSysRegs.PCLKCR16.bit.DAC_B = 1; // Enable DAC_B clocking
	CpuSysRegs.PCLKCR16.bit.DAC_C = 1; // Enable DAC_C clocking

	// Configurate all the DACs
	configureDAC(1); //same as ADCA0
	configureDAC(2); //same as ADCA1
	configureDAC(3); //same as ADCB0

}

void configureDAC(Uint16 dac_num)
{
	EALLOW;
	DAC_PTR[dac_num]->DACCTL.bit.DACREFSEL = 1; //VDAC as reference 
	DAC_PTR[dac_num]->DACCTL.bit.LOADMODE = 0; //immediate load
	DAC_PTR[dac_num]->DACOUTEN.bit.DACOUTEN = 1; //Enable DAC
	DAC_PTR[dac_num]->DACVALS.all = 0; // value reset
	DELAY_US(10); // Delay for buffered DAC to power up
	EDIS;
}

void DACA(float32 value, float32 range)
{
	DAC_PTR[1]->DACVALS.all = (Uint16) (( value / range + 1.0f ) * DAC_RESOLUTION / 2);
}

void DACB(float32 value, float32 range)
{
	DAC_PTR[2]->DACVALS.all = (Uint16) (( value / range + 1.0f ) * DAC_RESOLUTION / 2);
}

void DACC(float32 value, float32 range)
{
	DAC_PTR[3]->DACVALS.all = (Uint16) (( value / range + 1.0f )  * DAC_RESOLUTION / 2);
}	

void uDACA(float32 value, float32 range)
{
	DAC_PTR[1]->DACVALS.all = (Uint16) (value / range * DAC_RESOLUTION);
}	

void uDACB(float32 value, float32 range)
{
	DAC_PTR[2]->DACVALS.all = (Uint16) (value / range * DAC_RESOLUTION);
}	

void uDACC(float32 value, float32 range)
{
	DAC_PTR[3]->DACVALS.all = (Uint16) (value / range * DAC_RESOLUTION);
}	
