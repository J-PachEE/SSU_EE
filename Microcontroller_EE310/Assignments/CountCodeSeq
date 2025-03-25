//-----------------------------
// Title: CountCode
//-----------------------------
// Purpose: This program is used to count in hexadecimal, using a 7 segment display, to show the number
// It will also be able to count cown an restart from zero. 
// Dependencies: AssemblyConfig.inc
// Compiler: pic-as (v3.0)
// Author: Jair Pacheco
// OUTPUTS: Port D
// INPUTS: Port B
// Versions:
//  	V1.0: 3/24/2025 - First version 
//  	V1.2: 3/24/2025 - Replacing instances of using register 60, 61,62
//	V1.3: 3/24/2025 - Replacing instances of using register 95
//-----------------------------
// Initalization 

#include "./AssemblyConfig.inc"
#include <xc.inc>

;----------------
; PROGRAM INPUTS
;----------------
;The DEFINE directive is used to create macros or symbolic names for values.
;It is more flexible and can be used to define complex expressions or sequences of instructions.
;It is processed by the preprocessor before the assembly begins.

#define  ButtonA 	PORTB,0 ; this is the input value
#define  ButtonB 	PORTB,1 ; this is the input value

;---------------------
; Definitions
;---------------------
;#define SWITCH    LATD,2  
#define LEDa      PORTD,0
#define LEDb	  PORTD,1
#define LEDc      PORTD,2
#define LEDd	  PORTD,3
#define LEDe      PORTD,4
#define LEDf	  PORTD,5
#define LEDg      PORTD,6
#define LEDh	  PORTD,7

;---------------------
; Program Constants
;---------------------
; The EQU (Equals) directive is used to assign a constant value to a symbolic name or label.
; It is simpler and is typically used for straightforward assignments.
;It directly substitutes the defined value into the code during the assembly process.

Inner_loop  equ 0xFF;180 // in decimal
Outer_loop  equ 0xFF;190	

REG10   EQU     0x10   
REG11   EQU     0x11
REG01   EQU     0x01 ; PLACE HOLDER FOR DEC

REG20	EQU     0x20 ;
REG21	EQU     0x21 ;
REG22	EQU     0x22 ;

REG40	EQU     0x40 ; Current INPUT
REG41	EQU     0x41 ; BUTTON A
REG42	EQU     0x42 ; BUTTON B
REG45	EQU	0x45 ; 
	
REG60	EQU     0x60 ; DO NOT USE 60!!!!!!!!!!!!!
REG61	EQU     0x61 ; BUTTON A
REG62	EQU     0x62 ; BUTTON B

REG70	EQU     0x70 ; 
REG71	EQU     0x71 ; 
REG72	EQU     0x72 ;

REG7F	EQU	0x7F ; LOWER BOUND FOR 7-SEG TABLE

REG80	EQU	0x80 ; holds 7-Seg. value for 0
REG81	EQU	0x81 ; holds 7-Seg. value for 1
REG82	EQU	0x82 ; holds 7-Seg. value for 2
REG83	EQU	0x83 ; holds 7-Seg. value for 3
REG84	EQU	0x84 ; holds 7-Seg. value for 4
REG85	EQU	0x85 ; holds 7-Seg. value for 5
REG86	EQU	0x86 ; holds 7-Seg. value for 6
REG87	EQU	0x87 ; holds 7-Seg. value for 7
REG88	EQU	0x88 ; holds 7-Seg. value for 8
REG89	EQU	0x89 ; holds 7-Seg. value for 9
REG8A	EQU	0x8A ; holds 7-Seg. value for A
REG8B	EQU	0x8B ; holds 7-Seg. value for B
REG8C	EQU	0x8C ; holds 7-Seg. value for C
REG8D	EQU	0x8D ; holds 7-Seg. value for D
REG8E	EQU	0x8E ; holds 7-Seg. value for E
REG8F	EQU	0x8F ; holds 7-Seg. value for F
	
REG90	EQU	0x90 ; UPPER BOUND FOR 7-SEG TABLE
REG91	EQU	0x91 ; 
REG95	EQU	0x95 ; HOLDS NUMBER OF INPUTS ON



;---------------------
; Main Program
;---------------------
PSECT absdata,abs,ovrld        ; Do not change
   
	ORG	00		;Reset vector
	GOTO	_Initialize

	ORG	0020H		;Begin assembly at 0020H

loopDelay: 
    MOVLW       Inner_loop
    MOVWF       REG10
    MOVLW       Outer_loop
    MOVWF       REG11
_loop1:
    DECF        REG10,1
    BNZ         _loop1
    MOVLW       Inner_loop ; Re-initialize the inner loop for when the outer loop decrements.
    MOVWF       REG10
    DECF        REG11,1 // outer loop
    BNZ        _loop1
    RETURN	

