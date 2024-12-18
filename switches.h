// DigiFilter
//
// (c)Ron Taylor, G4GXO
//
// Switches Header

#ifndef SWITCHES_H
#define	SWITCHES_H

#ifdef	__cplusplus
extern "C" {
#endif

// Defines
#define SWITCH_PORT     PORTA   // Presets switch port    
#define SWITCH_MASK     0x001C  // Mask for switch inputs
#define PRESET_0        0x001C  // All high
#define PRESET_1        0x000C  // Bit patterns for each pin (active low)
#define PRESET_2        0x0014  //
#define PRESET_3        0x0018  //
#define PRESET_4        0x0004  // Pins 1 & 2 low
#define PRESET_5        0x0010  // Pins 3 & 2 low
#define PRESET_6        0x0008  // Pins 3 & 1 low
#define PRESET_7        0x0000  // All pins low   
#define CHANGE          0x8000  // Change flag, 1=presets changed    

// Declarations


// Prototypes    
int switches();
int get_preset_addr();
int presets_read(); 
void preset_load();
void preset_save();
int init_presets();
    
#ifdef	__cplusplus
}
#endif

#endif	/* SETTINGS_H */        
    
    