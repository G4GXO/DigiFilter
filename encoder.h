// DigiFilter 
//
// (c)Ron Taylor, G4GXO  
// 
// File:   encoder.h
//  

#ifndef ENCODER_H
#define	ENCODER_H

#ifdef	__cplusplus
extern "C" {
#endif
//
// Encoder Definitions
//
// Port B
//
// RB6  ENC A CH A
// RB7  ENC A CH B
// RB8  ENC_SW
//
// Defines
#define ENC_A_PORT      PORTB                   // Encoder port
#define ENC_A_MASK      0x00c0                  // Encoder channel mask
#define ENC_A_SW        PORTBbits.RB8           // Encoder A switch

// Encoder State Channel Mask and Switch Counts
#define CH_MASK         0x0003      // Historic channel bit mask
#define HOLD_COUNT      10000       // Count for "press and hold" event
#define LONG_COUNT      25000       // Count for "press and hold long" event       
#define VLONG_COUNT     50000       // Very long hold count

// Encoder Flag Bit Masks
#define ENC_UP          0x0001
#define ENC_DOWN        0x0002
#define ENC_PRESS       0x0004
#define ENC_REL         0x0008
#define ENC_HOLD        0x0010

// Encoder Step rate
#define ENC_FAST        1           // Select number of case values
#define ENC_SLOW        0           // to set different step rates
    
// Declarations

// Prototypes    
int enc_A();
void enc_A_fast();
void enc_A_slow();
void reset_enc_A(void);
void init_enc_A();
 
#ifdef	__cplusplus
}
#endif

#endif	/* ENCODER_H */
