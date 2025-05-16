//-----------------------------
// Title: Final Project.c
//-----------------------------
// Purpose: This Project serves as a back up sensor for a car, The Ultrasonic sensor is interfaced with the PIC18F47K42 in order to indicate
// distance from the sensor. After using a timer to figure out travel time and calculating distance, That result is displayed on the LCD. When something
// is too close to the sensor, an interrupt will trigger causing a LED and Buzzer to go off informing the user that they are dangerously close to hitting
// something behind them. (This distance is currently set to 10 cm for demonstration purposes)
// Dependencies: stdio.h, stdlib.h, math.h, pic18f47k42.h, xc.h, string.h
// Compiler: XC8 (v3.0)
// Authors: Jair Pacheco, Cesar Jaime
// OUTPUTS: LCD(PortB0-7, RD0, RD1), LED(RD4), Buzzer(RD5), Ultra Sonic Sensor(Trigger[RD2])
// INPUTS: Poteniometer(LCD[Vo]), Ultra Sonic Sensor (ECHO[RD3])
// Versions:
//  	V1.0: 5/6/2025 - First version 
//    V1.2: 5/26/2025 - Fixed Clock configuration errors
//    V1.2: 5/28/2025 - Code clean up and While statement fixes 
//-----------------------------
// PIC18F46K42 Configuration Bit Settings
// 'C' source line config statements

// CONFIG1L
#pragma config FEXTOSC = LP     // External Oscillator Selection (LP (crystal oscillator) optimized for 32.768 kHz; PFM set to low power)
#pragma config RSTOSC = EXTOSC  // Reset Oscillator Selection (EXTOSC operating per FEXTOSC bits (device manufacturing default))

// CONFIG1H
#pragma config CLKOUTEN = OFF   // Clock out Enable bit (CLKOUT function is disabled)
#pragma config PR1WAY = ON      // PRLOCKED One-Way Set Enable bit (PRLOCK bit can be cleared and set only once)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)

// CONFIG2L
#pragma config MCLRE = EXTMCLR  // MCLR Enable bit (If LVP = 0, MCLR pin is MCLR; If LVP = 1, RE3 pin function is MCLR )
#pragma config PWRTS = PWRT_OFF // Power-up timer selection bits (PWRT is disabled)
#pragma config MVECEN = ON      // Multi-vector enable bit (Multi-vector enabled, Vector table used for interrupts)
#pragma config IVT1WAY = ON     // IVTLOCK bit One-way set enable bit (IVTLOCK bit can be cleared and set only once)
#pragma config LPBOREN = OFF    // Low Power BOR Enable bit (ULPBOR disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled , SBOREN bit is ignored)

// CONFIG2H
#pragma config BORV = VBOR_2P45 // Brown-out Reset Voltage Selection bits (Brown-out Reset Voltage (VBOR) set to 2.45V)
#pragma config ZCD = OFF        // ZCD Disable bit (ZCD disabled. ZCD can be enabled by setting the ZCDSEN bit of ZCDCON)
#pragma config PPS1WAY = ON     // PPSLOCK bit One-Way Set Enable bit (PPSLOCK bit can be cleared and set only once; PPS registers remain locked after one clear/set cycle)
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config DEBUG = OFF      // Debugger Enable bit (Background debugger disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Extended Instruction Set and Indexed Addressing Mode disabled)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period selection bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF       // WDT operating mode (WDT Disabled; SWDTEN is ignored)

// CONFIG3H
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4L
#pragma config BBSIZE = BBSIZE_512// Boot Block Size selection bits (Boot Block size is 512 words)
#pragma config BBEN = OFF       // Boot Block enable bit (Boot block disabled)
#pragma config SAFEN = OFF      // Storage Area Flash enable bit (SAF disabled)
#pragma config WRTAPP = OFF     // Application Block write protection bit (Application Block not write protected)

// CONFIG4H
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block not write-protected)
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)
#pragma config WRTSAF = OFF     // SAF Write protection bit (SAF not Write Protected)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored)

// CONFIG5L
#pragma config CP = OFF         // PFM and Data EEPROM Code Protection bit (PFM and Data EEPROM code protection disabled)

