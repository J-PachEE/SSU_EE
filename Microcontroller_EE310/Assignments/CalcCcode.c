//-----------------------------
// Title: CalcCcode.c
//-----------------------------
// Purpose: This program calculates basic operations and display the result on 8 LEDS in binary
// It receives its input values from a 4x4 (extended) keypad. The program sends a high value through 
// one of the output bits in Port B and checks if it received anything from the input bits of Port B
// if nothing was received then it checks the next column, if it is detected it translates it to
// that specific key 
// Dependencies: Header.h, stdio.h, stdlib.h, math.h, pic18f47k42.h
// Compiler: XC8 (v3.0)
// Author: Jair Pacheco
// OUTPUTS: Port D(connected to 8 LEDS), Port B [0-3](For keypad input testing), 8 LEDS
// INPUTS: Port B[4-7](For keypad input testing), KEYPAD
// Versions:
//  	V1.0: 4/2/2025 - First version 
//      V1.1: 4/5/2025 - adjusting keypad function to better allow loops
//  	V1.2: 4/5/2025 - adjusting and polishing loops
//      V1.3: 4/6/2025 - Adjusting clear () function
//-----------------------------
// Initalization 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xc.h> // must have this 
#include "C:\Program Files\Microchip\xc8\v3.00\pic\include\proc\pic18f47k42.h"
#include "Header.h"

#define _XTAL_FREQ 4000000                 // Fosc  frequency for _delay()  library
#define FCY    _XTAL_FREQ/4


char Display_Result = 0; // holds result  of operation that will be displayed 
char X_Input = 0; // Our first full number put into the calculator 
char Y_Input = 0; // Our second full number put into the calculator
char X_keypress = 0; // holds the value of the most recently pressed value for input X
char Y_keypress = 0; // holds the value of the most recently pressed value for input y
int Digit_Num = 0; // number of digits our input X/input Y has received 
int Operation = 0; // holds the value of what operation was selected
int op_num = 0; 
char test = 0; //
int Final_Check = 0; //


void init_keypad(){
    LATD = 0b00000000;
    ANSELD = 0b00000000;
    TRISD = 0b00000000; // sets PORTD as all outputs
    PORTD = 0b00000000; // turns off PORTD outputs
   
    LATB = 0b00000000;
    ANSELB = 0b00000000;
    PORTB = 0b00000000;
    TRISB = 0b11110000; // sets PORTB as inputs for rows and outputs for columns
    
    Digit_Num = 0;
    X_Input = 0;
    X_keypress = 0;
    Y_keypress = 0;
    Y_Input = 0;
    op_num = 0;
    Final_Check = 0;
    Operation = 0;
}


char get_keypad_input(){

    PORTBbits.RB3 = 1;  // Checking far left column
        if (PORTBbits.RB7 == 1)
            return Final_Check = Final_Check +1, '1';
        if (PORTBbits.RB6 == 1)
            return Final_Check = Final_Check +1, '4';
        if (PORTBbits.RB5 == 1)
            return Final_Check = Final_Check +1, '7';
        if (PORTBbits.RB4 == 1) 
            return  '*';
    PORTBbits.RB3 = 0;
     
    PORTBbits.RB2 = 1; // checking middle left column 
        if (PORTBbits.RB7 == 1)
            return Final_Check = Final_Check +1, '2';
        if (PORTBbits.RB6 == 1)
            return Final_Check = Final_Check +1, '5';
        if (PORTBbits.RB5 == 1)
            return Final_Check = Final_Check +1, '8';
        if (PORTBbits.RB4 == 1)
            return Final_Check = Final_Check +1, '0';
    PORTBbits.RB2 = 0;
     
    PORTBbits.RB1 = 1; // Checking middle right column 
        if (PORTBbits.RB7 == 1)
            return Final_Check = Final_Check +1, '3';
        if (PORTBbits.RB6 == 1)
            return Final_Check = Final_Check +1, '6';
        if (PORTBbits.RB5 == 1)
            return Final_Check = Final_Check +1, '9';
        if (PORTBbits.RB4 == 1)  
            return Final_Check = Final_Check +1, '#';
    PORTBbits.RB1 = 0;
}


