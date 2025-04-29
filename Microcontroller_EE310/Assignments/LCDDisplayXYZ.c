
//-----------------------------
// Title: DisplayXYZ.c
//-----------------------------
// Purpose: This program is meant to display different information on an LCD using a PIC. This information can 
// vary and can even update in real time by incorporating the ADC. The interrupt shows that this process can be 
//stopped and delayed by a press of a button. 
// Dependencies: stdio.h, stdlib.h, math.h, pic18f47k42.h
// Compiler: XC8 (v3.0)
// Author: Jair Pacheco
// OUTPUTS: LCD(PortD0-7, RD0, RD1), LED(RD3) 
// INPUTS: Interrupt Button (RC2), Potentiometer(LCD[Vo]), Photoresistor1(RA0)
// Versions:
//  	V1.0: 4/25/2025 - First version 
//      V1.1: 4/28/2025 - interrupt and LUX display
//-----------------------------
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
//#include "../../../../../Program Files/Microchip/xc8/v2.40/pic/include/proc/pic18f46k42.h"
//#include "C:\Program Files\Microchip\xc8\v2.40\pic\include\proc\pic18f46k42"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

//#include "../../../../../Program Files/Microchip/xc8/v2.40/pic/include/proc/pic18f46k42.h"
//#include "C:\Program Files\Microchip\xc8\v2.40\pic\include\proc\pic18f46k42"

#define _XTAL_FREQ 4000000                 // Fosc  frequency for _delay()  library
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
void LCD_String(const char *);
void LCD_String_xy(char ,char ,const char*);
void MSdelay(unsigned int );
void INTERRUPT_Initialize();
void ADC_Init();
int light_loop = 0 ;
int digital; // holds the digital value 
float voltage; // hold the analog value (volt))
char data[10];
float E;
/*****************************Main Program*******************************/

void main(void)
{   
    IOCCFbits.IOCCF1 = 0;  // Ensure flag starts cleared
    PIR0bits.IOCIF = 0;
    INTERRUPT_Initialize();
    IOCCFbits.IOCCF1 = 0;  // Ensure flag starts cleared
    PIR0bits.IOCIF = 0;
    
    
    //OSCCON=0x72;                   /* Use Internal Oscillator with Frequency 8MHZ */ 
    ADC_Init();
    LCD_Init();                    /* Initialize 16x2 LCD */
    IOCCFbits.IOCCF1 = 0; // Clear the flag after handling     
    
    while (1) {
        //DO: Set ADCON0 Go to start conversion
        ADCON0bits.GO = 1; 
        while (ADCON0bits.GO){//Wait for conversion done 
        
            digital = (ADRESH*256) | (ADRESL);/*Combine 8-bit LSB and 2-bit MSB*/
        // DO: define voltage = Vref/4096 (note that voltage is float type
            voltage = digital * ((float)Vref/(float)4096) ;
        // DO: Write a code to translate the values from ADRESH:ADRESL register
        //         pair to IO Port. In this case we can connect ADRESL to Port D
        /*This is used to convert integer value to ASCII string*/
            E = 200 * voltage; // This will calculate the light level of the Photoresistor 
        sprintf(data,"%.2f", E);
        strcat(data," LUX");	/*Concatenate result and unit to print*/
            LCD_String_xy(1,0,"Jair Pacheco");    /* Display string at location(row,location). */
                                   /* This function passes string to display */
            LCD_String_xy(2,0, data );   /*Display string at location(row,location). */
            
    }
    }
}


/****************************Functions********************************/




void __interrupt(irq(default),base(0x6008)) ISR(void) {
    if (IOCCFbits.IOCCF2 == 1)
//         Handle the RC1 (IOCC1) interrupt event
        {
        while (light_loop < 10){
        PORTDbits.RD3 = 0;
        __delay_ms(500);
        PORTDbits.RD3 = 1;
        __delay_ms(500);
        light_loop++;
        }
        
        
        light_loop = 0;
        PORTDbits.RD3 = 0;
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
    LCD_Control = 0x00;    /* Set PORTD as output PORT LCD Control(RS,EN) Pins */
        LATC = 0b00000000;
        ANSELC = 0b00000000; 
        PORTC = 0b00000000;
        TRISC = 0b00000100;
    
        LATA = 0b00000000;
        ANSELA = 0b00000001; 
        PORTA = 0b00000000;
        TRISA = 0b00000001;
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
