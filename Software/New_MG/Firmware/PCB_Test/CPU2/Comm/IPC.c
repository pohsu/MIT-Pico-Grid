#include "F28x_Project.h"
#include "USB.h"
#include "IPC.h"

//***********************************************************************//
//                          F u n c t i o n s                            //
//***********************************************************************//
void Recieve_from_CPU1(Uint16 * array, Uint16 * data, Uint16 * size)
{
	Uint16 i;

	*size = array[0];
	for (i = 1; i <= *size/2; i++) data[i-1] = array[i];
}

void Send_to_CPU1(Uint16 * array, Uint16 * data)
{
	Uint16 i;

	array[0] = SIZEOFDATA_RX;
	for (i = 1; i <= SIZEOFDATA_RX; i++) array[i] = data[i-1];

	IpcRegs.IPCSET.bit.IPC0 = 1; //Set the remote CPU interrupt
}

void Reset_remote_IPC(void)
{
	IpcRegs.IPCACK.bit.IPC0 = 1; //clear the remote CPU interrupt
}

void Wait_memory_access(void)
{
    while(!( MemCfgRegs.GSxMSEL.bit.MSEL_GS0 &
 		    MemCfgRegs.GSxMSEL.bit.MSEL_GS14 ) )
    {
    }
}
