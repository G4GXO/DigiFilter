// DigiFilter
//
// (c)Ron Taylor G4GXO
//
// Control Routines
//
#include <xc.h>
#include <stdio.h>
#include "main.h"
#include "controls.h"               // Control definitions
#include "tones.h"
#include "controls_fsm.h"
#include "filters.h"
#include "gains.h"
#include "switches.h"
#include "settings.h"
#include "isr.h"
#include "delays.h"
#include "analogues.h"
#include "encoder.h"

//==============================================================================
// Control Tones
//
// Switch audio path for tones out
void tone_path(){
    save_audio_routing();               // Save current ISR audio output routing
    write_audio_routing(TONES);         // Switch output to tones
}
// Restore audio output path
void restore_path(){        
   restore_audio_routing();             // Restore ISR audio routing
}

void limit_high_tone(){
    tone_path();                        // Switch output to tone generator
    morse_generator(F1500Hz,S);         // High tone "S"
    restore_path();                     // Restore original audio path
}

void limit_low_tone(){
    tone_path();                        // Switch output to tone generator
    morse_generator(F700Hz,S);          // Low tone "S"
    restore_path();                     // Restore original audio path
}

int button_beep(){                      // Encoder A press tone
    tone_path();                        // Switch out to tone generator
    morse_generator(F1500Hz,E);         // 1500Hz "E"
    restore_path();                     // Restore original audio path
    return state;
}

void enc_step_beep(){
    tone_path();                        // Switch out to tone generator
    morse_generator(F800Hz,E);          // 800Hz "E"
    restore_path();                     // Restore original audio path  
}

void preset_saved(){
    tone_path();                        // Switch out to tone generator
    morse_generator(F800Hz,K);          // 800Hz "K"
    restore_path();                     // Restore original audio path
}

//==============================================================================
// Menu Encoder A
//
// Set state according to menu navigation and send corresponding tones

int menu_filter(){
    tone_path();                        // Switch out to tone generator
    morse_generator(F800Hz,F);
    restore_path();                     // Restore original audio path
    return ST_FILTER;
}

int filter_upper(){             // Upper skirt adjust
    tone_path();                // Switch out to tone generator
    morse_generator(F800Hz,U);
    restore_path();             // Restore original audio path
    return ST_FILTER;
}

int filter_lower(){             // Lower skirt adjust    
    tone_path();                // Switch out to tone generator
    morse_generator(F800Hz,L);
    restore_path();             // Restore original audio path
    return ST_FILTER;
}

int menu_denoise(){
    tone_path();                        // Switch out to tone generator
    morse_generator(F800Hz,D);
    restore_path();                     // Restore original audio path
    return ST_DENOISE;
}

int menu_anotch(){
    tone_path();                        // Switch out to tone generator
    morse_generator(F800Hz,A);
    restore_path();                     // Restore original audio path
    return ST_ANOTCH;
}

int menu_binaural(){
    tone_path();                        // Switch out to tone generator
    morse_generator(F800Hz,B);
    restore_path();                     // Restore original audio path
    return ST_BINAURAL;
}

int binaural_bal(){                     // Select balance adjust
    tone_path();                        // Switch out to tone generator
    morse_generator(F800Hz,Q);          // Send a "Q"
    restore_path();                     // Restore audio path
    return ST_BINAURAL_BAL;             // Return new state
}

int binaural_dly(){                     // Select delay adjust
    tone_path();                        // Switch out to tone generator
    morse_generator(F800Hz,T);          // Send a "T" 
    restore_path();                     // Restore audio path
    return ST_BINAURAL_DLY;             // Return new state
}

int menu_tone_gain(){
    tone_path();                        // Switch out to tone generator
    morse_generator(F800Hz,G);
    restore_path();                     // Restore original audio path
    return ST_TONE_GAIN;
}

int menu_preset(){
    tone_path();                        // Switch out to tone generator
    morse_generator(F800Hz,P);
    restore_path();                     // Restore original audio path
    return ST_PRESET;
}

//==============================================================================
// Start Up Message
//
void start_msg(){
    tone_path();                        // Switch out to tone generator
    morse_generator(F800Hz,D);
    morse_generator(F800Hz,F);
    morse_generator(F800Hz,SPACE);
    morse_generator(F800Hz,ONE);
    morse_generator(F800Hz,V);
    morse_generator(F800Hz,ZERO);    
    restore_path();                     // Restore original audio path
}

