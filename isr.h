// DigiFilter 
//
// (c)Ron Taylor, G4GXO 
// 
// File:   isr.h
//  

#ifndef ISR_H
#define	ISR_H

#ifdef	__cplusplus
extern "C" {
#endif

// Defines
volatile extern int sig_pwr;                    // Input signal power

// Prototypes
void __attribute__((interrupt, no_auto_psv))_DAC1RInterrupt(void);  // DAC ISR Prototype    
void write_audio_routing(int);                  // Populate audio path
int read_audio_routing();                       //
void save_audio_routing();                      //
void restore_audio_routing();                   //
void led_is_busy();
void led_is_free();

#ifdef	__cplusplus
}
#endif

#endif	/* ISR_H */