#include <xc.h> // must have this
#include "C:\Program Files\Microchip\xc8\v3.00\pic\include\proc\pic18f47k42.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define _XTAL_FREQ  8000000                 // Fosc  frequency for _delay()  library
#define FCY    _XTAL_FREQ/4

#define RS LATD0                   /* PORTD 0 pin is used for Register Select */
#define EN LATD1                   /* PORTD 1 pin is used for Enable */
#define ldata LATB                 /* PORTB is used for transmitting data to LCD */

#define LCD_Port TRISB              
#define LCD_Control TRISD
#define Vref 3.3 // voltage reference 

void LCD_Init();
void LCD_Command(char );
void LCD_Char(char x);
void LCD_Clear();
void LCD_String(const char *);
void LCD_String_xy(char ,char ,const char*);
void MSdelay(unsigned int );
void INTERRUPT_Initialize();
void ADC_Init();


float a = 1 ;
int light_loop = 0 ;
int digital; // holds the digital value 
//float temp; 
char data[10];
//float voltage; // hold the analog value (volt)
int int_loop = 0;

void main(void) {
    // --- Initialize peripherals ---
    LCD_Init();
    INTERRUPT_Initialize();
    IOCCFbits.IOCCF1 = 0;  // Ensure flag starts cleared
    PIR0bits.IOCIF = 0;
    
    // --- Configure TRIS ---
    TRISDbits.TRISD2 = 0;  // Trigger output
    TRISDbits.TRISD3 = 1;  // Echo input

    // --- Timer1 setup ---
    T1CLK = 0x01;              // Clock = Fosc/4
    T1CONbits.CKPS = 0b00;     // 1:1 Prescaler
    T1CONbits.TMR1ON = 0;      // Timer1 off initially

    while (1) {
        // --- Send 10us pulse on Trigger pin ---
        PORTDbits.RD2 = 1;
        __delay_us(10);
        PORTDbits.RD2 = 0;

        // --- Wait for Echo pin to go high ---
        while (!PORTDbits.RD3);  // Wait for echo to start

        // --- Start Timer1 ---
        TMR1H = 0;
        TMR1L = 0;
        T1CONbits.TMR1ON = 1;

        // --- Wait for Echo pin to go low ---
        while (PORTDbits.RD3);  // Measure pulse width

        // --- Stop Timer1 ---
        T1CONbits.TMR1ON = 0;

        // --- Read timer and calculate distance ---
        unsigned int timerVal = (TMR1H << 8) | TMR1L;
        a = timerVal * 0.01715;  // Convert to cm

        // --- Display on LCD ---
        MSdelay(10);
        sprintf(data, "%.2f cm", a);
        LCD_String_xy(1, 0, "Distance:");
        LCD_String_xy(2, 0, "           ");
        LCD_String_xy(2, 0, data);
        MSdelay(200);
        if (a < 10){
                PORTCbits.RC3 = 1;
            }
            PORTCbits.RC3 = 0;
    }
}