_Initialize: 
; Set up REG with values for 7-SEG    
    MOVLW   0x11
    MOVWF   REG7F 
    MOVLW   0x3F
    MOVWF   REG80
    MOVLW   0x06
    MOVWF   REG81
    MOVLW   0x5B
    MOVWF   REG82
    MOVLW   0x4F
    MOVWF   REG83
    MOVLW   0x66
    MOVWF   REG84
    MOVLW   0x6D
    MOVWF   REG85
    MOVLW   0x7D
    MOVWF   REG86
    MOVLW   0x07
    MOVWF   REG87
    MOVLW   0x7F
    MOVWF   REG88
    MOVLW   0x67
    MOVWF   REG89
    MOVLW   0x77
    MOVWF   REG8A
    MOVLW   0x7C
    MOVWF   REG8B
    MOVLW   0x39
    MOVWF   REG8C
    MOVLW   0x5E
    MOVWF   REG8D
    MOVLW   0x79
    MOVWF   REG8E
    MOVLW   0x71
    MOVWF   REG8F
    SETF    REG90
    //Setting up Port D
    BANKSEL	PORTD ;
    CLRF	PORTD ;Init PORTD 
    BANKSEL	LATD ;Data Latch
    CLRF	LATD ;
    BANKSEL	ANSELD ;
    CLRF	ANSELD ;digital I/O
    BANKSEL	TRISD ;
    MOVLW	0b00000000 ;Set RD[7:1] as outputs 
    MOVWF	TRISD ;and set RD0 as ouput
    //Setting up B
    BANKSEL	PORTB ;
    CLRF	PORTB ;Init PORTB
    BANKSEL	LATB ;Data Latch
    CLRF	LATB ;
    BANKSEL	ANSELB ;
    CLRF	ANSELB ; digital I/O
    BANKSEL	TRISB ; 
    MOVLW	0b00000011 ;Set RB[2:0] as inputs
    MOVWF	TRISB ;and set RD0 as ouput
    
    ; RRRRRRRRRRRRRRRRRRRRAAAAAAAAAAAAAAAAAAAAAAAAAHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
   ; MOVLW	0x02 ; REMOVE WHEN UPLOADING!!!!!!!!!!!!!!!!!!!!
   ; MOVWF	PORTB ; THIS TOO
    
_PointInitial:
    ;Setting up pointer 
    CLRF	FSR0H
    MOVLW	0x80
    MOVWF	FSR0L
    MOVFF	INDF0,PORTD

_Main:
    CLRF    STATUS
    CLRF    REG45
    MOVFF   PORTB,WREG
    MOVWF   REG40 
    ;Check if Button A is ON or OFF
    BTFSC   REG40,0
    CALL    SET_BIT0
    BTFSS   REG40,0
    CALL    CLEAR_BIT0
    ;Check if Button B is ON or OFF
    BTFSC   REG40,1
    CALL    SET_BIT1
    BTFSS   REG40,1
    CALL    CLEAR_BIT1
    CALL    loopDelay
    CALL    loopDelay
    ;CHECK IF # of ON BUTTON is more than zero
    MOVLW   0x00
    CPFSGT  REG45 ; SKIP Next line if you have atleast 1 button on
    GOTO    _Main
    ;CHECK IF # of ON buttons is Equal to 1 
    MOVLW   0x01
    CPFSEQ REG45
    GOTO    _PointInitial
    ; Since one is on, Check which one
    CPFSEQ  REG41 ;CHeck if button A is on 
    GOTO    _DecrementCount
    GOTO    _IncrementCount
    
    
_DecrementCount:
    MOVFF   POSTDEC0,REG01
    MOVLW   0x11
    CPFSEQ  INDF0
    MOVFF   INDF0,PORTD
    CPFSEQ  INDF0
    GOTO    _Main
    MOVLW   0x8F
    MOVWF   FSR0L
    MOVFF   INDF0,PORTD
    GOTO    _Main
    
_IncrementCount:    
    SETF    WREG
    CPFSEQ  PREINC0
    MOVFF   INDF0,PORTD
    CPFSEQ  INDF0
    GOTO    _Main
    GOTO    _PointInitial

    
; SUBROUTINES
    
SET_BIT0:
    MOVLW   0x01
    MOVWF   REG41
    INCF    REG45
    Return
    
CLEAR_BIT0:
    MOVLW   0x00
    MOVWF   REG41
    Return
    
SET_BIT1:
    MOVLW   0x01
    MOVWF   REG42
    INCF    REG45
    Return

CLEAR_BIT1:
    MOVLW   0x00
    MOVWF   REG42
    Return
    
    
    
    
    
    
    
    
    
    
END

