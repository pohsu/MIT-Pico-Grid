#define SIZEOFUSB_RX 4 //1 unit = 1 bytes or more via IPC
#define NUMOFDEVICE 3 // number of devices
#define SIZEOFUSB_TX 12 //1 unit = 4 bytes
#define BEGIN 111
#define END 222

void USB_TX(void);
void USB_RX (void);
void USB_TO_RS485Interpreter(void);


