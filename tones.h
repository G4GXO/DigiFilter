// DigiFilter
//
// (c)Ron Taylor, G4GXO
//
// Tone Keyer Definitions

#ifndef TONES_H
#define	TONES_H

#ifdef	__cplusplus
extern "C" {
#endif
    

// Defines
    
#define F700Hz  ((700UL*65535UL)/FS)    // 700Hz phase increment
#define F800Hz  ((800UL*65535UL)/FS)    // 800Hz phase increment
#define F1000Hz ((1000UL*65535UL)/FS)   // 1000Hz phase increment
#define F1500Hz ((1500UL*65535UL)/FS)   // 1500Hz phase increment

#define T50mS   ((50UL*FS)/1000)        // 50mS tone period
#define T100mS  ((100UL*FS)/1000)       // 100mS tone period
#define T150mS  ((150UL*FS)/1000)       // 150mS tone period
#define T300mS  ((300UL*FS)/1000)       // 300mS tone period

// Raised Cosine Taper used for amplitude shaping of leading and trailing edges of tones
#define Nt  100                 // Sequence length

// Morse character codes (First 1 from left is start marker after which 0=dot, 1=dash)
#define A       0b00000101      // A
#define B       0b00011000      // B
#define D       0b00001100      // D
#define E       0b00000010      // E
#define F       0b00010010      // F
#define G       0b00001110      // G
#define H       0b00010000      // H    
#define K       0b00001101      // K
#define L       0b00010100      // L
#define M       0b00000111      // M
#define P       0b00010110      // P
#define Q       0b00011101      // Q
#define S       0b00001000      // S
#define T       0b00000011      // T
#define U       0b00001001      // U
#define V       0b00010001      // V        
#define ZERO    0b00111111      // 0    
#define ONE     0b00101111      // 1    
#define TWO     0b00100111      // 2
#define SPACE   0b00000001      // SPACE    

// Declarations
extern volatile int tone_shaping;      // Envelope scaling factor
extern int tone_out;
extern int phase_inc;
extern int sin_coeff[6];

// Prototypes
void init_tones();
void tone_keyer(int,int);
void morse_generator(int,char);
extern void tone_gen();                 // Assembler function

#ifdef	__cplusplus
}
#endif

#endif	/* TONES_H */
