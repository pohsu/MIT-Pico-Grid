#include <xc.h>
#include <stdint.h>
#include <p33EP512GM710.h>
#include <pps.h>
#include <uart.h>

// <editor-fold defaultstate="collapsed" desc="Configuration bits">
#pragma config JTAGEN = OFF         // JTAG Enable bit (JTAG is disabled)

// FPOR
#pragma config BOREN = ON           // Brown-out Reset (BOR) Detection Enable bit (BOR is enabled)
#pragma config ALTI2C1 = OFF        // Alternate I2C1 pins (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = OFF        // Alternate I2C2 pins (I2C2 mapped to SDA2/SCL2 pins)
#pragma config WDTWIN = WIN25       // Watchdog Window Select bits (WDT Window is 25% of WDT period)
#pragma config ICS = PGD2           // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)

// FWDT
#pragma config WDTPOST = PS32768    // Watchdog Timer Postscaler bits (1:32,768)
#pragma config WDTPRE = PR128       // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON          // PLL Lock Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF         // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF         // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FOSC
#pragma config POSCMD = XT          // Primary Oscillator Mode Select bits (XT Crystal Oscillator Mode)
#pragma config OSCIOFNC = OFF       // OSC2 Pin Function bit (OSC2 is clock output)
#pragma config IOL1WAY = OFF        // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSECMD       // Clock Switching Mode bits (Clock switching is enabled,Fail-safe Clock Monitor is disabled)

// FOSCSEL
#pragma config FNOSC = FRC          // Oscillator Source Selection (Internal Fast RC (FRC))
#pragma config PWMLOCK = OFF        // PWM Lock Enable bit is off
#pragma config IESO = OFF           // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FGS
#pragma config GWRP = OFF           // General Segment Write-Protect bit (General Segment may be written)
#pragma config GCP = OFF            // General Segment Code-Protect bit (General Segment Code protect is Disabled)
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Port Definitions">
#define LED1 LATCbits.LATC9
#define LED2 LATDbits.LATD6
#define LED3 LATDbits.LATD5
#define LED4 LATCbits.LATC8
#define LED5 LATDbits.LATD13
#define LED6 LATDbits.LATD12

#define D_EN LATCbits.LATC7
// </editor-fold>

/***************************** Variables **************************************/

// <editor-fold defaultstate="collapsed" desc="Variables">
unsigned int dataMG[12];        // Data received from Microgrid.
unsigned char countData;        // This variable indicates if the first 4 or second 4 bytes are being received from MG.
unsigned int InstMG[2];         // Instruction to be sent to Microgrid. Address + Command

unsigned char varDev[5][12];    // Variables from devices to send to computer
unsigned char varDevpre[5][12];    // Variables from devices to send to computer
unsigned char varSta[12];       // Variables describing the state of the system
unsigned char varCPU[72];       // Variables to send to User Interface (Computer)
unsigned int dataCPU[4];       // Variables from User Interface (Computer)
unsigned char flag_main;        // Flag that times the main operation cycle
unsigned char flag_CPU;         // Flag that indicates reception from the CPU
unsigned char flag_MG;          // Flag that indicates reception from Microgrid
unsigned char response;         // Flag that indicate the the device called responded before the call to the next element

unsigned char kk;               // Counter
unsigned char jj;

unsigned char seq_device[5];
unsigned char seq_inst[5];
unsigned char device;
unsigned char InstBuffer[4];

unsigned char trash;

unsigned char EM_STOP;      // Emergency Stop bit

unsigned int ctr = 0;        // Data received from Microgrid.
// </editor-fold>

/***************************** Routines ***************************************/
void SendDataCPU(void)
{
    for (kk = 0; kk < 72; kk++)
    {
        U1TXREG = varCPU[kk];
        while(!U1STAbits.TRMT);
    }
    for (kk = 0; kk < 50; kk++)
        Nop();
}

void SendDataMG(int address, int instruction, int value)
{
    D_EN = 1;
    INTCON2bits.GIE = 0;            //Disable All Int
    U2TXREG = address+256;          // Call a specific element in Microgrid
    while(!U2STAbits.TRMT);
    for (kk = 1; kk < 15; kk++)
        Nop();
    U2TXREG = instruction;                   // Send instruction to talk back
    while(!U2STAbits.TRMT);
    for (kk = 1; kk < 15; kk++)
        Nop();
    U2TXREG = value;                   // Send value
    while(!U2STAbits.TRMT);
    for (kk = 1; kk < 15; kk++)
        Nop();
    INTCON2bits.GIE = 1;            //Disable All Int
    D_EN = 0;
}