//==============================================================================
// Filter Routines
//
// Initialise FIR Filter
void init_filter(){
    fc=read_freq_1();                   // Copy freq_1 to fc
    A_coeff_build();                    // Construct new lower frequency kernel
    fc=read_freq_2();                   // Copy freq_2 to fc
    B_coeff_build();                    // Construct new upper frequency kernel
    spectral_inversion();               // Merge with higher kernel and form bandpass
}

// Switch adjustment mode between HP and LP corner frequencies
//
// Switch to lower HP corner Freq 1
int freq_1_sel(){
    filter_lower();                     // Announce lower skirt    
    return ST_FREQ_1;
}

// Switch to upper LP corner Freq 2 (Entry mode)
int freq_2_sel(){
    filter_upper();                     // Announce upper skirt
    enc_A_fast();                       // Select fast encoder stepping
    return ST_FREQ_2;
}

int filter_menu(){                       // Drop back to menu level leaving operation unchanged
//    menu_entry();                        // Announce return to menu
    enc_A_slow();                        // Return to slow change rate
    menu_filter();                       // Announce menu   
    return ST_FILTER;                    // Return state
}

// Corner Frequency changes
//
// Kernel A Lower Corner
// Kernel B Upper Kernel
//
// Freq 1 - Lower Corner, HP frequency
//
static int freq1;

int freq_1_inc(){
    fc=inc_freq_1();                    // Increment freq_1
    if(fc==freq1){                      // If no change
        limit_high_tone();              // Signal upper limit        
    }
    else{                               // If changed,   
        freq1=fc;                       // update freq and calculate new filter
        A_coeff_build();                // Construct new lower frequency kernel
        spectral_inversion();           // Merge with higher kernel and form bandpass        
    }
    return ST_FREQ_1;
}

int freq_1_dec(){
    fc=dec_freq_1();                    // Decrement freq_1
    if(fc==freq1){                      // Test for limit
        limit_low_tone();               // Signal lower limit
    }
    else{                               // If changed,
        freq1=fc;                       // update freq and calculate new filter
        A_coeff_build();                // Construct new lower frequency kernel
        spectral_inversion();           // Merge with higher kernel and form bandpass
    }
    return ST_FREQ_1;    
}
// Freq 2 - Upper Corner, LP Frequency

static int freq2;

int freq_2_inc(){
    fc=inc_freq_2();                    // Increment freq and get new value
    if(fc==freq2){                      // If no change
        limit_high_tone();              // Signal upper limit
    }
    else{                               // If changed,
        freq2=fc;                       // update freq and calculate new filter                           
        B_coeff_build();                // Construct new upper frequency kernel
        spectral_inversion();           // Merge with higher kernel and form bandpass
    }
    return ST_FREQ_2;
}

int freq_2_dec(){
    fc=dec_freq_2();                    // Decrement
    if(fc==freq2){                      // If no change,
        limit_low_tone();               // Signal lower limit
    }
    else{                               // If changed,
        freq2=fc;                       // update freq and calculate new filter
        B_coeff_build();                // Construct new upper frequency kernel
        spectral_inversion();           // Merge with higher kernel and form bandpass
    }
    return ST_FREQ_2;
}

//==============================================================================
// Denoiser Routines 
//

int denoiser_in(){                              // Engage denoiser
    int path;
    path=read_audio_routing();                  // Get current path
    write_audio_routing(path|DENOISE_ON);       // OR-in denoise path
    DEN_LED=ON;                                 // Show On
    return ST_DENOISE_ADJ;                      // Return state
}

int denoiser_out(){                             // Disengage denoiser
    int path;
    path=read_audio_routing();                  // Get current path
    write_audio_routing(path&DENOISE_OFF);      // AND-out denoise path
    DEN_LED=OFF;                                // Show On    
    return ST_DENOISE;                          // Return state
}

int denoiser_inc(){                             // Increase denoise
    int limit;
    limit=den_inc();                            // Increment table index
    if(limit){
        limit_high_tone();                      // Signal upper limit        
    }
    else{
        enc_step_beep();                        // Confirm pip        
    }
    return ST_DENOISE_ADJ;
}

