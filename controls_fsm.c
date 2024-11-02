// DigiFilter
//
// (c)Ron Taylor G4GXO
//
// Finite State Machine
//
// A Function Pointer based scheme for menu navigation and parameter change

// State and Event Enumerations
//
// The enum type assigns incrementing numerical values to each item defined starting
// at zero unless otherwise specified.

#include <xc.h>
#include <stdio.h>
#include "Main.h"
#include "controls_fsm.h"
#include "controls.h"
#include "encoder.h"
#include "switches.h"

// State, Event and function to call Structure
//
// Defines the structure of the parameters used to call a function

 typedef struct {
        states state;       // New state
        events event;       // New event
        int(*func)();       // Pointer to function to call
    }state_matrix;

  typedef struct {
        af_states af_state; // New state
        af_events af_event; // New event
        int(*af_func)();    // Pointer to function to call
    }af_state_matrix;

// Current State and Event Variables
//
// Holds the current state and the returned event. The initialistion values
// are also defined here to determine where the state machine starts from

    int state=ST_INIT;          // Initialisation
    static int event=EV_NONE;   //

//--------------------------------------------------------------------------
// Controls State Transition Matrix
//
// This array is used to look up what function to call in response to a new
// event and the current state

    // Valid Operating States
   static state_matrix state_trans[]={
    // State        Event       Function

    // Circular Menu   
    {ST_ANY,            EV_ENC_A_PRESS, (*button_beep)},       // Button press sound          

    {ST_FILTER,         EV_ENC_A_UP,    (*menu_preset)},       // Point to Denoiser
    {ST_FILTER,         EV_ENC_A_REL,   (*freq_2_sel)},        // Enter adjustment
    {ST_FILTER,         EV_ENC_A_DOWN,  (*menu_anotch)},       // Point to Autonotch
    {ST_FILTER,         EV_ENC_A_HOLD,  (*filter_menu)},       // Point to Filter
    
    {ST_ANOTCH,         EV_ENC_A_UP,    (*menu_filter)},       // Point to Filter
    {ST_ANOTCH,         EV_ENC_A_REL,   (*autonotch_toggle)},  // Toggle Autonotch
    {ST_ANOTCH,         EV_ENC_A_DOWN,  (*menu_denoise)},      // Point to Denoiser
    {ST_ANOTCH,         EV_ENC_A_HOLD,  (*menu_anotch)},       // Point to Autonotch    

    {ST_DENOISE,        EV_ENC_A_UP,    (*menu_anotch)},       // Point to Autonotch
    {ST_DENOISE,        EV_ENC_A_REL,   (*denoiser_in)},       // Engage Denoiser    
    {ST_DENOISE,        EV_ENC_A_DOWN,  (*menu_binaural)},     // Point to Binaural   
    {ST_DENOISE,        EV_ENC_A_HOLD,  (*menu_denoise)},      // Point to Denoiser
    
    {ST_BINAURAL,       EV_ENC_A_UP,    (*menu_denoise)},      // Point to Denoiser
    {ST_BINAURAL,       EV_ENC_A_REL,   (*binaural_in)},       // Toggle Binaural on
    {ST_BINAURAL,       EV_ENC_A_DOWN,  (*menu_tone_gain)},    // Point to Tone Gain
    {ST_BINAURAL,       EV_ENC_A_HOLD,  (*menu_binaural)},     // Point to Binaural 
    
    {ST_TONE_GAIN,      EV_ENC_A_UP,    (*menu_binaural)},     // Point to Binaural
    {ST_TONE_GAIN,      EV_ENC_A_REL,   (*tone_gain_adj)},     // Enter adjustment
    {ST_TONE_GAIN,      EV_ENC_A_DOWN,  (*menu_preset)},       // Point to Preset    
    {ST_TONE_GAIN,      EV_ENC_A_HOLD,  (*menu_tone_gain)},    // Point to Tone Gain
    
    {ST_PRESET,         EV_ENC_A_UP,    (*menu_tone_gain)},    // Point to Tone Gain
    {ST_PRESET,         EV_ENC_A_REL,   (*save_preset)},       // Enter adjustment
    {ST_PRESET,         EV_ENC_A_DOWN,  (*menu_filter)},       // Point to Filter    
    {ST_PRESET,         EV_ENC_A_HOLD,  (*menu_preset)},       // Point to Preset    
    
    // Functions
    // Filter  
    {ST_FREQ_2,         EV_ENC_A_REL,   (*freq_1_sel)},        // Switch to Lower frequency mode
    {ST_FREQ_1,         EV_ENC_A_REL,   (*freq_2_sel)},        // Switch to Upper frequency mode
    {ST_FREQ_2,         EV_ENC_A_UP,    (*freq_2_inc)},        // LP corner frequency increase
    {ST_FREQ_2,         EV_ENC_A_DOWN,  (*freq_2_dec)},        // LP corner frequency decrease
    {ST_FREQ_1,         EV_ENC_A_UP,    (*freq_1_inc)},        // HP corner frequency increase
    {ST_FREQ_1,         EV_ENC_A_DOWN,  (*freq_1_dec)},        // HP corner frequency decrease
    {ST_FREQ_1,         EV_ENC_A_HOLD,  (*filter_menu)},       // Exit filter adjust, return to menu
    {ST_FREQ_2,         EV_ENC_A_HOLD,  (*filter_menu)},       // Exit filter adjust, return to menu

    // Denoiser
    {ST_DENOISE_ADJ,     EV_ENC_A_REL,   (*denoiser_out)},      // Disengage Denoiser
    {ST_DENOISE_ADJ,     EV_ENC_A_UP,    (*denoiser_inc)},      // Increase denoise
    {ST_DENOISE_ADJ,     EV_ENC_A_DOWN,  (*denoiser_dec)},      // Decrease denoise
    {ST_DENOISE_ADJ,     EV_ENC_A_HOLD,  (*menu_denoise)},      // Return to menu

    // Binaural
    {ST_BINAURAL_BAL,   EV_ENC_A_UP,    (*bin_right_bal)},     // Balance to right
    {ST_BINAURAL_BAL,   EV_ENC_A_DOWN,  (*bin_left_bal)},      // Balance to left
    {ST_BINAURAL_BAL,   EV_ENC_A_REL,   (*binaural_out)},      // Toggle Binaural off
    {ST_BINAURAL_BAL,   EV_ENC_A_HOLD,  (*menu_binaural)},     // Return to menu   
    
    {ST_TONE_GAIN_ADJ,  EV_ENC_A_HOLD,  (*tone_gain_exit)},     // Save and return to Tone Gain menu
    {ST_TONE_GAIN_ADJ,  EV_ENC_A_UP,    (*tone_gain_inc)},      // Increase Tone Gain    
    {ST_TONE_GAIN_ADJ,  EV_ENC_A_DOWN,  (*tone_gain_dec)},      // Decrease Tone Gain    

    // Memory Load
    {ST_ANY,            EV_PRESET_CHG,  (*load_preset)},        // Load preset
 
    // Initialisation
    {ST_MENU_START,     EV_ENC_A_UP,    (*menu_filter)},        // Point to Filter
    {ST_MENU_START,     EV_ENC_A_REL,   (*freq_2_sel)},         // Enter adjustment
    {ST_MENU_START,     EV_ENC_A_DOWN,  (*menu_filter)},        // Point to Filter    
    
    {ST_INIT,           EV_NONE,        (*init_fsm)},          // Normal initialisation
    };

