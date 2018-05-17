#include "F28x_Project.h"
#include "SCI.h"
#include "../Comm/USB.h"

void SCIA_init()
{
    SCIA_fifo_init();
    EALLOW;
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
    EDIS;

}

void SCIA_fifo_init()
{
    EALLOW;
    SciaRegs.SCIFFTX.all=0xE040; //TXFFIL = 0 TXFFIENA = 1
    SciaRegs.SCIFFRX.all=0x2060 +4; //RXFFIL = 5 RXFFIENA = 1
    SciaRegs.SCIFFCT.all=0x0; //Auto BAUD disabled
    EDIS;
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
