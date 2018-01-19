#define SIZEOFRS485_TX 4
#define SIZEOFRS485_RX 4
#define ADDR_DEVICE1 100
#define ADDR_BROADCAST 0
#define ADDR_HOST 1


void RS485_TX(Uint16 device);
void RS485_RX(Uint16 device_rx);
void USB_RX (void);
void RS485_init(void);