void LoadData()
{
for (jj = 0; jj < 5; jj++)
    for (kk = 0; kk < 12; kk++)
        varCPU[12*jj + kk] = varDev[jj][kk];
for (jj = 0; jj < 5; jj++)
{
     varCPU[12*jj +0] = 0;
     varCPU[12*jj +11] = 0;
}
varCPU[0]=111;
varCPU[59]=222;
for (kk = 0; kk < 4; kk++)
    varCPU[60+kk] = varSta[kk];
}

/******************* Interruption service routines ****************************/
void __attribute__ ( (interrupt, no_auto_psv) ) _INT1Interrupt( void )
{
    EM_STOP = 1;
    IFS1bits.INT1IF = 0;
}

void __attribute__ ( (interrupt, no_auto_psv) ) _T1Interrupt( void )
{
    IFS0bits.T1IF = 0;
    flag_main = 1;
}

void __attribute__ ( (interrupt, no_auto_psv) ) _U1RXInterrupt( void )
{
    while(!U1STAbits.URXDA);
    dataCPU[0] = U1RXREG;
    dataCPU[1] = U1RXREG;
    dataCPU[2] = U1RXREG;
    dataCPU[3] = U1RXREG;
    flag_CPU = 1;
    U1STAbits.OERR = 0;
    IFS0bits.U1RXIF = 0;
}

void __attribute__ ( (interrupt, no_auto_psv) ) _U2RXInterrupt( void )
{
    while(!U2STAbits.URXDA);
    if (countData == 1)
    {
        dataMG[0] = U2RXREG;
        dataMG[1] = U2RXREG;
        dataMG[2] = U2RXREG;
        dataMG[3] = U2RXREG;
        countData = 2;
    }
    else if (countData == 2)
    {
        dataMG[4] = U2RXREG;
        dataMG[5] = U2RXREG;
        dataMG[6] = U2RXREG;
        dataMG[7] = U2RXREG;
        countData = 3;
    }
    else
    {
        dataMG[8] = U2RXREG;
        dataMG[9] = U2RXREG;
        dataMG[10] = U2RXREG;
        dataMG[11] = U2RXREG;
        countData = 1;
        flag_MG = 1;

    }

    U2STAbits.OERR = 0;
    IFS1bits.U2RXIF = 0;
}


