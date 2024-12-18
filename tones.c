// DigiFilter
//
// (c)Ron Taylor, G4GXO
//
// File: tones.c
  
#include <xc.h>
#include "Main.h"
#include "tones.h"
#include "delays.h"
#include "filters.h"
#include "isr.h"

extern int          sin_coeff[6];       // Sine coefficient table start
extern volatile int tone_shaping;       // Envelope scaling factor
volatile int ti_max;                    // Max shaping index
extern int phase_inc;                   // Tone phase increment    

//int tone_duration;
//extern int phase_inc;
extern int tone_out;
//volatile extern int AFs_out;
/*
const   int tone_taper[Nt]={ 0x0000,0x000C,0x0033,0x0074,0x00CE,0x0142,0x01D0,0x0276,0x0336,0x040E,
                           0x04FE,0x0606,0x0726,0x085C,0x09AA,0x0B0D,0x0C85,0x0E13,0x0FB5,0x116A,
                           0x1332,0x150C,0x16F8,0x18F4,0x1B00,0x1D1B,0x1F44,0x217A,0x23BD,0x260B,
                           0x2864,0x2AC6,0x2D31,0x2FA3,0x321C,0x349B,0x371E,0x39A5,0x3C2E,0x3EB9,
                           0x4145,0x43D0,0x4659,0x48E0,0x4B63,0x4DE2,0x505B,0x52CD,0x5538,0x579A,
                           0x59F3,0x5C41,0x5E84,0x60BA,0x62E3,0x64FE,0x670A,0x6906,0x6AF2,0x6CCC,
                           0x6E94,0x7049,0x71EB,0x7379,0x74F1,0x7654,0x77A2,0x78D8,0x79F8,0x7B00,
                           0x7BF0,0x7CC8,0x7D88,0x7E2E,0x7EBC,0x7F30,0x7F8A,0x7FCB,0x7FF2,0x7FFF,
                        };
*/

// Riased Cosine factors
const   int tone_taper[Nt]={ 0x0000,0x0008,0x0020,0x004A,0x0083,0x00CD,0x0128,0x0192,0x020D,0x0297,
                            0x0332,0x03DC,0x0495,0x055E,0x0636,0x071D,0x0812,0x0916,0x0A28,0x0B48,
                            0x0C76,0x0DB1,0x0EF9,0x104D,0x11AE,0x131A,0x1493,0x1616,0x17A4,0x193D,
                            0x1AE0,0x1C8C,0x1E41,0x1FFF,0x21C6,0x2394,0x2569,0x2745,0x2928,0x2B10,
                            0x2CFE,0x2EF1,0x30E8,0x32E4,0x34E2,0x36E3,0x38E7,0x3AED,0x3CF3,0x3EFB,
                            0x4103,0x430B,0x4511,0x4717,0x491B,0x4B1C,0x4D1A,0x4F16,0x510D,0x5300,
                            0x54EE,0x56D6,0x58B9,0x5A95,0x5C6A,0x5E38,0x5FFF,0x61BD,0x6372,0x651E,
                            0x66C1,0x685A,0x69E8,0x6B6B,0x6CE4,0x6E50,0x6FB1,0x7105,0x724D,0x7388,
                            0x74B6,0x75D6,0x76E8,0x77EC,0x78E1,0x79C8,0x7AA0,0x7B69,0x7C22,0x7CCC,
                            0x7D67,0x7DF1,0x7E6C,0x7ED6,0x7F31,0x7F7B,0x7FB4,0x7FDE,0x7FF6,0x7FFF,
                        };

// Shaped tone generator employing a raised cosine table to produce a variable duration
// tone at a frequency set by the calling routine. The routine receives the tone phase
// increment (which sets frequency) and keying period from the calling routine. The
// phase increment is passed on to the assembler tone generator, the keying period
// is used with this routine.

void tone_keyer(int tone, int tone_duration){
    volatile int i=0;                   // Index counter, initialise at zero    
    phase_inc=tone;                     // Load tone phase increment
    
    GRN_LED=1;    
    do{                                 // Lead in
    Idle();                             // Wait for interrupt   
    tone_shaping=tone_taper[i];         // Generate entry taper from table
    i++;                                // Increment index
    tone_gen();                         // Generate tone
    }while(i<(Nt));                     // Continue lead in until end of taper table
    do{                                 // Full amplitude tone
    Idle();                             // Wait for interrupt
    tone_gen();                         // Generate tone
    tone_duration--;                    // Decrement duration counter
    }while(tone_duration);              // Loop round whilst non zero
    do{                                 // Lead out  
    Idle();                             // Wait for interrupt
    i--;                                // Decrement index (first dec places index on table end)
    tone_shaping=tone_taper[i];         // Generate exit taper from table    
    tone_gen();                         // Generate tone    
    }while(i);                          // Exit once taper complete
    GRN_LED=0;
}
// Convert 8 bit character codes into keying sequences. The beginning of the
// character is marked by a one after which the dot and dash elements are
// described by zeros and ones. The first task is to locate the marker bit
// which indicates where the code elements start. After this each bit is
// inspected and converted into dot or dash keying sequences.
void morse_generator(int tone, char character){
    led_is_busy();                      // Claim LED
    int tester=128;                     // Bit test pattern
    while(!(tester&character)){         // Find position of marker bit (first 1)
    tester=tester>>1;                   // Move test pattern right one bit
    }                                   // Continue until a 1 is found
                                        // We exit with tester on the marker bit,
    tester=tester>>1;                   // Move on one bit to align with first element
    do{                                 // Work through character elements
    if(character&tester){               // If element is a 1, send a dash
        tone_keyer(tone,T150mS);        // Dash freq and duration
    }
    else{                               // Character is a zero, send a dot
        tone_keyer(tone,T50mS);         // Dot freq and duration
    }
    tester=tester>>1;                   // Next bit
    delay_ms(50);                       // Inter element delay
    }while(tester);                     // Continue until test pattern shifted to zero
    delay_ms(100);                      // 3 element inter character delay (50mSec + 100mSec)
    led_is_free();                      // Declare LED as available
}

// Values for sin polynomial calculation (EMRFD Ch10.9)
// Dummy value used for last coefficient to fit with computation loop
const int sin_coefficients[6]={0x3240, 0x0053, 0xAACC, 0x08B7, 0x1CCE, 0x0000};

void init_tones(){              
    int i;
    for(i=0;i<6;i++){           // Load coefficient table
        sin_coeff[i]=sin_coefficients[i];
    }
    ti_max=Nt-1;                // Initialise max shaping index value
}

