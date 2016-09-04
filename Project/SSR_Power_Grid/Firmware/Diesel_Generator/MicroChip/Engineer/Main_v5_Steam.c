//Engineer
#include <xc.h>
#include <stdint.h>
#include <p33EP512GM710.h>
#include <pps.h>
#include <uart.h>
#include <math.h>
#include <libq.h>

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
#define LED1 LATDbits.LATD12
#define LED2 LATDbits.LATD13
#define LED3 LATCbits.LATC8
#define LED4 LATDbits.LATD5
#define LED5 LATDbits.LATD6
#define LED6 LATCbits.LATC9

#define D_EN LATCbits.LATC7
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Definition">
#define pi 3.14159265359    //value of pi
#define pi23 2.094395102    //value of 2/3*pi
#define wl 12               //sliding window length of DFT
#define wlf 12              //sliding window length for frequency smoothing filter
#define bufN 14             //total memory for buffer should be wl+2
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Variables">
unsigned char varMG[12];        // Data to be sent to central controller
unsigned char command;          // Data received from Central Controller
unsigned int receive;           // Data received from Central Controller
unsigned char frontbit;         // First bit of the 9-bit communication
unsigned char address;          // Received address
unsigned char varCPU[14];       // Variables to be sent to CPU
unsigned int dummy;

unsigned long V[3];              // Measured Voltages
unsigned long I[3];              // Measured Currents
unsigned int T;                 // Measured time between clock pulses
unsigned int PHI;               // Current Phase
unsigned int DATAR;             // For SPI

unsigned char flag_main;        // Flag that times the main operation cycle
unsigned char flag_MG;          // Flag that indicates reception from Microgrid Central Controller
unsigned char flag_adc=0;       // Flag that indicates that ADC has finished
unsigned char flag_speed=0;
unsigned char mux_select=0;     // Flag that helps to store ADC into specific location of buffer
unsigned char flag_clk;         // Flag that indicates that a clock pulse has been detecte

unsigned char kk;               // Counter 1
unsigned int  jj;                // Counter 2
unsigned int  V_O = 0;           // output voltage
unsigned char cont;             // Dummy counter

unsigned int  NoM = 3;           //SPI send
unsigned int  VIW[3] = {0};      //SPI send
unsigned char Dummy;            // Dummy
unsigned int  ctr, speedcounter = 0;           //counter
unsigned int  speedref, voltageref = 0;           //counter
unsigned int  Duty1, Duty2 = 0;           //counter
int speed = 0;          //Rotor Speed

unsigned char device;           // Device address

unsigned char status;           // Indicate Status
// </editor-fold>


/****************************** Routines **************************************/
void rs_transmit(unsigned int bus)
{
	U2TXREG = bus;				// Loads contents of bus to the Tx Register
	while(!U2STAbits.TRMT);
        for (kk = 1; kk < 100; kk++)
        Nop();
}

void SendDataCPU(void)
{
    // Send data to Computer via USB
    for (kk = 0; kk < 14; kk++)
    {
        U1TXREG = varCPU[kk];
        while(!U1STAbits.TRMT);
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
        Nop();
    }
    for (kk = 0; kk < 30; kk++)
        Nop();
}

void SendDataMG(void)
{
    // Send data to Microgrid Central Controller
    for (kk = 0; kk < 11; kk++)
    {
        rs_transmit(varMG[kk]);
//        U2TXREG = varMG[kk];
//        while(!U2STAbits.TRMT);
//        for (jj = 0; jj < 100; jj++)
//            Nop();
    }
}

void Delay_us(unsigned int delay)
{
    unsigned char i;
    for (i = 0; i < delay; i++)
    {
        __asm__ volatile ("repeat #39");
        __asm__ volatile ("nop");
    }
}

/******************* Interruption service routines ****************************/

void __attribute__ ( (interrupt, no_auto_psv) ) _T1Interrupt( void )
{
    IFS0bits.T1IF = 0;
    if (speedcounter == 20)
    {
        speedcounter =0;
        speed = VEL1CNT;
        if (speed < 0)
              speed = speed*-1;
        flag_speed = 1;
    }
    speedcounter++;
    flag_main = 1;
}