char get_keypad_OP() {
    
    PORTBbits.RB3 = 1;  // Checking far left column
        if (PORTBbits.RB4 == 1) 
            return '*';
    PORTBbits.RB3 = 0;
    
    PORTBbits.RB0 = 1;  // Checking far right column (Reserved for operations)
        if (PORTBbits.RB7 == 1) 
            return op_num = op_num + 1, 'A'; 
        if (PORTBbits.RB6 == 1)
            return op_num = op_num + 1, 'B'; 
        if (PORTBbits.RB5 == 1) 
            return op_num = op_num + 1, 'C'; 
        if (PORTBbits.RB4 == 1) 
            return op_num = op_num + 1, 'D'; 
    PORTBbits.RB0 = 0;
    }


char math(char Operation){
        if (Operation == 'A')
        return (X_Input + Y_Input);
        if (Operation == 'B')
        return (X_Input - Y_Input);
        if (Operation == 'C')
        return (X_Input * Y_Input);
        if (Operation == 'D')
        return (X_Input / Y_Input);
}


void clear() {
        PORTD = 0xFF;
        __delay_ms(500);
        PORTD = 0;
        PORTB = 0;
        Digit_Num = 0;
        X_Input = 0;
        X_keypress = 0;
        Y_keypress = 0;
        Y_Input = 0;
        Final_Check = 0;
        op_num = 0;
        Operation = 0;
}
// --------------------------MAIN CODE STARTS BELOW----------------------------------------

void main(void) {
    init_keypad();
start:    
    clear();
    while(1) {
        
        // Get first full number
        while (Digit_Num < 2) {
            X_keypress = get_keypad_input();
            restart:
            if (X_keypress == '*'){
                goto start;
            }
            if (X_keypress != '\0' && X_keypress >= '0' && X_keypress <= '9') {
                X_Input = X_Input * 10 + (X_keypress - '0');
                PORTD = 1;
                __delay_ms(750);
                PORTD = 0;
                Digit_Num++;
            }
        }
        
        // Reset for second number
        Y_Input = 0;
        PORTD = 1; // visual feedback, shows 1st number was received
        Digit_Num = 0;
        __delay_ms(750);
        op_num = 0;
        
        //ensuring operation is received
        while (op_num < 1) {
            Operation = get_keypad_OP(); // Get arithmetic operation
            if (Operation == '*'){
                goto start;
            }
        }
        
        PORTD = 3; // visible feedback saying OP was received
        __delay_ms(750);
        
        // Getting second number 
        while (Digit_Num < 2 ) {
            Y_keypress = get_keypad_input();
            if (Y_keypress == '*'){
                goto start;
            }
            if (Y_keypress != '\0' && Y_keypress >= '0' && Y_keypress <= '9') {
                Y_Input = Y_Input * 10 + (Y_keypress - '0');
                PORTD = 2;
                __delay_ms(750);
                PORTD = 0;
                Digit_Num++;
            }
        }
        
        PORTD = 2; // visual feedback, this shows we got second number
        Digit_Num = 0;
        __delay_ms(750);
        Final_Check = 0; // final check must be reset for loop to work properly
        
        // Show result or start over
        while (Final_Check < 2 ){
            test = get_keypad_input();
            if (test == '*'){
                goto start;
            }
     
            else if (test == '#') {
                Display_Result = math(Operation);
                PORTD = Display_Result;
                __delay_ms(750);
            }
            
        }
                
        if (test == '#') {
            goto start;
        }
        
        PORTD = 0;
                PORTB = 0;
                Digit_Num = 0;
                X_Input = 0;
                X_keypress = test;
                Y_keypress = 0;
                Y_Input = 0;
                Final_Check = 0;
                op_num = 0;
                Operation = 0;
                goto restart;
                
   }
}