//previous kinda not so functional iteration
//void main(void) {
//    LCD_Init(); /* Initialize 16x2 LCD */
//    IOCCFbits.IOCCF1 = 0;  // Ensure flag starts cleared
//    PIR0bits.IOCIF = 0;
//    INTERRUPT_Initialize();
//    IOCCFbits.IOCCF1 = 0;  // Ensure flag starts cleared
//    PIR0bits.IOCIF = 0;
//    
//    
//    //OSCCON=0x72;                   /* Use Internal Oscillator with Frequency 8MHZ */ 
//    ADC_Init();                   
//    PORTD = 0;
//    IOCCFbits.IOCCF1 = 0; // Clear the flag after handling     
//    //T1CON = 0x10;               //Initialize Timer Module
//    TMR1H = 0;                //Sets the Initial Value of Timer
//    TMR1L = 0;                //Sets the Initial Value of Timer
//
//    T1CONbits.CKPS = 0b00;
//    T1CLK = 0;
//    
//    
//    while(1){
//        TMR1H = 0;                //Sets the Initial Value of Timer
//        TMR1L = 0; 
//        ADCON0bits.GO = 1; 
//        while (ADCON0bits.GO){//Wait for conversion done  
//            TMR1H = 0;                //Sets the Initial Value of Timer
//            TMR1L = 0;                //Sets the Initial Value of Timer
//            a = 0;
//            PORTDbits.RD2 = 1;                  //TRIGGER HIGH
//            __delay_us(10);           //10uS Delay 
////            T1CONbits.TMR1ON = 1;
////            T1CLK = 1;
////            T1CONbits.CKPS = 0b00;
//            PORTDbits.RD2 = 0;                  //TRIGGER LOW
//            //__delay_ms(1);
//            while(PORTDbits.RD3 == 0)
//            {             //Waiting for Echo
//               // __delay_us(5);
//                T1CONbits.TMR1ON = 1;
//                T1CLK = 1;
//                T1CONbits.CKPS = 0b00;
//            }
//                       //Timer Starts
//            
//            
//            while(PORTDbits.RD3 == 1)
//            {             //Waiting for Echo goes LOW
//                T1CONbits.TMR1ON = 0;               //Timer Stops
//                T1CLK = 0;
//                T1CONbits.CKPS = 0b00;
//            }
//            
////            while(PORTDbits.RD3 == 0) { // Wait for Echo pulse to go HIGH
////                    T1CONbits.TMR1ON = 1; // Timer starts
////                    }
////
////            while(PORTDbits.RD3 == 1) { // Wait for Echo pulse to go LOW
////                T1CONbits.TMR1ON = 0; // Timer stops
////                    }
//
//            
//            a = (TMR1L | ((unsigned int)TMR1H<<8)); //Reads Timer Value
//            //a = (a * 0.0343)/2;
//            a = (a * 0.01715);//Converts Time to Distance
//            //a = a + 1;                //Distance Calibration 
////            TMR1H = 0;                //Sets the Initial Value of Timer
////            TMR1L = 0;                //Sets the Initial Value of Timer
//            //T1CONbits.CKPS = 0b11;
//           // digital = (ADRESH*256) | (ADRESL);/*Combine 8-bit LSB and 2-bit MSB*/
//            LCD_Clear();
//            MSdelay(10);
//            sprintf(data,"%.2f", a);
//            strcat(data," cm");	/*Concatenate result and unit to print*/
//            LCD_String_xy(1,0,"Distance: ");    /* Display string at location(row,location). */
//                                   /* This function passes string to display */
//            LCD_String_xy(2,0, data);   /*Display string at location(row,location). */
//            MSdelay(50);
////            if (temp > 48.42){
////                MSdelay(100);
////                PORTCbits.RC3 = 1;
////            }
////            PORTCbits.RC3 = 0;
//            
//            
//        }
//    }
//}           

void __interrupt(irq(default),base(0x6008)) ISR(void) {
    if (IOCCFbits.IOCCF2 == 1)
//         Handle the RC1 (IOCC1) interrupt event
        {
        while (int_loop < 5){
        PORTDbits.RD4 = 0;
        PORTDbits.RD5 = 0;
        __delay_ms(500);
        PORTDbits.RD4 = 1;
        PORTDbits.RD5 = 1;
        __delay_ms(500);
        int_loop++;
        }

       
        int_loop = 0;
        PORTDbits.RD4 = 0;
        PORTDbits.RD5 = 0;
        IOCCFbits.IOCCF2 = 0; // Clear the flag after handling
        PIR0bits.IOCIF = 0;
       
    }
}

void INTERRUPT_Initialize (void)
{
    // Enable interrupt priority bit in INTCON0 (check INTCON0 register and find the bit)
    INTCON0bits.IPEN = 1;                                 
    // Enable high priority interrupts using bits in INTCON0
    INTCON0bits.GIEH = 1;                                 
    // Enable low priority interrupts using bits in INTCON0
    INTCON0bits.GIEL = 1;
    INTCON0bits.GIE = 1;
    
    // Interrupt on rising edge of INT0 pin using bits in INTCON0  
    IOCCPbits.IOCCP2 = 1;
    // Set the interrup high priority (IP) 
    IPR0bits.IOCIP = 1;                                  ///// gucciiiiii
    // Enable the interrup (IE) 
    PIE0bits.IOCIE = 1;                                  ///// gucciiiiii
    //Clear interrupt flag for INT0
    PIR0bits.IOCIF = 0;                                  ///// gucciiiiii
    IOCCFbits.IOCCF2 = 0;
    // Change IVTBASE by doign the following
// Set IVTBASEU to 0x00 
    IVTBASEU = 0x00;
// Set IVTBASEH to  0x40; 
    IVTBASEH = 0x60;
// Set IVTBASEL to 0x08; 
    IVTBASEL = 0x08;
}