void __attribute__ ( (interrupt, no_auto_psv) ) _U2RXInterrupt( void )
{
    while(!U2STAbits.URXDA);

    receive = U2RXREG;

    if (receive >= 256)
    {
        frontbit = 1;
	address = receive%256;		// Extracts 8 bit address from 9 bits received (8bits + frontbit)
    }
    else
    	frontbit = 0;

    if (frontbit)
    {
        if (address == device)
            U2STAbits.ADDEN = 0;				// Disable Address Detect to listen for the next data byte
        frontbit = 0;
    }
    else if (!frontbit)
    {
        command = receive%256;
        flag_MG = 1;
        U2STAbits.ADDEN = 1;
    }

    U2STAbits.OERR = 0;
    IFS1bits.U2RXIF = 0;
}

void __attribute__ ( (interrupt, no_auto_psv) ) _AD1Interrupt( void )
{
    I[0] = ADC1BUF0;
    V[2] = ADC1BUF1;
    V[1] = ADC1BUF2;
    V[0] = ADC1BUF3;
    IFS0bits.AD1IF = 0;
}

void __attribute__ ( (interrupt, no_auto_psv) ) _IC1Interrupt( void )
{
    flag_clk = 1;
    T = TMR2;
    while(IC1CON1bits.ICBNE)
        dummy = IC1BUF;
    TMR2 = 0;
    IFS0bits.IC1IF = 0;
}

void __attribute__ ( (interrupt, no_auto_psv) ) _IC2Interrupt( void )
{
    PHI = TMR2;
    IFS0bits.IC2IF = 0;
}

void __attribute__ ( (interrupt, no_auto_psv) ) _SPI2Interrupt( void )
{
    while (!SPI2STATbits.SPIRBF);
    DATAR = SPI2BUF;
    switch(DATAR)
    {
        case 111:
            LED6 = 0;
            LED4 = 1;
            break;
        case 999:
            LED6 = 1;
            LED4 = 0;
            break;
        case 333:
            NoM = 3;
            for(kk=0;kk<NoM;kk++)
            {
                while (SPI2STATbits.SPITBF);  // Wait till TXB is clear
                SPI2BUF = VIW[kk]; //continuous write
                while (!SPI2STATbits.SPIRBF);  // Wait till RXB is set
                Dummy = SPI2BUF;
            }
            break;
        case 100:
            while (SPI2STATbits.SPITBF);  // Wait till TXB is clear
            SPI2BUF = Dummy; //continuous write
            while (!SPI2STATbits.SPIRBF);  // Wait till RXB is set
            speedref = SPI2BUF;
            break;
        case 101:
            while (SPI2STATbits.SPITBF);  // Wait till TXB is clear
            SPI2BUF = Dummy; //continuous write
            while (!SPI2STATbits.SPIRBF);  // Wait till RXB is set
            voltageref = SPI2BUF;
            break;
        default:
            SPI2BUF = 000; //Idle
            LED6 = 1;
            LED4 = 0;
            break;
    }
    LED5 =0;
    IFS2bits.SPI2IF = 0;
}

