//-----------------------------
// Title: Tempchange
//-----------------------------
// Purpose: This program turns on appropriate system to make a measured temprature into the desired temprature,
// saving these values in decimal 
// Dependencies: NONE
// Compiler: pic-as (v3.0)
// Author: Jair Pacheco
// OUTPUTS: Port D, REG20-25, REG60-62, REG70-72
// INPUTS: Meas_temp, Ref_temp 
// Versions:
//  	V1.0: 3/9/2025 - First version 
//  	V1.2: 3/11/2025 - Added hex to decimal Conversion
//-----------------------------
// Initalization 
;#include "./AssemblyConfig.inc"
;#include "C:\Users\student\Documents\myMPLABXProjects\ProjectFirstAssemblyMPLAB\FirstAssemblyMPLAB.X\AssemblyConfig.inc"
#include <xc.inc>
;----------------
; PROGRAM INPUTS
;----------------
;The DEFINE directive is used to create macros or symbolic names for values.
;It is more flexible and can be used to define complex expressions or sequences of instructions.
;It is processed by the preprocessor before the assembly begins.

#define  measuredTempInput 	5 ; this is the input value
#define  refTempInput 		15 ; this is the input value

;---------------------
; Definitions
;---------------------
#define SWITCH    LATD,2  
#define LED0      PORTD,0
#define LED1	  PORTD,1

;---------------------
; Program Constants
;---------------------
; The EQU (Equals) directive is used to assign a constant value to a symbolic name or label.
; It is simpler and is typically used for straightforward assignments.
;It directly substitutes the defined value into the code during the assembly process.
	
REG80	EQU	0x80 ; REF Lower Bound
REG81	EQU	0x81 ; REF higher Bound 
REG90	EQU	0x90 ; Meas. Lower Bound
REG91	EQU	0x91 ; Meas. Higher Bound 

NUMERATOR EQU 0x30 ;NUMERATOR
QUOTIENT EQU 0x40 ;QUOTIENT

NUMERATOR2 EQU 0x31 ;NUMERATOR
QUOTIENT2 EQU 0x41 ;QUOTIENT

REG10   EQU     0x10   // in HEX, MY DENOMINATOR
REG11   EQU     0x11
REG01   EQU     0x01

REG20	EQU     0x20 ;RefTemp
REG21	EQU     0x21 ;Measured Temp
REG22	EQU     0x22 ;ContREG

REG60	EQU     0x60 ; LOW DEC REFtemp
REG61	EQU     0x61 ; MID DEC REFtemp
REG62	EQU     0x62 ; HIGH DEC REFtemp

REG70	EQU     0x70 ; LOW DEC MEAStemp
REG71	EQU     0x71 ; MID DEC MEAStemp
REG72	EQU     0x72 ; HIGH DEC MEAStemp


;---------------------
; Main Program
;---------------------
PSECT absdata,abs,ovrld        ; Do not change
   
	ORG	00		;Reset vector
	GOTO	measuredDECIMAL

	ORG	0020H		;Begin assembly at 0020H

measuredDECIMAL:
    NUMERATOR EQU 0x30 
    QUOTIENT EQU 0x40 
    REG70 EQU 0x70 ;measuredTempLOW
    REG71 EQU 0x71 ;measuredTempMID
    REG72 EQU 0x72 ;measuredTempHIGH
    DENOMINATOR EQU 10 ;measuredTempREG/10
    MOVLW   measuredTempInput ;number we are converting
    MOVWF   NUMERATOR 
    GOTO    measuredDECIMALCONT

ItIsNeg:
    NUMERATOR EQU 0x30 
    NEGF    NUMERATOR
    GOTO    measuredDECIMALCONT2
    
measuredDECIMALCONT:    
    NUMERATOR EQU 0x30 
    QUOTIENT EQU 0x40 
    REG70 EQU 0x70 ;measuredTempLOW
    REG71 EQU 0x71 ;measuredTempMID
    REG72 EQU 0x72 ;measuredTempHIGH
    DENOMINATOR EQU 10 ;measuredTempREG/10
    CLRF	STATUS
    MOVLW	0x0 
    MOVF	REG21,0
    BN		ItIsNeg
measuredDECIMALCONT2:    
    NUMERATOR EQU 0x30 
    QUOTIENT EQU 0x40 
    REG70 EQU 0x70 ;measuredTempLOW
    REG71 EQU 0x71 ;measuredTempMID
    REG72 EQU 0x72 ;measuredTempHIGH
    MOVLW   DENOMINATOR 
    CLRF    QUOTIENT 
D1:
    INCF    QUOTIENT ;increment quotient for every sub
    SUBWF   NUMERATOR ;sub WREG from NUMERATOR 
    BC    D1 ;branch if carry, if positive go back
    ADDWF   NUMERATOR ;this is our first digit
    DECF    QUOTIENT ;once too many for quotient
    MOVFF   NUMERATOR,REG70 ;save the first digit
    MOVFF   QUOTIENT,NUMERATOR ;repeat the process 
    CLRF    QUOTIENT 