/**************************** Configuration ***********************************/
void Configuration(void)
{
    // <editor-fold defaultstate="collapsed" desc="Configure Watchdog Timer">
    RCONbits.SWDTEN = 0;                        // Disable Dog Watch Timer
    // </editor-fold>
    
    // <editor-fold defaultstate="collapsed" desc="Configure Oscillator">
    // Use equations: Fosc= Fin*M/(N1*N2), Fcy=Fosc/2. To operate at 70MIPS use Fin = 8MHz (external XT), M = 70, N1 = 2, N2 = 2
    PLLFBD = 68;                                // M = (PLLBD + 2) = 70
    CLKDIVbits.PLLPOST = 0;                     // N2 = 2*(PLLPOST + 1) = 2
    CLKDIVbits.PLLPRE = 0;                        // N1 = (PLLPRE + 1) = 2
    __builtin_write_OSCCONH( 0x03 );            // Initiate Clock Switch to Primary
    __builtin_write_OSCCONL( OSCCON || 0x01 );  // Start clock switching
    while( OSCCONbits.COSC != 0b011 );
    while (OSCCONbits.LOCK != 1);
    // </editor-fold>
    
    // <editor-fold defaultstate="collapsed" desc="Configure Timer 1">
    T1CON = 0;          // Timer reset
    T1CONbits.TCKPS = 0b01;
    PR1 = 35000;       // Timer1 period register
    // </editor-fold>
    
    // <editor-fold defaultstate="collapsed" desc="Configure PWM">
    PTCONbits.PTEN = 0;             // "PMU module enable bit"  Keep PWM module off for now
    PTCONbits.PTSIDL = 0;           // "PMU Time Base Stop in Idle mode bit" Disable halt on PMU time base during idle mode
    PTCONbits.SESTAT = 0;           // "Special Event Interrupt Status bit"
    PTCONbits.EIPU = 1;             // "Immediate Period Updates Bit" Enabling bit means that the "active Period" register is updated immediately

    //PTCON2 register is used to set PWM clock. PWM clock is a scaled version of the system clock Tosc.
    PTCON2bits.PCLKDIV = 0b111;     // These bits set the prescaler for the PWM clock. 111 corresponds to reserved

    PTPER = 9114;                   // This register sets the PWM switching Frequency. "Primary Master Time Base Period Register"
    PWMKEY = 0;                     // PWM Unlock Register
    PWMCON2bits.FLTIEN = 0;         // Disable Fault Interrupst
    PWMCON2bits.CLIEN = 0;          // Disable Current-Limit Interrupt
    PWMCON2bits.TRGIEN = 0;         // Disable Trigger Interrupt
    PWMCON2bits.MDCS = 0;           // "Master Duty Cycle Register Select Bit". Set Low means Duty cycle is determined by PDCx and SDCx instead of the Master Duty cycle
    PWMCON2bits.DTC = 0b10;         // "Dead Time Control Bits" 0b10 means Dead time function is disabled
    PWMCON2bits.MTBS = 0;           // "Master Time Base Select" Use primary master time base instead of secondary master time base.
    PWMCON2bits.CAM = 0;            // Edge aligned mode selected
    PWMCON2bits.XPRES = 0;          // External pins do not affect PWM time base
    PWMCON2bits.IUE = 1;            // Immediate updates
    IOCON2bits.PENH = 1;            // PWM2H is PWM pin
    IOCON2bits.PENL = 1;            // PWM2L is PWM pin
    IOCON2bits.POLH = 0;            // Polarity is active high
    IOCON2bits.POLL = 0;            // Polarity is active high
    IOCON2bits.PMOD = 0b11;         // Independent mode. PMOD allows us to set the duty cycle for PWMxH and PWMxL together or independantly.
    IOCON2bits.OVRENH = 0;
    IOCON2bits.OVRENL = 0;
    FCLCON2bits.IFLTMOD = 1;
    FCLCON2bits.CLMOD = 0;
    FCLCON2bits.FLTMOD = 0b11;

    //As we set IOCON2bits.PMOD to independant mode, we can have seperate duty cycles for PWM2H and PWM2L
    PDC2 = 4557;                    // "PWM Primary Generator Duty Cycle Register" This register sets the duty cycle for PWM2H. Depending on PMOD, this PDC can apply to both PWM2H and PWM2L
    SDC2 = 5250;                    // "PWM Secondary Generator Duty Cycle Register". This register sets the
    // </editor-fold>
    
    // <editor-fold defaultstate="collapsed" desc="Configure USB Communication">
    CloseUART1();
    U1BRG = 17;               // Baud rate =
    U1MODEbits.UARTEN = 1;      // Keep UART2 disabled for now
    U1MODEbits.USIDL = 0;       // Continue operation in idle mode
    U1MODEbits.IREN = 0;        // Disable irDA encoder/decoder
    U1MODEbits.RTSMD = 1;       // Simplex Mode
    U1MODEbits.UEN = 0b00;      // Flow control pins are controlled by PORT latches
    U1MODEbits.WAKE = 1;
    U1MODEbits.LPBACK = 0;      // No loopback
    U1MODEbits.ABAUD = 0;       // Disable Baud rate measurement
    U1MODEbits.URXINV = 0;      // Idle state is one
    U1MODEbits.BRGH = 1;        // BRG in high speed mode
    U1MODEbits.PDSEL = 0b00;    // 8-bit no parity
    U1MODEbits.STSEL = 0;       // One stop bit
    U1STAbits.UTXINV = 0;       // U2TX idle state is 1
    U1STAbits.UTXBRK = 0;       // Sends Sync Break is disabled
    U1STAbits.UTXEN = 1;        // Transmit is disabled for now
    U1STAbits.ADDEN = 0;        // Address character detect is disables
    U1STAbits.OERR = 0;         // Clear overrun error
    U1STAbits.URXISEL = 0b11;   // Interrupt when 4th byte is received
    // </editor-fold>
     
    // <editor-fold defaultstate="collapsed" desc="Configure RS485 Communication">
    CloseUART2();
    U2BRG = 1;               // Baud rate
    
    U2MODEbits.UARTEN = 1;      // Enable UART2
    U2MODEbits.USIDL = 0;       // Continue operation in idle mode
    U2MODEbits.IREN = 0;        // Disable irDA encoder/decoder
    U2MODEbits.RTSMD = 1;       // Simplex Mode
    U2MODEbits.UEN = 0b00;      // Flow control pins are controlled by PORT latches
    U2MODEbits.WAKE = 1;
    U2MODEbits.LPBACK = 0;      // No loopback
    U2MODEbits.ABAUD = 0;       // Disable Baud rate measurement
    U2MODEbits.URXINV = 0;      // Idle state is one
    U2MODEbits.BRGH = 1;        // BRG in high speed mode
    U2MODEbits.PDSEL = 0b11;    // 9-bit no parity
    U2MODEbits.STSEL = 0;       // One stop bit
    U2STAbits.UTXINV = 0;       // U2TX idle state is 1
    U2STAbits.UTXBRK = 0;       // Sends Sync Break is disabled
    U2STAbits.UTXEN = 1;        // Enable transmission - Transmission always has to be enabled after the UART!!!
    U2STAbits.ADDEN = 0;        // Address character detect is disables
    U2STAbits.OERR = 0;         // Clear overrun error
    U2STAbits.URXISEL = 0b11;   // Interrupt when the fourth byte has been received
    // </editor-fold>
    
    // <editor-fold defaultstate="collapsed" desc="Configure Emergency Stop">
    INTCON2bits.INT1EP = 0;
    // </editor-fold>
    
    // <editor-fold defaultstate="collapsed" desc="Configure Ports">
    TRISC = 0x0000;
    TRISD = 0x0000;
    TRISBbits.TRISB5 = 1;
    TRISBbits.TRISB6 = 1;
    TRISBbits.TRISB9 = 1;
    TRISBbits.TRISB12 = 0;      // PWM pin is output
    TRISBbits.TRISB13 = 0;      // PWM pin is output
    TRISAbits.TRISA8 = 1;       // U2RX is input
    TRISBbits.TRISB4 = 0;       // U2TX is output
    TRISGbits.TRISG15 = 1;
    ANSELG = 0;
    ANSELB = 0;
    LATBbits.LATB4 = 1;
    LATCbits.LATC6 = 1;
    D_EN = 0;
    // </editor-fold>
    
    // <editor-fold defaultstate="collapsed" desc="Configure Peripheral Pin Select">
    RPINR18bits.U1RXR = 0b0011000;          // Connect RPI24 to U1RX
    RPOR1bits.RP36R = 0b000001;             // Connect RP36 to U1TX

    RPINR19bits.U2RXR = 0b0101001;          // Connect RP41 to U2RX
    RPOR6bits.RP54R = 0b000011;             // Connect RP54 to U2TX

    RPINR0bits.INT1R = 0b1111111;           // Connect INT1 to RP127
    // </editor-fold>
}

