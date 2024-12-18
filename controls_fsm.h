// DigiFilter 
//
// (c)Ron Taylor, G4GXO 
// 
// File:   controls_fsm.h
//  

#ifndef CONTROLS_FSM_H
#define	CONTROLS_FSM_H

#ifdef	__cplusplus
extern "C" {
#endif


// Defines
   
    
// Declarations
    typedef enum {          // State List
        ST_ANY,             // Any state
        ST_FILTER,          // Filter mode
        ST_DENOISE,         // Denoiser mode
        ST_FREQ_2,          // Upper frequency mode
        ST_FREQ_1,          // Lower frequency mode
        ST_DENOISE_ADJ,     // Denoiser in circuit, adjustment
        ST_DENOISE_OUT,     // Denoiser out
        ST_ANOTCH,          // Autonotch in circuit
        ST_BINAURAL,        // Binaural mode
        ST_BINAURAL_DLY,    // Binaural delay adjust
        ST_BINAURAL_BAL,    // Binaural balance        
        ST_BINAURAL_IN,     // Binaural mode enabled
        ST_BINAURAL_OUT,    // Binaural mode disabled
        ST_TONE_GAIN,       // Tone Gain
        ST_TONE_GAIN_ADJ,   // Tome Gain Adjust
        ST_PRESET,          // Save to preset        
        ST_MENU_START,      // Menu Start Up State        
        ST_INIT,            // Initialisation    
    }states;

    typedef enum {          // Event List
        EV_ENC_A_UP,        // Encoder up
        EV_ENC_A_DOWN,      // Encoder down
        EV_ENC_A_PRESS,     // Encoder switch press
        EV_ENC_A_HOLD,      // Press and hold event
        EV_ENC_A_REL,       // Press and hold released
        EV_PRESET_CHG,      // New preset switch change seen
        EV_NONE,            // No event
        EV_INIT,            // Initialisation
        EV_ANY,        
    }events;

     typedef enum {         // State List
        ST_AF_ANY,          // Any state
        ST_AF_GAIN,         // AF Gain adjust
        ST_AF_BAL,          // AF Balance adjust
        ST_PGA_GAIN,        // PGA Gain adjust
        ST_AF_INIT,         // Initialisation
     }af_states;

    typedef enum {          // Event List
        EV_AF_ANY,          // Any event
        EV_AF_NONE,         // No event
        EV_ENC_B_UP,        // Encoder B up
        EV_ENC_B_DOWN,      // Encoder B down
        EV_B_PRESS,         // Encoder B switch press
        EV_B_PRESS_REL,     // Encoder B switch released
        EV_B_HOLD,          // Press and hold event
        EV_B_HOLD_REL,      // Press and hold released
        EV_AF_WAIT,         // Dummy event used to stop fsm making decision
    }af_events;

extern int state;    

// Prototypes    
int get_event();
int get_af_event();
void main_fsm();


#ifdef	__cplusplus
}
#endif

#endif	/* CONTROLS_FSM_H */

