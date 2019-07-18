#include "F28x_Project.h"
#include "Comm.h"
#include "../Peripheral/SCI.h"

//***********************************************************************//
//                     G l o b a l  V a r i a b l e s                    //
//***********************************************************************//
Uint16 device_flag[NUMOFDEVICE] = {0};
Uint16 cast_flag[NUMOFDEVICE] = {0};
Uint16 RS485_addr[NUMOFDEVICE] = {0};
Uint16 RS485_tx[NUMOFDEVICE][SIZEOFRS485_TX] = {0};
Uint16 RS485_rx[NUMOFDEVICE][SIZEOFRS485_RX] = {0};
Uint16 RX_ctr = 0;


extern Uint16 usb_tx[SIZEOFUSB_TX];

//***********************************************************************//
//                          F u n c t i o n s                            //
//***********************************************************************//
void RS485_init(void)
{
    ScibRegs.SCICTL1.bit.RXENA = 1; //Enable SCIB RX
    GpioDataRegs.GPACLEAR.bit.GPIO13 = 0; //Disable RS485 Driver
    GpioDataRegs.GPACLEAR.bit.GPIO12 = 1; //Initialize RS485 Receiver

    RS485_addr[0] = ADDR_DEVICE1; //device addr
    RS485_addr[1] = ADDR_DEVICE2; //device addr
    RS485_addr[2] = ADDR_DEVICE3; //device addr
    Uint16 i;
    for (i = 0; i < NUMOFDEVICE; i++)
    {
        RS485_tx[i][0] = 222; //device command: 222: stop
        RS485_tx[i][1] = 0; //device value
    }
}

void RS485_RX(Uint16 device_rx)
{
    Uint16 addr = 0;
    if (ScibRegs.SCICTL1.bit.SLEEP && RX_ctr == 0)
    {
        addr = ScibRegs.SCIRXBUF.all;
        if (addr == ADDR_HOST)
        {
            ScibRegs.SCICTL1.bit.SLEEP = 0; //if address detected wake up
        }
    }
    else
    {
        RS485_rx[device_rx][RX_ctr] = ScibRegs.SCIRXBUF.all;
        RX_ctr ++;
    }

    if (RX_ctr == SIZEOFRS485_RX)
    {
        ScibRegs.SCICTL1.bit.SLEEP = 1; //back to sleep
        RX_ctr = 0; //ctr reset
        Uint16 i;
        for (i = 0; i < SIZEOFRS485_TX; i++) usb_tx[4*device_rx+i] = RS485_rx[device_rx][i];

    }

}

void RS485_TX(Uint16 device)
{
    GpioDataRegs.GPASET.bit.GPIO12 = 1; // Disable RS485 Receiver
    // adding delay?
    GpioDataRegs.GPASET.bit.GPIO13 = 1; // Enable RS485 Driver
    EALLOW;
    ScibRegs.SCICTL1.bit.TXENA = 1; //Enable TX
    ScibRegs.SCICTL1.bit.TXWAKE = 1;
    SCIB_xmit(cast_flag[device]? ADDR_BROADCAST: RS485_addr[device]);
    SCIB_xmit(device_flag[device]? RS485_tx[device][0]: CMD_NTH); // cmd or nth
    Uint16 i;
    for(i = 1; i < SIZEOFRS485_TX; i++)
        SCIB_xmit(RS485_tx[device][i]); // value, P_ref, Q_ref, w_avg, V_avg
    ScibRegs.SCICTL1.bit.TXENA = 0; //Disable TX
    EDIS;
    while (ScibRegs.SCICTL2.bit.TXEMPTY == 0) {}
    cast_flag[device] = 0;//Unflag
    device_flag[device] = 0; //Unflag
    GpioDataRegs.GPACLEAR.bit.GPIO13 = 1; // Disable RS485 Driver
    GpioDataRegs.GPACLEAR.bit.GPIO12 = 1; // Enable RS485 Receiver
}