/**************************** Initialization **********************************/
void Initialization(void)
{
    // <editor-fold defaultstate="collapsed" desc="Initialize Sequence of Polling">
    device = 0;
    
    seq_device[0] = 1; //A
    seq_device[1] = 5; //B
    seq_device[2] = 9; //C
    seq_device[3] = 10; //D
    seq_device[4] = 21; //E
    
    seq_inst[0] = 3; // 3- send data that you have
    seq_inst[1] = 3;
    seq_inst[2] = 3;
    seq_inst[3] = 15; // 15 - report status
    seq_inst[4] = 10; // 10 - report status
    // </editor-fold>
    
    // <editor-fold defaultstate="collapsed" desc="Initialize Interrupt Flags">
    flag_main = 0;
    flag_CPU = 0;
    flag_MG = 0;
    // </editor-fold>
    
    // <editor-fold defaultstate="collapsed" desc="Initialize LED">
    LED6 = 0;
    LED5 = 0;
    LED4 = 0;
    LED3 = 0;
    LED2 = 0;
    LED1 = 0;
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Initialize Variables">
    response = 1;
    EM_STOP = 0; 
    countData = 1;
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Initialize Arrays">
    for (kk = 0; kk < 72; kk++)
        varCPU[kk] = 0;

    for (kk = 0; kk < 4; kk++)
        dataCPU[kk] = kk;

    for (kk = 0; kk < 4; kk++)
        InstBuffer[kk] = kk;

    for (kk = 0; kk < 12; kk++)
    {
        varDev[0][kk] = 0;
        varDev[1][kk] = 0;
        varDev[2][kk] = 0;
        varDev[3][kk] = 0;
        varDev[4][kk] = 0;
    }

    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Start Timer">
    TMR1 = 0;
    T1CONbits.TON = 1;
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Start PWM">
    PTCONbits.PTEN = 1; // This enables the PWM module
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Priority Table">
    IEC0bits.T1IE = 1;          // Enable Timer1 interrupt
    IPC0bits.T1IP = 4;          // Timer1 priority
    IFS0bits.T1IF = 0;          // clear Timer1 interrupt flag

    IEC0bits.U1RXIE = 1;        // Turn on USB receive interruption
    IPC2bits.U1RXIP = 6;        // USB priority
    IFS0bits.U1RXIF = 0;        // clear USB interrupt flag

    IEC1bits.U2RXIE = 1;        // Turn on RS485 receive interruption
    IPC7bits.U2RXIP = 5;        // RS485 priority
    IFS1bits.U2RXIF = 0;        // clear RS485 interrupt flag

    IEC1bits.INT1IE = 0;        // Disable Emergency STOP interrupt
    IPC5bits.INT1IP = 7;        // RS485 Emergency STOP priority
    IFS1bits.INT1IF = 0;        // clear Emergency STOP interrupt flag

    INTCON2bits.GIE = 1;        // Initialize Global Interruptions
    // </editor-fold>

}