// Runtime calculation of state table entries (saves manual coding)
#define STATE_COUNT sizeof(state_trans)/sizeof(state_trans[0])

//--------------------------------------------------------------------------
// Get Event Routine
//
// Examines the encoder and switch. If a change is seen the corresponding event state
// is passed to the finite state machine for action. The order in which the events
// are handled is important.

// Operating Controls
int get_event(){
    int flags;
    // Encoder A
    flags=enc_A();                  // Get encoder flags
    if(flags){                      // If any are set search for event
    // Button actions
    if(flags&ENC_HOLD){             // Hold active
        return EV_ENC_A_HOLD;       // Return event
        }
    else if(flags&ENC_REL){         // Hold released
        return EV_ENC_A_REL;        // Return event
        }
    else if(flags&ENC_PRESS){       // Press active
        return EV_ENC_A_PRESS;      // Return event
        }

    // Encoder rotate
    else if(flags&ENC_UP){          // Test UP flag
        return EV_ENC_A_UP;         // Return event
        }
    else if(flags&ENC_DOWN){        // Test DOWN flag     
        return EV_ENC_A_DOWN;       // Return event
        }
    }
 
    // Preset Switch Check   
    flags=presets_read();           // Read presets
        if(flags&CHANGE){           // If change flag bit set
            return EV_PRESET_CHG;   // Return event
            }

    return EV_NONE;                 // The fall-through event (no events)
}


//------------------------------------------------------------------------------
// Controls Finite State Machine
//
// This short routine checks for a new event and if seen uses the State Transition
// Matrix to determine the current state and corresponding action for the new event.
//
void main_fsm(){
    int i;                          // Counter

    event = get_event();            // Check for new event
    
    if(event!=EV_NONE){             // If event seen
    // Step through each state and check for correlation with state and event
    // In addition to individual states and events an "ANY" condition is
    // included to apply an action irrespective of state or event.
    for(i=0; i<STATE_COUNT; i++){
         if((state_trans[i].state == state)||(state_trans[i].state == ST_ANY)){
             if((state_trans[i].event == event)||(state_trans[i].event == EV_ANY)){
                 // Call the function associated with transition (also returns next state)
                state = (state_trans[i].func)();
                break;
                }
            }
        }
    reset_enc_A();  // Clear any new flags that have arrived during execution
    }
}