/**************************** Configuration ***********************************/
void Configuration(void)
{
    // <editor-fold defaultstate="collapsed" desc="Configure Watchdog Timer">
    RCONbits.SWDTEN = 0;                        // Disable Dog Watch Timer
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Configure Oscillator">
    // Use equations: Fosc= Fin*M/(N1*N2), Fcy=Fosc/2. To operate at 70MIPS use Fin = 8MHz (external XT), M = 70, N1 = 2, N2 = 2
    PLLFBD = 68;                                // 68 M = (PLLBD + 2) = 70
    CLKDIVbits.PLLPOST = 0;                     // 1 N2 = 2*(PLLPOST + 1) = 4
    CLKDIVbits.PLLPRE = 0;                        // 0 N1 = (PLLPRE + 2) = 2
    __builtin_write_OSCCONH( 0x03 );            // Initiate Clock Switch to Primary
    __builtin_write_OSCCONL( OSCCON | 0x01 );  // Start clock switching
    while( OSCCONbits.COSC != 0b011 );
    while (OSCCONbits.LOCK != 1);
    
//    PLLFBD = 68;                                // M = (PLLBD + 2) = 65
//    CLKDIVbits.PLLPOST = 1;                     // N2 = 2*(PLLPOST + 1) = 2
//    CLKDIVbits.PLLPRE = 0;                        // N1 = (PLLPRE + 2) = 2
//    __builtin_write_OSCCONH( 0x01);            // Initiate Clock Switch to Internal
//    __builtin_write_OSCCONL(OSCCON | 0x01);    // Start clock switching
//    while( OSCCONbits.COSC != 0b001 );
//    while (OSCCONbits.LOCK != 1);
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Configure Timer 1">
    T1CON = 0;          // Timer reset
    T1CONbits.TCKPS = 0b00;
    IFS0bits.T1IF = 0;  // Reset Timer1 interrupt flag
    IPC0bits.T1IP = 6;  // Timer1 Interrupt priority level=7
    IEC0bits.T1IE = 1;  // Enable Timer1 interrupt
    PR1 = 35000;        // Timer1 period register 2kHz (35000)
    TMR1=0;             //Timer1 reset
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Configure RS485 Communication">
    CloseUART2();
    U2BRG = 17;               // Baud rate
    IEC1bits.U2RXIE = 0;        // Turn on receive interruption
    IFS1bits.U2RXIF = 0;        // clear the interrupt flag
    IPC7bits.U2RXIP = 6;        // Maximum priority
    U2MODEbits.UARTEN = 0;      // Keep UART2 disabled for now
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
    U2STAbits.UTXEN = 1;        // Enable transmission
    U2STAbits.ADDEN = 1;        // Address character detect is enabled
    U2STAbits.OERR = 0;         // Clear overrun error
    U2STAbits.URXISEL = 0b00;   // Interrupt when the fourth byte has been received
    // </editor-fold>   //used for communication between the PMUs but currently disabled

    // <editor-fold defaultstate="collapsed" desc="Configure USB Communication">
    CloseUART1();
    U1BRG = 8;                  // Baud rate = 8MHz
    IEC0bits.U1RXIE = 0;        // Turn on receive interruption
    IFS0bits.U1RXIF = 0;        // clear the interrupt flag
    IPC2bits.U1RXIP = 4;        // Maximum priority
    U1MODEbits.UARTEN = 1;      // Keep UART1 disabled for now
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

    // <editor-fold defaultstate="collapsed" desc="Configure AD Converter">
    IEC0bits.AD1IE = 0;
    IFS0bits.AD1IF = 0;
    IPC3bits.AD1IP = 5;

    AD1CON4bits.ADDMAEN = 0;        // DMA is not used
    AD1CON1bits.ADON = 0;           // Keep the AD module off fir now
    AD1CON1bits.ADSIDL = 0;         // Continue operation during idle mode
    AD1CON1bits.AD12B = 0;          // 4 channel 10 bit operation
    AD1CON1bits.FORM = 0b00;        // Non-signed integer output
    AD1CON1bits.SSRC = 0b000;       // PWM1 trigger
    AD1CON1bits.SSRCG = 1;          // PWM trigger
    AD1CON1bits.SIMSAM = 1;         // Enable simultaneous sampling
    AD1CON1bits.ASAM = 1;           // Automatic sampling
    AD1CON1bits.SAMP = 0;           // Sampling bit. Set to 1 to start conversion
    AD1CON1bits.DONE = 0;           // Indicates when conversion is complete
    AD1CON2bits.VCFG = 0b000;       // AVdd and AVss are the references for conversion
    AD1CON2bits.CSCNA = 0;          // Dont scan inputs
    AD1CON2bits.CHPS = 0b11;        // Converts CH0, CH1, CH2 and CH3
    AD1CON2bits.SMPI = 0;           // Interrupt after N+1 completitions of conversion
    AD1CON2bits.BUFM = 0;           // One 16-word buffer
    AD1CON2bits.ALTS = 0;           // Alternate between MUXA and MUXB
    AD1CON3bits.ADRC = 0;           // ADC conversion clock derived from system clock
    AD1CON3bits.SAMC = 0b1111;       // Sample time is 31 Tad
    AD1CON3bits.ADCS = 30;          // ADC Conversion Clock is (ADCS+1)*Tp
    AD1CON4bits.ADDMAEN = 0;        // DMA is not used
    
    AD1CHS0bits.CH0SA = 22;          // Ch0 - AN22, Current
    AD1CHS0bits.CH0NA = 0;          // Channel 0 negative input is Vrefl

    AD1CHS123bits.CH123SA0 = 0;          // Ch1 - RA0;
    AD1CHS123bits.CH123SA1 = 0;          // Ch2 - RA1;
    AD1CHS123bits.CH123SA2 = 0;          // Ch3 - RB0;
    AD1CHS123bits.CH123NA = 0;      // CH1, CH2, CH3 negative input is Vrefl


    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Configure Clock Capture (IC1)">
    IEC0bits.IC1IE = 1;                 // Enable Caputure 1 Interruptions
    IFS0bits.IC1IF = 0;
    IPC0bits.IC1IP = 2;

    IC1CON1bits.ICSIDL = 0;             // Continues operation in idle mode
    IC1CON1bits.ICTSEL = 0b001;         // T2CLK 2 is the clock source for the IC1 module
    IC1CON1bits.ICI = 0;                // Interrupt in every capture event
    IC1CON1bits.ICM = 0;                // Keep Caputre module off for now
    IC1CON2bits.IC32 = 0;               // Disable cascade module operation
    IC1CON2bits.ICTRIG = 1;             // Input source is used to trigger the IC timer (Trigger mode)

    T2CON = 0;
    T2CONbits.TON = 0;                  // Keep Timer 2 off for now
    T2CONbits.TCKPS = 0b11;             // Timer prescaler is 256
    T2CONbits.T32 = 0;                  // Timer 2 is a 16 bit counter
    T2CONbits.TCS = 0;                  // Use internal clock
    IEC0bits.T2IE = 0;                  // Disable Timer 2 interruptions
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Configure Phase Calculation Capture (IC2)">
    IEC0bits.IC2IE = 0;                 // Keep Caputure 2 Interruptions disabled for now
    IFS0bits.IC2IF = 0;
    IPC1bits.IC2IP = 7;

    IC2CON1bits.ICSIDL = 0;             // Continues operation in idle mode
    IC2CON1bits.ICTSEL = 0b001;         // Timer 3 is the clock source for the IC1 module
    IC2CON1bits.ICI = 0;                // Interrupt in every capture event
    IC2CON1bits.ICM = 0;                // Keep Caputre module off for now
    IC2CON2bits.IC32 = 0;               // Disable cascade module operation
    IC2CON2bits.ICTRIG = 1;             // Input source is used to trigger the IC timer (Trigger mode)

    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Configure Ports">
    TRISAbits.TRISA0 = 1;       // Analog input 6
    TRISAbits.TRISA1 = 1;       // Analog input 7
    TRISAbits.TRISA8 = 1;       // U2RX is input

    TRISBbits.TRISB0 = 1;       // Analog Input 8
    TRISBbits.TRISB1 = 1;       // Analog Input 9
    TRISBbits.TRISB2 = 1;       // Analog Input 10
    TRISBbits.TRISB3 = 1;       // Analog Input 11
    TRISBbits.TRISB4 = 0;       // U2TX is output
    TRISBbits.TRISB5 = 1;
    TRISBbits.TRISB6 = 1;
    TRISBbits.TRISB7 = 0;       // SDO2 Output
    TRISBbits.TRISB8 = 1;       // SCK2 Input
    TRISBbits.TRISB9 = 1;
    TRISBbits.TRISB12 = 1;      // Capture pin is input
    TRISBbits.TRISB13 = 1;      // Capture pin is input

    TRISC = 0x0000;
    TRISCbits.TRISC1 = 1;       // Input Capture 2 pin
    TRISCbits.TRISC2 = 1;       // Input Capture 3 pin
    TRISCbits.TRISC13 = 1;      // SDI2 Input

    TRISD = 0x0000;
    TRISFbits.TRISF9 = 1;
    TRISGbits.TRISG15 = 1;
    TRISGbits.TRISG10 = 1;      // Analog Input 1
    TRISGbits.TRISG6 = 1;       //RG6 is an input - QEA1
    TRISGbits.TRISG7 = 1;       //RG7 is an input - QEB1

    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSELG = 0;
    ANSELAbits.ANSA0 = 1;
    ANSELAbits.ANSA1 = 1;
    ANSELBbits.ANSB0 = 1;
    ANSELBbits.ANSB1 = 1;
    ANSELBbits.ANSB2 = 1;
    ANSELBbits.ANSB3 = 1;
    ANSELGbits.ANSG10 = 1;
    
    LATBbits.LATB4 = 1;
    LATCbits.LATC6 = 1;
    
    D_EN = 0;
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Configure Peripheral Pin Select">
    RPINR18bits.U1RXR = 0b0011000;          // Connect RPI24 to U1RX
    RPOR1bits.RP36R = 0b000001;             // Connect RP36 to U1TX

    RPINR19bits.U2RXR = 0b0101001;          // Connect RP41 to U2RX
    RPOR6bits.RP54R = 0b000011;             // Connect RP54 to U2TX
    
    RPINR22bits.SDI2R = 0b0111101;          // Connect SDI2 to RPI61
    RPOR2bits.RP39R = 0b001000;             // Connect SDO2 to RP39
    RPINR22bits.SCK2R = 0b0101000;          // Connect SCK2 to RP40 as input

    RPINR7bits.IC1R = 0b0101100;            // Connect IC1 to RPI44 - Clock Input
    RPINR7bits.IC2R = 0b0110001;            // Connect IC2 to RPI49 - Current zero-crossing

    RPINR0bits.INT1R = 0b1111111;           // Connect INT1 to RP127

    RPINR14bits.QEA1R = 0b1110110;           // Connect QEA1 to RP118
    RPINR14bits.QEB1R = 0b1110111;           // Connect QEB1 to RP119
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Configure SPI (Slave)">
    IFS2bits.SPI2IF = 0; // Clear the Interrupt flag
    IEC2bits.SPI2IE = 0; // Disable the interrupt
    IPC8bits.SPI2IP = 7; // Priority

    SPI2CON1bits.DISSCK = 0; // Internal serial clock is disabled
    SPI2CON1bits.DISSDO = 0; // SDOx pin is controlled by the module
    SPI2CON1bits.MODE16 = 1; // Communication is word-wide (16 bits)
    SPI2CON1bits.MSTEN = 0; // Slave mode enabled
    SPI2CON1bits.SMP = 0; // Input data is sampled at the middle of data output time
    SPI2CON1bits.CKE = 0; // Serial output data changes on transition from
    SPI2CON1bits.SSEN = 0; // SSX pin is diabled
    SPI2CON1bits.CKP = 0; // Idle state for clock is a low level;

    SPI2CON2bits.FRMEN = 0;  // Framed SPI1 support is disables
    SPI2CON2bits.SPIBEN = 0; // Enhanced buffer is disabled

    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="PWM 1&5">
    PTCONbits.PTEN = 0;           // disbaled for now
    PTCON2bits.PCLKDIV = 0b000;   // PWM input clock prescaler 1:1
    PTPER = 7000;                 // PWM period 14000
    PHASE1 = 0;                   // PWM1 phase shift
    PHASE5 = 0;                   // PWM5 phase shift
    PDC1 = 0;                     // PWM1 Duty reset
    PDC5 = 0;                     // PWM5 Duty reset
    DTR1 = 45;                    // PWM1 Dead Time  45
    DTR5 = 45;                    // PWM5 Dead Time  45
    ALTDTR1 = 45;
    ALTDTR5 = 45;
    IOCON1 = 0xC000;              // PWM1 Complementary
    IOCON5 = 0xC000;              // PWM5 Complementary
    PWMCON1 = 0x0000;             // Primay Time base, Edge-Aligned, Independent Duty
    PWMCON5 = 0x0000;             // Primay Time base, Edge-Aligned, Independent Duty
    FCLCON1 = 0x0003;             // Faults
    FCLCON5 = 0x0003;             // Faults
    
    /* Trigger ADC*/
    TRIG1 = 6000;  //12000
    TRGCON1bits.TRGDIV = 0;       // Prescalar 1:1
    TRGCON1bits.TRGSTRT = 0;       // Postscalar 1:1
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="QEI">
    QEI1CONbits.QEIEN = 0;         // Disable QEI1 for now
    QEI1CONbits.INTDIV = 0b000;    // 1:1 prescale
    QEI1STATbits.VELOVIEN = 0;     // Disable overflow interrupt
    QEI1IOCbits.QFDIV = 0b000;     // 1:1 clock divide


    // </editor-fold>
}