void ADC_Init(void)
{
    //------Setup the ADC
    //DO: using ADCON0 set right justify
    ADCON0bits.FM = 1;
    //DO: Using ADCON0 set ADCRC Clock
    ADCON0bits.CS = 1;
    //DO: Set RA0 to input
    TRISAbits.TRISA0 = 1;
    //DO: Set RA0 to analog
    ANSELAbits.ANSELA0 = 1;
    //DO: Set RA0 as Analog channel in ADC ADPCH
    ADPCH = 0x00;
    //DO: set ADC CLOCK Selection register to zero
    ADCLK = 0x00;
    //DO: Clear ADC Result registers
    ADRESH = 0;
    ADRESL = 0;
    //DO: set precharge select to 0 in register ADPERL & ADPERH
    ADPREL = 0;
    ADPREH = 0;
    //DO: Set qcquisition LOW and HIGH bytes to zero
    ADACQL = 0;
    ADACQH = 0;
    //DO: Turn ADC On on register ADCON0   
    ADCON0bits.ON = 1;
    
}

void LCD_Init()
{
    MSdelay(15);           /* 15ms,16x2 LCD Power on delay */
    LCD_Port = 0x00;       /* Set PORTB as output PORT for LCD data(D0-D7) pins */
    LATB = 0b00000000;
        ANSELB = 0b00000000; 
        PORTB = 0b00000000;
    LCD_Control = 0b00001000;    /* Set PORTD as output PORT LCD Control(RS,EN) Pins */
    LATD = 0b00000000;
        ANSELD = 0b00000000; 
        PORTD = 0b00000000;
        
           
    
    LATC = 0b00000000;
        ANSELC = 0b00000000; 
        PORTC = 0b00000000;
        TRISC = 0b00000100;
    
        LATA = 0b00000000;
        ANSELA = 0b00000000; 
        PORTA = 0b00000000;
        TRISA = 0b00000000;
        //WPUC = 0b11111111;
    LCD_Command(0x01);     /* clear display screen */
    LCD_Command(0x38);     /* uses 2 line and initialize 5*7 matrix of LCD */
    LCD_Command(0x0c);     /* display on cursor off */
    LCD_Command(0x06);     /* increment cursor (shift cursor to right) */
}

void LCD_Clear()
{
        LCD_Command(0x01); /* clear display screen */
}

void LCD_Command(char cmd )
{
    ldata= cmd;            /* Send data to PORT as a command for LCD */   
    RS = 0;                /* Command Register is selected */
    EN = 1;                /* High-to-Low pulse on Enable pin to latch data */ 
    NOP();
    EN = 0;
    MSdelay(3); 
}

void LCD_Char(char dat)
{
    ldata= dat;            /* Send data to LCD */  
    RS = 1;                /* Data Register is selected */
    EN=1;                  /* High-to-Low pulse on Enable pin to latch data */   
    NOP();
    EN=0;
    MSdelay(1);
}


void LCD_String(const char *msg)
{
    while((*msg)!=0)
    {       
      LCD_Char(*msg);
      msg++;    
        }
}

void LCD_String_xy(char row,char pos,const char *msg)
{
    char location=0;
    if(row<=1)
    {
        location=(0x80) | ((pos) & 0x0f); /*Print message on 1st row and desired location*/
        LCD_Command(location);
    }
    else
    {
        location=(0xC0) | ((pos) & 0x0f); /*Print message on 2nd row and desired location*/
        LCD_Command(location);    
    }  
    LCD_String(msg);

}
/*********************************Delay Function********************************/
void MSdelay(unsigned int val)
{
     unsigned int i,j;
        for(i=0;i<val;i++)
            for(j=0;j<165;j++);      /*This count Provide delay of 1 ms for 8MHz Frequency */
} 
