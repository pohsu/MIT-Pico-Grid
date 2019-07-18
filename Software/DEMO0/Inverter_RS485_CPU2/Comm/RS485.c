#include "Comm.h"
#include "F28x_Project.h"
#include "../Peripheral/SCI.h"

//***********************************************************************//
//                     G l o b a l  V a r i a b l e s                    //
//***********************************************************************//

Uint16 RS485_tx[SIZEOFRS485_TX] = {0};
Uint16 RS485_rx[SIZEOFRS485_RX] = {0};
Uint16 RX_complete = 0, RX_cast = 0;

//***********************************************************************//
//                     L o c a l  V a r i a b l e s                      //
//***********************************************************************//
Uint16 RX_ctr = 0;

//***********************************************************************//
//                          F u n c t i o n s                            //
//***********************************************************************//
void RS485_RX(void)
{
    Uint16 addr = 0;
    if (ScibRegs.SCICTL1.bit.SLEEP)
    {
        addr = ScibRegs.SCIRXBUF.all;
        if (addr == ADDR_DEVICE || addr == ADDR_BROADCAST)
        {
            ScibRegs.SCICTL1.bit.SLEEP = 0; //if address detected wake up
            if(addr == ADDR_BROADCAST)
                RX_cast = 1;
            else
                RX_cast = 0;
        }
    }
    else
    {
        RS485_rx[RX_ctr] = ScibRegs.SCIRXBUF.all;
        RX_ctr ++;
    }

    if (RX_ctr == SIZEOFRS485_RX)
    {
//        GpioDataRegs.GPCTOGGLE.bit.GPIO83 = 1;
        ScibRegs.SCICTL1.bit.SLEEP = 1; //back to sleep
        RX_ctr = 0; //ctr reset
        RX_complete = 1; //RX completed ready to report
    }

}

void RS485_TX(void)
{
    GpioDataRegs.GPASET.bit.GPIO12 = 1; // Disable RS485 Receiver
    // adding delay?
    GpioDataRegs.GPASET.bit.GPIO13 = 1; // Enable RS485 Driver
    EALLOW;
    ScibRegs.SCICTL1.bit.TXENA = 1; //Enable TX
    ScibRegs.SCICTL1.bit.TXWAKE = 1;
    SCIB_xmit(ADDR_HOST); //send addr to the host
    Uint16 i;
    for (i = 0; i < SIZEOFRS485_TX; i++) SCIB_xmit(RS485_tx[i]);
    ScibRegs.SCICTL1.bit.TXENA = 0; //Disable TX
    EDIS;
    while (ScibRegs.SCICTL2.bit.TXEMPTY == 0) {}
    GpioDataRegs.GPACLEAR.bit.GPIO13 = 1; // Disable RS485 Driver
    GpioDataRegs.GPACLEAR.bit.GPIO12 = 1; // Enable RS485 Receiver
}

void RS485_init(void)
{
    ScibRegs.SCICTL1.bit.RXENA = 1; //Enable SCIB RX
    GpioDataRegs.GPACLEAR.bit.GPIO13 = 0; //Disable RS485 Driver
    GpioDataRegs.GPACLEAR.bit.GPIO12 = 1; //Initialize RS485 Receiver
}

