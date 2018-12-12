#include "F28x_Project.h"
#include "Comm.h"


//***********************************************************************//
//                     G l o b a l  V a r i a b l e s                    //
//***********************************************************************//
bool enable = 0; 

struct_IPC_tx IPC_tx = {
    .V = 0,
    .w = 0,
    .P  = 0,
    .Q  = 0,
};
struct_IPC_rx IPC_rx = {
	.vref = 0,
    .kp = 0,
    .kq = 0,
    .xm = 0,
    .rm = 0,
    .si = 0,
    .dt = 0,
    .dac = 0,
    .V_avg = 0,
    .w_avg = 0,
    .P_ref =  0,
    .Q_ref =  0,
    .sc_enable = 0,
};

//***********************************************************************//
//                          F u n c t i o n s                            //
//***********************************************************************//
void IPC_TX(Uint16 * array)
{
	array[0] = IPC_tx.V;
	array[1] = IPC_tx.w;
	array[2] = IPC_tx.P;
	array[3] = IPC_tx.Q;
	IpcRegs.IPCSET.bit.IPC0 = 1; //Set the remote CPU interrupt
}

void Decoder_Process_IPC (Uint16 * array)
{
    GpioDataRegs.GPCTOGGLE.bit.GPIO75 = 1; //LED2
	if (array[0] == CMD_STOP)
	{
		enable = 0; //stop PWM
		GpioDataRegs.GPCSET.bit.GPIO77 = 1; //LED3
	} 

	if (array[0] == CMD_START){
		enable = 1;
		GpioDataRegs.GPCCLEAR.bit.GPIO77 = 1; //LED3
	}

	if (array[0] == CMD_RM) IPC_rx.rm = array[1];

	if (array[0] == CMD_XM) IPC_rx.xm = array[1];

	if (array[0] == CMD_KP) IPC_rx.kp = array[1];

	if (array[0] == CMD_KQ) IPC_rx.kq = array[1];

	if (array[0] == CMD_VREF) IPC_rx.vref = array[1];

	if (array[0] == CMD_SI) IPC_rx.si = array[1];

	if (array[0] == CMD_DT) IPC_rx.dt = 1;

	if (array[0] == CMD_DAC) IPC_rx.dac = array[1];

	if (array[0] == CMD_SC) IPC_rx.sc_enable = array[1];

    IPC_rx.V_avg = array[2];
    IPC_rx.w_avg = array[3];
    IPC_rx.P_ref = array[4];
    IPC_rx.Q_ref = array[5];
}

void Reset_remote_IPC(void)
{
	IpcRegs.IPCACK.bit.IPC0 = 1; //clear the remote CPU interrupt
}

void Grant_meomory_access (void)
{
	// Give Memory Access to GS0/ GS14 SARAM to CPU02
	while( !(MemCfgRegs.GSxMSEL.bit.MSEL_GS0 &
			 MemCfgRegs.GSxMSEL.bit.MSEL_GS14))
	{
		EALLOW;
		MemCfgRegs.GSxMSEL.bit.MSEL_GS0 = 1;
		MemCfgRegs.GSxMSEL.bit.MSEL_GS14 = 1;
		EDIS;
	}
}
