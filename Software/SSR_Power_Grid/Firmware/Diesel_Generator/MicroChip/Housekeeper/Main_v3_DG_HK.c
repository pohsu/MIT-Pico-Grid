//Housekeeper
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
unsigned int command;          // Data received from Central Controller
unsigned int receive;           // Data received from Central Controller
unsigned char frontbit;         // First bit of the 9-bit communication
unsigned char address;          // Received address
unsigned char varCPU[3];       // Variables to be sent to CPU
unsigned int dummy;

unsigned long V[3];              // Measured Voltages
unsigned long I[3];              // Measured Currents
unsigned int T;                 // Measured time between clock pulses
unsigned int PHI;               // Current Phase

unsigned char flag_main;        // Flag that times the main operation cycle
unsigned char flag_MG;          // Flag that indicates reception from Microgrid Central Controller
unsigned char flag_adc=0;       // Flag that indicates that ADC has finished
unsigned char mux_select=0;     // Flag that helps to store ADC into specific location of buffer
unsigned char flag_clk;         // Flag that indicates that a clock pulse has been detecte

unsigned char kk;               // Counter 1
unsigned int jj;                // Counter 2
unsigned char cont;             // Dummy counter
unsigned int DATASPI[3] = {0};  // SPI
unsigned int SPICONSOLE;        // SPI
unsigned int NoM;               // SPI Number of measurement
unsigned int goflag = 999, gotemp = 999;
unsigned int ptr485 = 0, value = 0;

unsigned int Dummy = 0;             // For *
unsigned int dataCPU[6] = {0};  // USB
unsigned int ctr, ctr2 = 0;             // For *
unsigned int command0, command1, command2 = 0;             // For *
unsigned int change = 0, report =0;             // For *

unsigned char device;           // Device address

unsigned char status;           // Indicate Status
// </editor-fold>


/****************************** Routines **************************************/
void rs_transmit(unsigned int bus)
{
        U2TXREG = bus;				// Loads contents of bus to the Tx Register
	while(!U2STAbits.TRMT);
        for (jj = 1; jj < 20; jj++)
        Nop(); 
}

