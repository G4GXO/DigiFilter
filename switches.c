// DigiFilter
//
// (c)Ron Taylor, G4GXO
//

#include <xc.h>
#include "main.h"
#include "switches.h"
#include "settings.h"
#include "delays.h"

static int presets_last=0;                // Previous switch setting
static int presets=0;                     // Current switch setting  
 
void preset_load(){
    int ee_base_addr=get_preset_addr();   // Get EEPROM address
        load_filter(ee_base_addr);        // Load filter held at address offset
    }

void preset_save(){
    int ee_base_addr=get_preset_addr();   // Get EEPROM address
    save_filter(ee_base_addr);            // Save filter to current address  
}

//
// Translate switch setting into EEPROM base address
//
int get_preset_addr(){
    int ee_base_addr;
    presets=(SWITCH_PORT&SWITCH_MASK);    // Get current input pin states    
         switch (presets){                // Get EEPROM address offset
            case PRESET_0:                //
                ee_base_addr=EE_PS0;      // If no input grounded, set ee_addr_offset to address zero
            break;

            case PRESET_1:                // Test each input pin state
                ee_base_addr=EE_PS1;      // and recorded first grounded
            break;                        // by ee_addr_offset number 1..4

            case PRESET_2:                //
                ee_base_addr=EE_PS2;      //
            break;                        //

            case PRESET_3:                //
                ee_base_addr=EE_PS3;      //
            break;                        //

            case PRESET_4:                //
                ee_base_addr=EE_PS4;      //
            break;                        //
            
            case PRESET_5:                //
                ee_base_addr=EE_PS5;      //
            break;                        //

            case PRESET_6:                //
                ee_base_addr=EE_PS6;      //
            break;                        //            

            case PRESET_7:                //
                ee_base_addr=EE_PS7;      //
            break;                        //
            
            default:                      //  
                ee_base_addr=EE_PS0;      //
            break;                        //
         }
    return ee_base_addr;
}

int presets_read(){                         // Read presets and return state
    presets=(SWITCH_PORT&SWITCH_MASK);      // Get current input pin states
    if(presets^presets_last){               // If change seen
        delay_ms(100);                      // Debounce
        presets_last=presets;               // Record new state
        presets=presets|CHANGE;             // Mark this is a new value
    }
    return(presets);                        // Return flag to caller
}

int init_presets(){
    presets=presets_read();                 // Read preset pin states
    presets=presets&0x7fff;                 // Mask off change bit    
    presets_last=presets;                   // Update historic state
    return(presets);                        // Return setting                           
}