int denoiser_dec(){                             // Decrease denoise
    int limit;
    limit=den_dec();                            // Increment table index
    if(limit){
        limit_low_tone();                       // Signal lower limit        
    }
    else{
        enc_step_beep();                        // Confirm pip        
    }
    return ST_DENOISE_ADJ;
}

//==============================================================================
// Autonnotch Routines
//

int autonotch_toggle(){
    int path;
    path=read_audio_routing();                  // Get current path
    write_audio_routing(path^=AUTONOTCH_ON);    // Toggle Autonotch path
    if(path&AUTONOTCH_ON){                      // If enabled    
        AUT_LED=ON;                             // Show On        
    }
    else{
        AUT_LED=OFF;                            // Show Off        
    }
    return ST_ANOTCH;
}

//==============================================================================
// Binaural Routines
//
// Fixed delay version. Press/Rel to toggle Binaural on/off. When Binaural is 
// enabled, the encoder controls channel balance. Press/Hold to exit to menu
// leaving Binaural enabled.

int binaural_in(){                              // Enable Binaural
    int path;    
    path=read_audio_routing();                  // Get current path
    write_audio_routing(path|BINAURAL_ON);      // Or-In Binaural path
    BIN_LED=ON;                                 // Show On    
    return ST_BINAURAL_BAL;                     // Return state
}

int binaural_out(){                             // Disable Binaural
    int path;    
    path=read_audio_routing();                  // Get current path
    write_audio_routing(path&BINAURAL_OFF);     // And-Out Binaural path
    BIN_LED=OFF;                                // Show Off    
    return ST_BINAURAL;                         // Return state
}

// Binaural Balance Functions
//
// Increase Right Gain
int bin_right_bal(){
    int limit;
    limit=bin_bal_right();
    if(limit){
        limit_high_tone();                       // Signal limit
    }
    return ST_BINAURAL_BAL;
}

// Increase Left Gain
int bin_left_bal(){
    int limit;
    limit=bin_bal_left();
    if(limit){
        limit_low_tone();                       // Signal limit
    }   
    return ST_BINAURAL_BAL;
}

//==============================================================================
// Tone Gain Routines
//
int tone_gain_adj(){                            // Enter adjustment
    enc_step_beep();                            // Confirm pip        
    return ST_TONE_GAIN_ADJ;                    // Return state
}

int tone_gain_exit(){                           // Return to top menu
    save_tone_gain();
    menu_tone_gain();                           // Announce menu
    return ST_TONE_GAIN;                        // Return state
}

int tone_gain_inc(){
    int limit;
    limit=tone_inc_gain();                      // Increase gain
    if(limit){
        limit_high_tone();                      // Signal upper limit
    }
    else{
        enc_step_beep();                        // Confirm pip        
    }    
    return ST_TONE_GAIN_ADJ;
}

int tone_gain_dec(){
    int limit;
    limit=tone_dec_gain();                      // Decrease gain
    if(limit){
        limit_low_tone();                       // Signal upper limit
    }
    else{
        enc_step_beep();                        // Confirm pip        
    }
    return ST_TONE_GAIN_ADJ;
}

//==============================================================================
// Stored Configuration Routines
//
// Grounding any of the four "Preset" pins loads the complete parameter set stored
// against that pin. The parameters may be changed as in normal operation and if 
// desired saved back to the preset address by pressing and holding the encoder
// whilst in Menu mode.

int load_preset(){                              // Load preset configuration
    preset_load();                              // Load preset
    fc=read_freq_1();                           // Load lower corner frequency
    A_coeff_build();                            // Construct new lower frequency kernel
    fc=read_freq_2();                           // Load upper corner frequency
    B_coeff_build();                            // Construct new upper frequency kernel
    spectral_inversion();                       // Merge with higher kernel and form bandpass
    return ST_FILTER;                           // Return state
}

int save_preset(){                              // Save preset configuration
    preset_save();                              // Save settings to current address
    preset_saved();                             // Signal setting saved
    return ST_PRESET;                           // Return state
}

//==============================================================================
// Initialisation Routines
//
int init_fsm(){
    write_audio_routing(FILTER);        // Start with filter only    
    return ST_MENU_START;               // Initial Encoder action will be Filter
}

