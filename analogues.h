// DigiFilter 
//
// (c)Ron Taylor, G4GXO 
// 
// File:   analogues.h
//  

#ifndef ANALOGUES_H
#define	ANALOGUES_H

#ifdef	__cplusplus
extern "C" {
#endif


// Defines
   
    
// Declarations


// Prototypes
void init_adc();    
void init_dac();
void interrupt_enable();
void interrupt_disable();

#ifdef	__cplusplus
}
#endif

#endif	/* ANALOGUES_H */

