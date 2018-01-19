#include "F28x_Project.h"
#include "Comm.h"
#include "../Peripheral/SCI.h"

//***********************************************************************//
//                     G l o b a l  V a r i a b l e s                    //
//***********************************************************************//

Uint16 usb_tx[SIZEOFUSB_TX] = {0};
Uint16 usb_rx[SIZEOFUSB_RX] = {0};

extern Uint16 device_flag[NUMOFDEVICE];
extern Uint16 RS485_addr[NUMOFDEVICE];
extern Uint16 RS485_tx[NUMOFDEVICE][SIZEOFRS485_TX];
extern Uint16 RS485_rx[NUMOFDEVICE][SIZEOFRS485_RX];



//***********************************************************************//
//                          F u n c t i o n s                            //
//***********************************************************************//
void USB_TX(void)
{
    SCIA_xmit(BEGIN);
    Uint16 i;
    for(i=0;i<SIZEOFUSB_TX;i++) SCIA_xmit(usb_tx[i]);
    SCIA_xmit(END);
}

void USB_RX (void)
{
	Uint16 i;
	for (i = 0; i < SIZEOFUSB_RX; i++)
	{
		Uint16 j;
		j = (SIZEOFUSB_RX - 1) - (SciaRegs.SCIFFRX.bit.RXFFST - 1) % SIZEOFUSB_RX;

		usb_rx[i] = (j == i)? SciaRegs.SCIRXBUF.all : usb_rx[i]; //If data sequence matches, load the data; otherwises, do nth.
	}
}

void USB_TO_RS485Interpreter(void)
{
    Uint16 addr = 0;
    Uint16 i = 0;
    addr = usb_rx[1];
    //Matching Address
    for (i = 0; i < NUMOFDEVICE; i++)
    {
        if  ((RS485_addr[i] == addr)||(addr == ADDR_BROADCAST))
        {
            RS485_tx[i][0] = usb_rx[0]; // command
            RS485_tx[i][1] = usb_rx[3]; // value
            device_flag[i] = 1;
        }
    }
}
