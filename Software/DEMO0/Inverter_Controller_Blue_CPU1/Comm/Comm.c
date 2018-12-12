#include "F28x_Project.h"
#include "Comm.h"

void Enable_Comm(void)
{
	Grant_meomory_access();// Give Memory Access to GS0/ GS14 SARAM to CPU02
	Reset_remote_IPC();// Reset remote IPC Interrupt to avoid initialization issues
}
