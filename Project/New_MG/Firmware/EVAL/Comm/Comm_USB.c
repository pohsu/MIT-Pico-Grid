#include "F28x_Project.h"
#include "Comm_USB.h"
#include "../Peripheral/SCI.h"

//***********************************************************************//
//                     G l o b a l  V a r i a b l e s                    //
//***********************************************************************//

Uint16 data_tx[SIZEOFDATA_TX] = {0};
Uint16 data_rx[SIZEOFDATA_RX] = {0}; 
bool enable = 0; 

struct_report_data report_data = {
    .Volt  = 0, 
    .Freq = 0,   
};
struct_console_data console_data = {
    .Fref  = 0,
    .Vref = 0,
};

//***********************************************************************//
//                          F u n c t i o n s                            //
//***********************************************************************//
void report_USB(void)
{
    static Uint16 ctr = 0;
    SCIA_xmit(BEGIN);
    Uint16 i;
    for(i=0;i<SIZEOFDATA_TX/2;i++)
    {
    	float32 data = *((float32 *)&report_data + i); //(float32 *) is to convert the pointer type to make ptr+1 equal to the size of a member  
    	SCIA_xmit(data); //LSB;
    	SCIA_xmit(data/256); //MSB
    }	
    SCIA_xmit(END);
    ctr++;
}

void Decoder_Process_USB (void)
{
	Uint16 i;
	for (i = 0; i < SIZEOFDATA_RX; i++)
	{
		Uint16 j;
		j = (SIZEOFDATA_RX - 1) - (SciaRegs.SCIFFRX.bit.RXFFST - 1) % SIZEOFDATA_RX;

		data_rx[i] = (j == i)? SciaRegs.SCIRXBUF.all : data_rx[i]; //If data sequence matches, load the data; otherwises, do nth.
	}	
	if (data_rx[0] == STOP)
	{
		//stop PWM
		enable = 0;
	} 

	if (data_rx[0] == START) enable = 1;

	if (data_rx[0] == FREF) console_data.Fref = (float32)data_rx[3];

	if (data_rx[0] == VREF) console_data.Vref = (float32)data_rx[3];

}