void SendDataCPU(void)
{
    // Send data to Computer via USB
    for (kk = 0; kk < 6; kk++)
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

    D_EN = 1;
    for (jj = 1; jj < 20; jj++)  //avoid glitching
        Nop();
    for (kk = 0; kk < 12; kk++)
    {
        rs_transmit(varMG[kk]);
    }
    D_EN = 0;
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

void Delay_Nop(unsigned int delay)
{
    unsigned char i;
    for (i = 0; i < delay; i++)
    {
        Nop();
    }
}

void SPITXRX(unsigned int SPICONSOLE)
{
    switch(SPICONSOLE)
    {
        case 111:
            SPI2BUF = 111; //START
            while (!SPI2STATbits.SPIRBF);  // Wait till RXB is set
            Dummy = SPI2BUF;
            break;
        case 999:
            SPI2BUF = 999; //STOP
            while (!SPI2STATbits.SPIRBF);  // Wait till RXB is set
            Dummy = SPI2BUF;
            break;
        case 333:
            SPI2BUF = 333; //READ
            
            while (!SPI2STATbits.SPIRBF);  // Wait till RXB is set
            Dummy = SPI2BUF;
            NoM = 3; //Number of Measurement
            for(kk=0;kk < NoM;kk++)
            {
                Delay_Nop(3); //Wait for Slave to prepare the data
                while (SPI2STATbits.SPITBF);  // Wait till TXB is clear
                SPI2BUF = 333; 
                while (!SPI2STATbits.SPIRBF);  // Wait till RXB is set
                DATASPI[kk] = SPI2BUF;
            }
            break;
         case 100:
            SPI2BUF = 100; //
            while (!SPI2STATbits.SPIRBF);  // Wait till RXB is set
            Dummy = SPI2BUF;
            Delay_Nop(3); //Wait for Slave to prepare the data
            while (SPI2STATbits.SPITBF);  // Wait till TXB is clear
            SPI2BUF = command0;
            while (!SPI2STATbits.SPIRBF);  // Wait till RXB is set
            Dummy = SPI2BUF;
            break;
        case 101:
            SPI2BUF = 101; //
            while (!SPI2STATbits.SPIRBF);  // Wait till RXB is set
            Dummy = SPI2BUF;
            Delay_Nop(3); //Wait for Slave to prepare the data
            while (SPI2STATbits.SPITBF);  // Wait till TXB is clear
            SPI2BUF = command1;
            while (!SPI2STATbits.SPIRBF);  // Wait till RXB is set
            Dummy = SPI2BUF;
            break;
        case 102:
            SPI2BUF = 102; //
            while (!SPI2STATbits.SPIRBF);  // Wait till RXB is set
            Dummy = SPI2BUF;
            Delay_Nop(3); //Wait for Slave to prepare the data
            while (SPI2STATbits.SPITBF);  // Wait till TXB is clear
            SPI2BUF = command2;
            while (!SPI2STATbits.SPIRBF);  // Wait till RXB is set
            Dummy = SPI2BUF;
            break;
        default:
            SPI2BUF = 000; //Idle
            break;
    }
}

/******************* Interruption service routines ****************************/

void __attribute__ ( (interrupt, no_auto_psv) ) _T1Interrupt( void )
{
    IFS0bits.T1IF = 0;
    flag_main = 1;
}

void __attribute__ ( (interrupt, no_auto_psv) ) _U2RXInterrupt( void )
{
    while(!U2STAbits.URXDA);
    receive = U2RXREG;
    if (receive >= 256)
    {
       address = receive%256;		// Extracts 8 bit address from 9 bits received (8bits + frontbit)
       if (address == device)
       {
            U2STAbits.ADDEN = 0;
            ptr485 = 1;
       }
       else
            ptr485 = 0;
    }
    else
    {
        switch(ptr485)
        {
            case 1:
                command = receive;
                ptr485 = 2;
                break;
            case 2:
                value = receive;
                ptr485 = 0;
                U2STAbits.ADDEN = 1;
                flag_MG = 1;
                if (value >0)
                    ctr2 = value;
                break;
            default:
                break;
        }
    }
    U2STAbits.OERR = 0;
    IFS1bits.U2RXIF = 0;
}

void __attribute__ ( (interrupt, no_auto_psv) ) _AD1Interrupt( void )
{
    dummy = ADC1BUF0;
    V[0] = ADC1BUF1;
    V[1] = ADC1BUF2;
    V[2] = ADC1BUF3;
    dummy = ADC1BUF4;
    I[0] = ADC1BUF5;
    I[1] = ADC1BUF6;
    I[2] = ADC1BUF7; //not required for single phase
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
}

void __attribute__ ( (interrupt, no_auto_psv) ) _U1RXInterrupt( void )
{
    while(!U1STAbits.URXDA);
    U1STAbits.OERR = 0;
    IFS0bits.U1RXIF = 0;
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
    IPC0bits.T1IP = 6;  // Timer1 Interrupt priority level=4
    IEC0bits.T1IE = 1;  // Enable Timer1 interrupt
    PR1 = 35000;        // Timer1 period register 2kHz
    TMR1=0;             //Timer1 reset
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Configure RS485 Communication">
    CloseUART2();
    U2BRG = 1;               // Baud rate
    IEC1bits.U2RXIE = 0;        // Turn on receive interruption
    IFS1bits.U2RXIF = 0;        // clear the interrupt flag
    IPC7bits.U2RXIP = 7;        // Maximum priority
    U2MODEbits.UARTEN = 1;      // Enable UART 2
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
    // </editor-fold> 

    // <editor-fold defaultstate="collapsed" desc="Configure USB Communication">
    CloseUART1();
    U1BRG = 17;                  // Baud rate = 1.0294MHz
    IEC0bits.U1RXIE = 0;        // Turn off receive interruption for now
    IFS0bits.U1RXIF = 0;        // clear the interrupt flag
    IPC2bits.U1RXIP = 1;        // Maximum priority
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
    U1STAbits.UTXEN = 1;        // Transmit is enabled
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
    AD1CON1bits.AD12B = 0;          // 4-channel 10 bit operation
    AD1CON1bits.FORM = 0b00;        // Non-signed integer output
    AD1CON1bits.SSRC = 0b111;       // Auto-sample mode with internal timer
    AD1CON1bits.SSRCG = 0;          // Same as above
    AD1CON1bits.SIMSAM = 1;         // Enable simultaneous sampling
    AD1CON1bits.ASAM = 0;           // No automatic sampling
    AD1CON1bits.SAMP = 0;           // Sampling bit. Set to 1 to start conversion
    AD1CON1bits.DONE = 0;           // Indicates when conversion is complete
    AD1CON2bits.VCFG = 0b000;       // AVdd and AVss are the references for conversion
    AD1CON2bits.CSCNA = 0;          // Dont scan inputs
    AD1CON2bits.CHPS = 0b11;        // Converts CH0, CH1, CH2 and CH3
    AD1CON2bits.SMPI = 1;           // Interrupt after N+1 completitions of conversion
    AD1CON2bits.BUFM = 0;           // One 16-word buffer
    AD1CON2bits.ALTS = 1;           // Alternate between MUXA and MUXB
    AD1CON3bits.ADRC = 0;           // ADC conversion clock derived from system clock
    AD1CON3bits.SAMC = 0b1111;       // Sample time is 31 Tad
    AD1CON3bits.ADCS = 30;          // ADC Conversion Clock is (ADCS+1)*Tp
    AD1CON4bits.ADDMAEN = 0;        // DMA is not used

    AD1CHS123bits.CH123SB0 = 0;     // CH1 - AN0, CH2 - AN1, CH3 - AN2
    AD1CHS123bits.CH123SB1 = 0;     // CH1 - AN0, CH2 - AN1, CH3 - AN2
    AD1CHS123bits.CH123SB2 = 0;     // CH1 - AN0, CH2 - AN1, CH3 - AN2
    AD1CHS123bits.CH123NB = 0;      // CH1, CH2, CH3 negative input is Vrefl
    AD1CHS123bits.CH123SA0 = 1;     // CH1 - AN3, CH2 - AN4, CH3 - AN5
    AD1CHS123bits.CH123SA1 = 0;     // CH1 - AN3, CH2 - AN4, CH3 - AN5
    AD1CHS123bits.CH123SA2 = 0;     // CH1 - AN3, CH2 - AN4, CH3 - AN5
    AD1CHS123bits.CH123NA = 0;      // CH1, CH2, CH3 negative input is Vrefl

    AD1CHS0bits.CH0NB = 0;          // Channel 0 negative input is Vrefl
    AD1CHS0bits.CH0SB = 0b111111;   // Channel 0 is unconnected
    AD1CHS0bits.CH0NA = 0;
    AD1CHS0bits.CH0SA = 0b111111;

    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Configure Clock Capture (IC1)">
    IEC0bits.IC1IE = 1;                 // Enable Caputure 1 Interruptions
    IFS0bits.IC1IF = 0;
    IPC0bits.IC1IP = 3;

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
    IPC1bits.IC2IP = 0;

    IC2CON1bits.ICSIDL = 0;             // Continues operation in idle mode
    IC2CON1bits.ICTSEL = 0b001;         // Timer 3 is the clock source for the IC1 module
    IC2CON1bits.ICI = 0;                // Interrupt in every capture event
    IC2CON1bits.ICM = 0;                // Keep Caputre module off for now
    IC2CON2bits.IC32 = 0;               // Disable cascade module operation
    IC2CON2bits.ICTRIG = 1;             // Input source is used to trigger the IC timer (Trigger mode)

    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Configure Ports">
    TRISAbits.TRISA0 = 1;       // Analog input 1
    TRISAbits.TRISA1 = 1;       // Analog input 2
    TRISAbits.TRISA8 = 1;       // U2TX is output

    TRISBbits.TRISB0 = 1;       // Analog Input 3
    TRISBbits.TRISB1 = 1;       // Analog Input 4
    TRISBbits.TRISB2 = 1;       // Analog Input 5
    TRISBbits.TRISB3 = 1;       // Analog Input 6
    TRISBbits.TRISB4 = 0;       // U2RX is Input
    TRISBbits.TRISB5 = 1;
    TRISBbits.TRISB6 = 1;
    TRISBbits.TRISB7 = 0;       // SDO2 Output
    TRISBbits.TRISB8 = 0;       // SCK2 Output
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
    
    LATBbits.LATB4 = 1;
    LATCbits.LATC6 = 1;
    
    D_EN = 0;
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Configure Peripheral Pin Select">

    RPINR18bits.U1RXR = 0b0011000;          // Connect RPI24 to U1RX (Bug)
    RPOR1bits.RP36R = 0b000001;             // Connect RP36 to U1TX (Bug)

    RPINR19bits.U2RXR = 0b0101001;          // Connect RP41 to U2RX
    RPOR6bits.RP54R = 0b000011;             // Connect RP54 to U2TX

//    RPINR19bits.U2RXR = 0b0110110;          // Connect RP54 to U2RX
//    RPOR3bits.RP41R = 0b000011;             // Connect RP41 to U2TX
    
    RPINR22bits.SDI2R = 0b0111101;          // Connect SDI2 to RPI61
    RPOR2bits.RP39R = 0b001000;             // Connect SDO2 to RP39
    RPINR22bits.SCK2R = 0b0101000;          // Connect SCK2 to RP40 as input
    RPOR3bits.RP40R = 0b001001;             // Connect SCK2 to RP40 as output

    RPINR7bits.IC1R = 0b0101100;            // Connect IC1 to RPI44 - Clock Input
    RPINR7bits.IC2R = 0b0110001;            // Connect IC2 to RPI49 - Current zero-crossing

    RPINR0bits.INT1R = 0b1111111;           // Connect INT1 to RP127
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Configure SPI (Master)">
    IFS2bits.SPI2IF = 0; // Clear the Interrupt flag
    IEC2bits.SPI2IE = 0; // Disable the interrupt
    IPC8bits.SPI2IP = 1; // INT Priority

    SPI2CON1bits.DISSCK = 0; // Internal serial clock is enabled
    SPI2CON1bits.DISSDO = 0; // SDOx pin is controlled by the module
    SPI2CON1bits.MODE16 = 1; // Communication is word-wide (16 bits)
    SPI2CON1bits.MSTEN = 1; // Master mode enabled
    SPI2CON1bits.SMP = 0; // Input data is sampled at the middle of data output time
    SPI2CON1bits.CKE = 0; // Serial output data changes on transition from
    SPI2CON1bits.SSEN = 0; // SSX pin is diabled
    SPI2CON1bits.CKP = 0; // Idle state for clock is a low level;
 
    //SPI2CON2bits.FRMEN = 0;  // Framed SPI1 support is disables
    //SPI2CON2bits.SPIBEN = 0; // Enhanced buffer is disabled

    // Prescaler
    SPI2CON1bits.SPRE =0b110; // Secondary 4:1
    SPI2CON1bits.PPRE =0b10; // Primary 4:1


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
    device = 9; //1 = DG01; 5 = Inverter; 9 = DG02
    kk = 1;
    D_EN = 0;
    IEC1bits.U2RXIE = 1;        // Turn on receive interruption
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
    IEC0bits.IC1IE = 0;                 // Enable Caputure 1 Interruptions
    T = 0;
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Start Clock Capture module 2">
    IC2CON1bits.ICM = 0b010;            // Capture Mode, Rising Edge
    IEC0bits.IC2IE = 0;                 // Diable Caputure 2 Interruptions
    // </editor-fold>//disabled

    // <editor-fold defaultstate="collapsed" desc="Start AD Converter">
    IEC0bits.AD1IE = 0;
    AD1CON1bits.ADON = 1;
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Initialize data to CPU">
    U1MODEbits.UARTEN = 1;      // Enable UART 1
    IEC0bits.U1RXIE = 1;        // Turn on receive interruption

    for (kk = 1; kk < 5; kk++)
        varCPU[kk] = kk;

    varCPU[0] = 111;
    varCPU[5] = 222;
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Start SPI2">
    SPI2STATbits.SPIEN = 1;                 // Enable SPI2
    SPI2STATbits.SPIROV = 0;
    IFS2bits.SPI2IF = 0; // Clear the Interrupt flag
    IEC2bits.SPI2IE = 0; // Enable the interrupt
    // </editor-fold> 
}

/******************************* Main *****************************************/
int main( void )
{
    Configuration();
    Initialization();

    while(1)
    {
        

        if (flag_main)
        {
            flag_main = 0;                  // Clear main flag
     

            LED1 = ~LED1;
            SPITXRX(333);
            if (change)
            {
                LED5 = ~LED5;
                change = 0;
                SPITXRX(goflag);
            }
//            varCPU[0] = 111;
//            varCPU[1] = 1;
//            varCPU[2] = 2;
//            varCPU[3] = 3;
//            varCPU[4] = ctr;
//            varCPU[5] = 222;
              ctr++;
//            SendDataCPU();
        }



//        flag_MG = 0;

        if (flag_MG)
        {
            switch(command)
            {
               case 1:
                  LED6 = 0;
                  LED4 = 1;
                  goflag = 111; //START
                  change = 1;
                  break;
               case 2:
                  LED4 = 0;
                  LED6 = 1;
                  goflag = 999;  //STOP
                  change = 1;
                  break;
               case 3:
                  report = 1;
                  break;
               case 4:
                  goflag = 100;
                  change = 1;
                  command0 = value;
                  break;
               case 5:
                  goflag = 101;
                  change = 1;
                  command1 = value;
                  break;
               case 6:
                  goflag = 102;
                  change = 1;
                  command2 = value;
                  break;
               default:
                  break;
            }
            
            varMG[0] = 111;
            varMG[1] = (char)(DATASPI[0] & 0x00FF);
            varMG[2] = (char)(DATASPI[0] >>8);
            varMG[3] = (char)(DATASPI[1] & 0x00FF);
            varMG[4] = (char)(DATASPI[1] >>8);
            varMG[5] = (char)(DATASPI[2] & 0x00FF);
            varMG[6] = (char)(DATASPI[2] >>8);
            varMG[7] = ctr;
            varMG[8] = 18;
            varMG[9] = 19;
            varMG[10] = 110;
            varMG[11] = 222;

            ctr++;
            report = 1;
            if (report == 1)
            {
                SendDataMG();
                report =0;
            }
            flag_MG = 0;

        }
    }
}