/**************************** Initialization **********************************/
void Initialization(void)
{
    // <editor-fold defaultstate="collapsed" desc="Initialize Interrupt Flags">
    flag_main = 0;
    flag_MG = 0;
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Initialize RS485 Module">
    device = 4;
    kk = 1;
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Initialize LED">
    LED6 = 0;
    LED5 = 0;
    LED4 = 0;
    LED3 = 0;
    LED2 = 0;
    LED1 = 0;
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Start Timer 1">
    TMR1 = 0;
    T1CONbits.TON = 1;
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Start Clock Capture module 1">
    TMR2 = 0;
    T2CONbits.TON = 1;                  // Enable Timer 2
    IC1CON1bits.ICM = 0b010;            // Capture Mode, Rising Edge
    IEC0bits.IC1IE = 1;                 // Enable Caputure 1 Interruptions
    T = 0;
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Start Clock Capture module 2">
    IC2CON1bits.ICM = 0b010;            // Capture Mode, Rising Edge
    IEC0bits.IC2IE = 0;                 // Diable Caputure 2 Interruptions
    // </editor-fold>//disabled

    // <editor-fold defaultstate="collapsed" desc="Start AD Converter">
    IEC0bits.AD1IE = 1;     
    AD1CON1bits.ADON = 1;
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Initialize data to CPU">
    U1MODEbits.UARTEN = 0;      // Enable UART 1
    IEC0bits.U1RXIE = 0;        // Turn on receive interruption

    for (kk = 1; kk < 13; kk++)
        varCPU[kk] = kk;

    varCPU[0] = 111;
    varCPU[13] = 222;
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Start SPI2">
    SPI2STATbits.SPIEN = 1;                 // Enable SPI2
    SPI2STATbits.SPIROV = 0;
    IFS2bits.SPI2IF = 0; // Clear the Interrupt flag
    IEC2bits.SPI2IE = 1; // Enable the interrupt
//    SPI2BUF = 0;
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Start PWM">
    PTCONbits.PTEN = 1;           // Enable PWM1
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Start QEI">
    QEI1CONbits.QEIEN = 1;         // Enable QEI1
    // </editor-fold>
}