/******************************* Main *****************************************/
int main( void )
{
    Configuration();
    Initialization();

    while(1)
    {
        // <editor-fold defaultstate="collapsed" desc="Main Loop">
        if (flag_main) //Triggered by Timer1
        {
            LED1 = ~LED1;
            varSta[device] = 1;

            if (response == 0)              // If the package from previous call was not received then set byte 1 and 12 to 111
            {
                varSta[device] = 0;
                for (kk = 1; kk < 11; kk++)
                    varDev[device][kk] = 0;
            }
            else if (response == 2)         // If the package from the previous call was received incorrectly then restart UART2
            {
                varSta[device] = 2;
//                U2MODEbits.UARTEN = 0;
//                for (kk = 0; kk < 50; kk++)
//                    Nop();
//                U2MODEbits.UARTEN = 1;
//                U2STAbits.UTXEN = 1;
            }

            device = device + 1;

            if (device > 2)
            {
                LoadData();                     // Prepare data to be transmitted to CPU
                device = 0;
                SendDataCPU();
            }

            // <editor-fold defaultstate="collapsed" desc="Package Reception from CPU">
            if (flag_CPU)                       // Reception from CPU
            {
                LED3 = ~LED3;
                if (dataCPU[0] == seq_device[device])
                {
                    flag_CPU = 0;
                    SendDataMG(dataCPU[0], dataCPU[1], dataCPU[2]);
                }
                else
                    SendDataMG(seq_device[device], seq_inst[device], 0);
            }
            else
                SendDataMG(seq_device[device], seq_inst[device], 0);       // Call new device
            // </editor-fold>

            countData = 1;                                          // First four bits to be received
            response = 0;                                           // Clear the response flag


            flag_main = 0;                                          // Clear main flag
        }
        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="Package Reception from Microgrid">
        if (flag_MG)
        {
            
            if ((dataMG[0] == 111)&&(dataMG[11] == 222))    // If the frame is correct
            {
                response = 1;
                LED5 = ~LED5;
                // Indicate that package was received correctly
                for (kk = 0; kk < 12; kk++)                 // Write received data package into memory
                {
                    varDev[device][kk] = dataMG[kk];
                    varDevpre[device][kk] = dataMG[kk];
                }
            }
            else
            {
                LED5 = 0;
                response = 2;                               // If frame is incorrect, indicate that package was received incorrectly
                for (kk = 0; kk < 12; kk++)                 // Write received data package into memory
                    varDev[device][kk] = varDevpre[device][kk];
            }
        // After receiving the 12th byte, restart the UART. This is done to avoid
        // capturing the glitch occuring during the enable of the receiver
        U2MODEbits.UARTEN = 0;
        for (kk = 0; kk < 50; kk++)
            Nop();
        U2MODEbits.UARTEN = 1;
        U2STAbits.UTXEN = 1;
            flag_MG = 0;                    // Clear the flag
        }
        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="Emergency Stop Response">
        if (EM_STOP)
        {
            InstBuffer[0] = 200;                // This is a reserved address that calls all sources
            InstBuffer[1] = 2;                  // Command all sources to stop
        }
        // </editor-fold>
    }
}