// DigiFilter 
//
// (c)Ron Taylor, G4GXO 
// 
// File:   delays.h
//  

#ifndef DELAYS_H
#define	DELAYS_H

#ifdef	__cplusplus
extern "C" {
#endif

// Defines
#define Fosc    80000000UL          // Oscillator frequency Hz   
#define FCY     Fosc/2              // Instruction clock in Hz

// Prototypes
void delay_us(unsigned int);
void delay_ms(unsigned int);
void debounce();

#ifdef	__cplusplus
}
#endif

#endif	/* DELAYS_H */

