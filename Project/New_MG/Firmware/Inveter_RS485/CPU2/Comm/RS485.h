#define SIZEOFRS485_TX 4
#define SIZEOFRS485_RX 2
#define ADDR_BROADCAST 0
#define ADDR_HOST 1
#define ADDR_DEVICE 102
#define CMD_NTH 0


void RS485_RX(void);
void RS485_TX(void);
void RS485_init(void);


