// DigitFilter
//
// (c)Ron Taylor, G4GXO
//
// Setup Routines
//
// These routines read and save settings to flash memory via the EEPROM emulation
// routines. Data is checked after each read to see if it falls within the valid
// range for the parameter. If it does it is used, if not it is replaced by a
// default value.

#include <xc.h>
#include "main.h"
#include "eeprom.h"
#include "settings.h"
#include "gains.h"
#include "analogues.h"
#include "filters.h"
#include "switches.h"
#include "filters.h"
#include "isr.h"

// Saved parameters

//volatile int    audio_routing;      // Audio path selection within ISR
volatile int    routing_temp;       // Store for current ISR audio routing

// Read EEPROM data, unpack to form settings and validate against parameter limits. If
// within range use the read setting, if out of range use the default value. The base
// address is passed from the calling routine, these are defined in the settings h file
// and represent the start address of the block of data relating to the chosen address.
// Each associated parameter is accessed using an address offset.
//
// Data Format
//
// Word 0    Word 1   
//________Freq________
//|        |         | 
//| Freq 1 | Freq 2  |
//
//  FREQ WORDS
//  Frequencies 0x0064..0x0dac, 1 word per parameter
//  AF GAIN WORD
//  Left 0x00nn, right 0xnn00 channel range 0x0000..0x007f

void save_filter(int ee_preset_addr){
    int f1;
    int f2;    
    
    f1=read_freq_1();                                 // Get f1
    f2=read_freq_2();                                 // Get f2  
    DataEEWrite(f1,ee_preset_addr+PS_F1);             // Store F1
    DataEEWrite(f2,ee_preset_addr+PS_F2);             // Store F2    
}

void save_tone_gain(){
    int index;
    // Pack and store AF routing and Tone gain
    index=read_tone_gain();                     // Get current setting  
    index=index&0x00ff;                         // Mask Tone Gain index  
    DataEEWrite(index,EE_COM_1);                // Store Word    
}

// Load common data
void load_tone_gain(){                          // Tone Gain
    int index;
    index=DataEERead(EE_COM_1);                     // Get tone gain index
    if((index>TONE_GAIN_MAX)|(index<TONE_GAIN_MIN)){
        index=TONE_GAIN_DEFAULT;                    // If out of range use default
        save_tone_gain();                           // Update EEPROM
    }
    write_tone_gain(index);                         // Populate variable
}

void load_filter(int ee_preset_addr){
    int f1;
    int f2;
    // Read, unpack and validate stored settings
    f1=DataEERead(ee_preset_addr+PS_F1);            // Read stored Freq 1 setting
    f2=DataEERead(ee_preset_addr+PS_F2);            // Read stored Freq 2 setting
    // Validate and use defaults if out of range
    if((f1<FREQ_1_MIN)||(f1>(f2-BW_MIN))){          // Check f1 data is within range
        f1=FREQ_1_DEFAULT;                          // If out of range load defaults
        f2=FREQ_2_DEFAULT;                          //
        DataEEWrite(f1,ee_preset_addr+PS_F1);       // Store default F1
        DataEEWrite(f2,ee_preset_addr+PS_F2);       // Store default F2    
    }
    if((f2>FREQ_2_MAX)||(f2<f1+BW_MIN)){            // Check f2 data is within range
        f2=FREQ_2_DEFAULT;                          // If out of range load defaults
        f1=FREQ_1_DEFAULT;                          //
        DataEEWrite(f1,ee_preset_addr+PS_F1);       // Store default F1
        DataEEWrite(f2,ee_preset_addr+PS_F2);       // Store default F2        
    }
    write_freq(f1,f2);                              // Write to static variables in filters.c    
}

void load_settings(){
    preset_load();                                  // Read presets
    load_tone_gain();                               // Load tone gain
//    load_bin_bal();                                 // Load binaural balance
}

