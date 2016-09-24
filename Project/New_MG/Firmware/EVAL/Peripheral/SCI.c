#include "F28x_Project.h"
#include "SCI.h"
#include "../Comm/Comm_USB.h"

void SCIA_init()
{
    //Configurate GPIO pins
	EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1; //Refer to datasheet
    GpioCtrlRegs.GPADIR.bit.GPIO28 = 0; //SCI_A_RX as input
    GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0; //pull-up
    GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 0; //syn
//
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1; //Refer to datasheet
    GpioCtrlRegs.GPADIR.bit.GPIO29 = 1; //SCI_A_TX as output
    GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0; //pull-up
    GpioCtrlRegs.GPAQSEL2.bit.GPIO29 = 3; //ascyn mode
	EDIS;

    SCIA_fifo_init();

    ClkCfgRegs.LOSPCP.bit.LSPCLKDIV = 2;//LSPCLK @ 50MHz = SYSCLK(200MHz)/4
    CpuSysRegs.PCLKCR7.bit.SCI_A = 1; //Initial SCI_A clock
 	SciaRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
	SciaRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
                                   // Disable RX ERR, SLEEP, TXWAKE
	SciaRegs.SCICTL2.all =0x0003;
	SciaRegs.SCICTL2.bit.TXINTENA =1;
	SciaRegs.SCICTL2.bit.RXBKINTENA =1;

    // BAUD register = LSPLK/(BAUD*8) - 1 @ 1041666 BAUD
	SciaRegs.SCIHBAUD.all    =0x0000;
    SciaRegs.SCILBAUD.all    =0x0005;

	SciaRegs.SCICTL1.all =0x0023;  // Relinquish SCI from Reset

}

void SCIA_fifo_init()
{
    SciaRegs.SCIFFTX.all=0xE040; //TXFFIL = 0 TXFFIENA = 1
    SciaRegs.SCIFFRX.all=0x2060 +4; //RXFFIL = 5 RXFFIENA = 1
    SciaRegs.SCIFFCT.all=0x0; //Auto BAUD disabled
}

void SCIA_xmit(int a)
{
    while (SciaRegs.SCIFFTX.bit.TXFFST != 0) {}
    SciaRegs.SCITXBUF.all =a;
}

void SCIA_msg(char * msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
    	SCIA_xmit(msg[i]);
        i++;
    }
}
