#include "F28x_Project.h"
#define SIZEOFIPC_RX 4
#define SIZEOFIPC_TX 5

void IPC_RX(Uint16 * array);
void IPC_TX(Uint16 * array);
void Reset_remote_IPC(void);
void Wait_memory_access(void);
void IPC_TO_RS485Interpreter(void);