/******************************* Main *****************************************/
int main( void )
{
    Configuration();
    Initialization();

    // <editor-fold defaultstate="collapsed" desc="Q16 Variables">
    _Q16 w_pu = _Q16ftoi(0);
    _Q16 temp0, temp1, temp2, temp3, temp4 = _Q16ftoi(0);
    _Q16 probe0 = _Q16ftoi(0);
    _Q16 Tsfreq = _Q16ftoi(0.01);

        // <editor-fold defaultstate="collapsed" desc="Speed Regulator">
        _Q16 wref, flowHP = _Q16ftoi(0);
        _Q16 Pref = _Q16ftoi(0.5);
        _Q16 inv_Rp = _Q16ftoi(20*0.2);
        _Q16 Kp = _Q16ftoi(1);
        _Q16 inv_Tsr = _Q16ftoi(1000);
        _Q16 inv_Tsm = _Q16ftoi(6.6667);
        _Q16 vgmax = _Q16ftoi(0.1);
        _Q16 vgmin = _Q16ftoi(-0.1);
        _Q16 gmax = _Q16ftoi(4.496);
        _Q16 gmin = _Q16ftoi(0);
        // </editor-fold>
        
        // <editor-fold defaultstate="collapsed" desc="Steam Turbine">
        _Q16 Pboil = _Q16ftoi(1);
        _Q16 inv_T5 = _Q16ftoi(2);
        _Q16 inv_T4 = _Q16ftoi(0.3030);
        _Q16 inv_T3 = _Q16ftoi(0.1);
        _Q16 F2 = _Q16ftoi(0.5);
        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="Two Mass">
        _Q16 inv_TMass = _Q16ftoi(0.596);
        _Q16 MassGain = _Q16ftoi(0.3469);
        _Q16 Te_est = _Q16ftoi(0);
        _Q16 w1_ref = _Q16ftoi(0);
        // </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="Inner Speed Loop">
        _Q16 Kp_InnerSpeed = _Q16ftoi(0.596);
        _Q16 Ki_InnerSpeed = _Q16ftoi(0.596);
        // </editor-fold>
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Q16 Constant">
    _Q16 speed2pu = _Q16ftoi(15.9155);


    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Q16 Integrator Output">
    _Q16 SpeedRelayOutput = _Q16ftoi(0);
    _Q16 Gate = _Q16ftoi(0);
    _Q16 Stage5Output = _Q16ftoi(0);
    _Q16 Stage4Output = _Q16ftoi(0);
    _Q16 Stage3Output = _Q16ftoi(0);
    _Q16 Stage2Output = _Q16ftoi(0);
    _Q16 MassOutput = _Q16ftoi(0);
    // </editor-fold>
    while(1)
    {
        if (flag_main)
        {
            LED2 = ~LED2;
            flag_main = 0;                  // Clear main flag

            // <editor-fold defaultstate="collapsed" desc="Speed Loop">
            if (flag_speed)
            {
                flag_speed = 0;
                w_pu = _Q16mac(_Q16ftoi(speed), speed2pu, 0);
                wref = _Q16mac(_Q16ftoi(0.01), _Q16ftoi(speedref),0);

                // <editor-fold defaultstate="collapsed" desc="Speed Regulator">
                temp0 = _Q16mac((wref-w_pu),inv_Rp ,0);
                temp1 = temp0+Pref;
                temp2 = _Q16mac(temp1,Kp ,0);
                temp3 = temp2 - _Q16mac(flowHP,Kp-_Q16ftoi(1),0);
                //speed Relay
                SpeedRelayOutput = temp3;
                // Servo-Motor
                temp0 = SpeedRelayOutput - Gate;
                probe0 = temp0;
                temp1 = _Q16mac(temp0,inv_Tsm ,0);
                if (temp1 > vgmax) temp1 = vgmax;
                if (temp1 < vgmin) temp1 = vgmin;
                Gate = _Q16mac(temp1,Tsfreq ,0) + Gate;
                if (Gate > gmax) Gate = gmax;
                if (Gate < gmin) Gate = gmin;
                // </editor-fold>
                                
                // <editor-fold defaultstate="collapsed" desc="Steam Turbine">
                //Stage 5 
                temp0 = _Q16mac(Pboil,Gate,0);
                temp1 = temp0 - Stage5Output;
                temp2 = _Q16mac(temp1,inv_T5,0);
                temp3 = _Q16mac(temp2,Tsfreq,0);
                Stage5Output = Stage5Output + temp3;
                flowHP = Stage5Output;
                //Stage 4
                temp0 = Stage5Output - Stage4Output;
                temp1 = _Q16mac(temp0,inv_T4,0);
                temp2 = _Q16mac(temp1,Tsfreq,0);
                Stage4Output = Stage4Output + temp2;
                //Stage 3
                temp0 = Stage4Output - Stage3Output;
                temp1 = _Q16mac(temp0,inv_T3,0);
                temp2 = _Q16mac(temp1,Tsfreq,0);
                Stage3Output = Stage3Output + temp2;
                //Stage 2
                Stage2Output = _Q16mac(Stage3Output,F2,0);
                // </editor-fold>        

                // <editor-fold defaultstate="collapsed" desc="Two Mass">
                temp0 =_Q16mac(Stage2Output,_Q16ftoi(2),0);
                temp1 = temp0 - Te_est;
                temp2 = temp1 - MassOutput;
                temp3 = _Q16mac(temp2,inv_TMass,0);
                temp4 = _Q16mac(temp3,Tsfreq,0);
                MassOutput = MassOutput + temp4;
                w1_ref = _Q16mac(MassOutput,MassGain,0);
                // </editor-fold>

                // <editor-fold defaultstate="collapsed" desc="Inner Speed Loop">
                w1_ref
                // </editor-fold>
            }    
            // </editor-fold>

             if (LED4 == 0)
            {
                SpeedRelayOutput = _Q16ftoi(0);
                Gate = _Q16ftoi(0);
                Stage5Output = _Q16ftoi(0);
                Stage4Output = _Q16ftoi(0);
                Stage3Output = _Q16ftoi(0);
                Stage2Output = _Q16ftoi(0);
                MassOutput = _Q16ftoi(0);
            }
            else
            {

            }
            
//            if (integ01_pre < 0)
//                integ01_pre = -1*integ01_pre;

            VIW[0] = (w1_ref*100)>>16; //Voltage Magnitude
            VIW[1] = (wref*100)>>16; //Voltage Magnitude
            VIW[2] = 2;
            kk++;
            ctr = ctr+1;

        }
    }
}




