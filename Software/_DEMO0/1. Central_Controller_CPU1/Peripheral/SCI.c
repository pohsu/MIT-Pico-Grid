#include "F28x_Project.h"
#include "SCI.h"

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
    // SciaRegs.SCIFFTX.TXFFIENA = 0;
    // For transmitting words to labview, we send one word to fifo, then fifo sends over SCI/UART to labview
    SciaRegs.SCIFFTX.all=0xE040; // TXINTCLR = 1, TXFFIENA = 0, not using interrupt for transmitting to labview
    // When receiving words from labview, receive 4 words, and generate an interrupt.
    SciaRegs.SCIFFRX.all=0x2060 +4; //RXFFIL = 5 RXFFIENA = 1
    SciaRegs.SCIFFCT.all=0x0; //Auto BAUD disabled
    EDIS;
}

void SCIA_xmit(int a)
{
    while (SciaRegs.SCIFFTX.bit.TXFFST != 0) {}
    SciaRegs.SCITXBUF.all = a;
}

void SCIB_init()
{
//    SCIB_fifo_init();
    EALLOW;
    ClkCfgRegs.LOSPCP.bit.LSPCLKDIV = 2;//LSPCLK @ 50MHz = SYSCLK(200MHz)/4
    CpuSysRegs.PCLKCR7.bit.SCI_B = 1;  //Initial SCI_B clock

    ScibRegs.SCICCR.all =0x0000;
    ScibRegs.SCICCR.bit.STOPBITS = 0;  // 1 stop bit
    ScibRegs.SCICCR.bit.PARITYENA = 0; //No parity
    ScibRegs.SCICCR.bit.LOOPBKENA = 0; //No loopback
    ScibRegs.SCICCR.bit.ADDRIDLE_MODE = 1; //Address-bit mode
    ScibRegs.SCICCR.bit.SCICHAR = 0x7; // LENGTH = 8bits


    ScibRegs.SCICTL1.all =0x0000;  // Disable for now
    ScibRegs.SCICTL1.bit.SLEEP = 1; //Enable sleep mode with address byte detection

    ScibRegs.SCICTL2.all = 0x0000; // Disable for now
    ScibRegs.SCICTL2.bit.RXBKINTENA = 1; //Enable RXRDY Interrupt

    // BRR = LSPLK/(BAUD*8) - 1
    // or Baud = LSPCLK / ((BRR + 1) *8)  @ 132.978kbps / 5 ms update rate
    ScibRegs.SCIHBAUD.all = 0x0000;
    ScibRegs.SCILBAUD.all = 0x002E; //0x2E = 46
    ScibRegs.SCICTL1.bit.SWRESET = 1;  // Software Reset
    EDIS;

}

void SCIB_fifo_init()
{
    EALLOW;
    ScibRegs.SCIFFTX.all=0xE040; //TXFFIL = 0 TXFFIENA = 1
    ScibRegs.SCIFFRX.all=0x2060 +4; //RXFFIL = 5 RXFFIENA = 1
    ScibRegs.SCIFFCT.all=0x0; //Auto BAUD disabled
    EDIS;
}

void SCIB_xmit(int a)
{
    while (ScibRegs.SCICTL2.bit.TXRDY == 0) {}
    ScibRegs.SCITXBUF.all = a;
}

