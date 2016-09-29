#include "F28x_Project.h"
#include "USB.h"
#include "../Peripheral/SCI.h"

//***********************************************************************//
//                     G l o b a l  V a r i a b l e s                    //
//***********************************************************************//

Uint16 data_tx[SIZEOFDATA_TX_MAX/2] = {0};
Uint16 data_rx[SIZEOFDATA_RX] = {0}; 

Uint16 sizeofdata_tx = 0;

//***********************************************************************//
//                          F u n c t i o n s                            //
//***********************************************************************//
void Report_to_USB(void)
{
    static Uint16 ctr = 0;

    SCIA_xmit(BEGIN);
    Uint16 i;
    for(i=0;i<sizeofdata_tx/2;i++)
    {
    	SCIA_xmit(data_tx[i]); //LSB;
    	SCIA_xmit(data_tx[i]/256); //MSB
    }	
    SCIA_xmit(END);
    ctr++;
}

void Receive_from_USB (void)
{
	Uint16 i;
	for (i = 0; i < SIZEOFDATA_RX; i++)
	{
		Uint16 j;
		j = (SIZEOFDATA_RX - 1) - (SciaRegs.SCIFFRX.bit.RXFFST - 1) % SIZEOFDATA_RX;

		data_rx[i] = (j == i)? SciaRegs.SCIRXBUF.all : data_rx[i]; //If data sequence matches, load the data; otherwises, do nth.
	}
}