D2:
    INCF    QUOTIENT
    SUBWF   NUMERATOR ;subtract WREG from NUMERATOR
    BC    D2 ;branch if carry --> C = 1 for positive
    ADDWF   NUMERATOR ;once too many
    DECF    QUOTIENT
    MOVFF   NUMERATOR,REG71 ;2nd digit
    MOVFF   QUOTIENT, REG72 ;3rd digit
    GOTO    refDECIMAL
   
refDECIMAL:
    NUMERATOR2 EQU 0x31 
    QUOTIENT2 EQU 0x41 
    REG60 EQU 0x60 ;refTempDecimalLOW
    REG61 EQU 0x61 ;refTempDecimalMID
    REG62 EQU 0x62 ;refTempDecimalHIGH
    DEN2 EQU 10 ;refTempREG/10
    MOVLW   refTempInput 
    MOVWF   NUMERATOR2 
    MOVLW   DEN2 
    CLRF    QUOTIENT2 
D11:
    INCF    QUOTIENT2 ;increment quotient for every sub
    SUBWF   NUMERATOR2 ;sub WREG from NUMERATOR2 
    BC    D11 ;branch if carry, if positive go back
    ADDWF   NUMERATOR2 ;this is our first digit
    DECF    QUOTIENT2 ;once too many for quotient
    MOVFF   NUMERATOR2,REG60 ;save the first digit
    MOVFF   QUOTIENT2,NUMERATOR2 ;repeat the process
    CLRF    QUOTIENT2 
D22:
    INCF    QUOTIENT2
    SUBWF   NUMERATOR2 ;subtract WREG from NUMERATOR2
    BC    D22 ;branch if carry 
    ADDWF   NUMERATOR2 ;once too many
    DECF    QUOTIENT2
    MOVFF   NUMERATOR2,REG61 ;2nd digit
    MOVFF   QUOTIENT2, REG62 ;3rd digit
    
START:	
    MOVLW   0x00
    MOVWF   TRISD,0 ; Making PORTD an output 
    
    MOVLW   measuredTempInput ; MEAS. TEMP VALUE IN REG21
    MOVWF   REG21
    
    MOVLW   refTempInput ; REF TEMP VALUE IN REG20
    MOVWF   REG20
     
    MOVLW   9 
    MOVWF   REG80 ; setting REF lower bound 
    MOVLW   11
    MOVWF   REG90 ; Meas. Lower bound
    MOVLW   51
    MOVWF   REG81 ; REF higher bound
    MOVLW   61
    MOVWF   REG91 ; Meas. Higher bound
;checking Meas. Range
    MOVLW   0x00
    MOVWF   REG01 
    MOVLW   measuredTempInput 
    CPFSEQ  REG01
    GOTO    START2 ;if not equal to zero investigate further
    GOTO    START3 ; If it is zero move along

ItIsNegative:
    NEGF    WREG
    CPFSGT  REG90  ; F > W
    GOTO    TERM
    GOTO    START3

START2:    
;Looking closer at Meas. Range    
    CLRF	STATUS
    MOVLW	0x0 
    MOVF	REG21,0
    BN		ItIsNegative
    CPFSGT  REG91 ; F> W
    GOTO    TERM
    GOTO    START3
START3:    
;Checking REF Range
    MOVLW   refTempInput
    CPFSLT  REG80 ; 9 < REF
    GOTO    TERM
    CPFSGT  REG81 ; 51 > REF
    GOTO    TERM
    GOTO    CHECK1
    
CHECK1: ;Check if MEAS. and REF are equal
    MOVLW   measuredTempInput
    MOVWF   REG01                 
    MOVLW   refTempInput                   
    CPFSEQ  REG01 ; skip if F = W
    GOTO    CHECK2
    GOTO    FINISH0 

ItIsNegative2: ; Special Case for when MEAS. is less than 0 
    NEGF    REG01
    CPFSLT  REG01 ; skip if F < W
    GOTO    FINISH2
    GOTO    FINISH1
    
CHECK2:   ; Checking if MEAS. < REF
    CLRF	STATUS
    MOVLW	0x00 
    MOVF	REG21,0
    BN		ItIsNegative2
    MOVLW   refTempInput
    CPFSLT  REG01 ; skip if F < W
    GOTO    FINISH2
    GOTO    FINISH1

  
FINISH0: ;Nothing Happens
    MOVLW   0x00
    MOVWF   REG22,0
    MOVFF   REG22,PORTD
    GOTO    TERM
    
FINISH1: ;Heater Turns on 
    MOVLW   0x01
    MOVWF   REG22, 0
    MOVFF   REG22,PORTD
    GOTO    TERM
    
FINISH2: ; Cooler turns on 
    MOVLW   0x02
    MOVWF   REG22,0
    MOVFF   REG22,PORTD
    GOTO    TERM
    
TERM: 
    END
