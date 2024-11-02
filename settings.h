// DigiFilter
//
// (c)Ron Taylor, G4GXO
//
// Settings Header File
//
// Parameter default values for flash store settings are defined here. If a
// value read from flash does not fall within the corresponding limits
// then the default value is used.

#ifndef SETTINGS_H
#define	SETTINGS_H

#ifdef	__cplusplus
extern "C" {
#endif

// Defines

// Defaults
#define FREQ_1_DEFAULT      100             // Default lower corner frequency
#define FREQ_2_DEFAULT      3500            // Default upper corner frequency

// Common Data addresses
#define EE_COM_1    0           // Storage for common settings    
#define EE_COM_2    1           //       
// Preset Addresses
//   
// The preset address form the base address and point to
// start of the parameter address range for each preset
#define EE_PS0      2           // Storage for current settings
#define EE_PS1      4           // Preset 1 base address
#define EE_PS2      6           // Preset 2 base address
#define EE_PS3      8           // Preset 3 base address
#define EE_PS4      10          // Preset 4 base address
#define EE_PS5      12          // Preset 5 base address
#define EE_PS6      14          // Preset 6 base address
#define EE_PS7      16          // Preset 7 base address
    
// Parameter Offsets
//    
// The offsets are added to the modified base address to point to
// each parameter location within the preset group
#define PS_F1       0           // Freq 1
#define PS_F2       1           // Freq 2

// Declarations
//extern volatile int audio_routing;    

// Prototypes    
void load_settings();
void save_filter(int);
void load_filter(int);
void load_tone_gain();
void save_tone_gain();


#ifdef	__cplusplus
}
#endif

#endif	/* SETTINGS_H */    