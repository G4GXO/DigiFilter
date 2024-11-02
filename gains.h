// DigitFilter 
//
// (c)Ron Taylor, G4GXO 
// 
// File:   gains.h
//  

#ifndef GAINS_H
#define	GAINS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
// Defines    
#define TONE_GAIN_MAX           43
#define TONE_GAIN_MIN           10
#define TONE_GAIN_DEFAULT       20
#define BIN_GAIN_MIN            28
#define BIN_GAIN_MAX            43
#define BIN_BAL_MAX             15          // Max positive balance 
#define BIN_BAL_MIN             -15         // Max negative balance
    
// Declarations
extern int tone_gain_factor;    
extern int left_gain_factor;
extern int right_gain_factor;

// Prototypes
//void tone_gain_init();
void init_gains();
void write_tone_gain(int);
int read_tone_gain();
void tone_gain_load();
int tone_inc_gain();
int tone_dec_gain();
//void bin_bal_init(int,int);
int bin_bal_right();
int bin_bal_left();
void bin_gains_load();
void bin_gains_init();
int read_bin_bal();
void write_bin_bal(int);

#ifdef	__cplusplus
}
#endif

#endif	/* GAINS_H */


