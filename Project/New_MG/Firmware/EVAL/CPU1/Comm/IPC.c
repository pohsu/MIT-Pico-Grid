#include "F28x_Project.h"
#include "IPC.h"


//***********************************************************************//
//                     G l o b a l  V a r i a b l e s                    //
//***********************************************************************//
Uint16 sizeofdata_rx = 0;

bool enable = 0; 

struct_report_data report_data = {
    .Volt  = 0, 
    .Freq = 0,   
};
struct_console_data console_data = {
	.Vref = 0,
    .Fref  = 0,    
};

//***********************************************************************//
//                          F u n c t i o n s                            //
//***********************************************************************//
void Send_to_CPU2(Uint16 * array)
{
	Uint16 i;
	array[0] = SIZEOFDATA_TX;
	for(i=1;i<=SIZEOFDATA_TX/2;i++) array[i] = *((float32 *)&report_data + i - 1); //(float32 *) is to convert the pointer type to make ptr+1 equal to the size of a member  
    
    IpcRegs.IPCSET.bit.IPC0 = 1; //Set the remote CPU interrupt	
}

void Decoder_Process_IPC (Uint16 * array)
{
	sizeofdata_rx = array[0];

	if (array[1] == STOP)
	{
		//stop PWM
		enable = 0;
	} 

	if (array[1] == START) enable = 1;

	if (array[1] == FREF) console_data.Fref = (float32)array[4];

	if (array[1] == VREF) console_data.Vref = (float32)array[4];

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
