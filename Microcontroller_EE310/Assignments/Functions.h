/* 
 * File:   Functions.h
 * Author: Jair Pacheco
 */
#ifndef FUNCTIONS_H
#define	FUNCTIONS_H

#ifdef	__cplusplus
extern "C" {
#endif

void PORTS_Initialization (void){
    //set port B and D as outputs 
        LATB = 0b00000000;
        ANSELB = 0b00000000; 
        PORTB = 0b00000000;
        TRISB = 0b10000111;
        
        LATD = 0b00000000;
        ANSELD = 0b00000000; 
        PORTD = 0b00000000;
        TRISD = 0b00000000;//sets PORTD as all outputs
}



void INTERRUPT_Initialize (void)
{
    // Enable interrupt priority bit in INTCON0 (check INTCON0 register and find the bit)
    INTCON0bits.IPEN = 1;
    // Enable high priority interrupts using bits in INTCON0
     INTCON0bits.GIEH = 1;
    // Enable low priority interrupts using bits in INTCON0
    INTCON0bits.GIEL = 1;
    // Interrupt on rising edge of INT0 pin using bits in INTCON0
    INTCON0bits.INT0EDG = 1;
    // Set the interrup high priority (IP) for INT0 - INT0IP
    IPR1bits.INT0IP = 1;
    // Enable the interrup (IE) for INT0
    PIE1bits.INT0IE = 1;
    //Clear interrupt flag for INT0
    PIR1bits.INT0IF = 0;
    // Change IVTBASE by doign the following
// Set IVTBASEU to 0x00 
    IVTBASEU = 0x00;
// Set IVTBASEH to  0x40; 
    IVTBASEH = 0x40;
// Set IVTBASEL to 0x08; 
    IVTBASEL = 0x08;
}


#ifdef	__cplusplus
}
#endif

#endif	/* FUNCTIONS_H */

