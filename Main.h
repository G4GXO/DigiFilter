// DigiFilter 
//
// (c)Ron Taylor, G4GXO 
// 
// File:   main.h
//  

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif
    
// Defines

#define FS 11053            // Sampling rate    
    
// I/O Allocations

// PORT A                   DIR
// RA0  NC                  0
// RA1  AF IN               1
// RA2  PRESET 1            1
// RA3  PRESET 2            1
// RA4  PRESET 3            1
//
// PORT B                   DIR
// RB0  PGED1               1
// RB1  PGEC1               1
// RB2  NC                  0
// RB3  NC  BIN_LED         0
// RB4  NC  AUT_LED         0
// RB5  NC  DEN_LED         0
// RB6  ENC CH A            1    
// RB7  ENC CH B            1
// RB8  ENC SW              1
// RB9  LED GREEN           0
// RB10 NC                  0
// RB11 LED RED             0
// RB12 DAC RIGHT O/P +     0    
// RB13 DAC RIGHT O/P -     0
// RB14 DAC LEFT O/P +      0
// RB15 DAC LEFT O/P -      0


// I/O Port directions
#define PORTA_DIR	0x001e	// Port A directions
#define PORTB_DIR	0x01c3	// Port B directions
// Pull Ups on Inputs
#define PULL_UPS1   0x0001  //
#define PULL_UPS2   0x61c0  //
// Indicators
#define GRN_LED     _LATB9  // Tuning direction LED
#define RED_LED     _LATB11 // Signal level LED

// Input Level Warning
#define CLIPPING    0x6000  //5554    // Input clipping threshold at 2/3 peak (0x7FFF)
#define LOW_SIGNAL  0x004f  // Input low signal threshold    
#define ON    1             // Port B direction LED off bit
#define OFF   0             // Port B direction LED on bit

// Test Pins
//#define TEST_PIN_SET    __asm__ volatile ("bset  LATB, #5");
//#define TEST_PIN_CLR    __asm__ volatile ("bclr  LATB, #5");
//#define TEST_PIN_5      _LATB5
//#define TEST_PIN_4      _LATB4
//#define TEST_PIN_3      _LATB3

// Indicator Pins
#define DEN_LED         _LATB5  // Pin to drive Denoiser LED
#define AUT_LED         _LATB4  // Pin to drive Autonotch LED
#define BIN_LED         _LATB3  // Pin to drive Binaural LED
    
// Declarations



// Prototypes    
extern void asm_def();
extern void asm_tables();
extern void A_coeff_build();
extern void B_coeff_build();
extern void spectral_inversion();
extern void af_processing();
extern void tone_gen();

int Main();

#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */    
    