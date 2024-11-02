// DigiFilter 
//
// (c)Ron Taylor, G4GXO 
// 
// File:   control.h
//  

#ifndef CONTROLS_H
#define	CONTROLS_H

#ifdef	__cplusplus
extern "C" {
#endif

// Defines
    
//#define BW_MIN              100             // Minimum passband width

// Audio Path Controls
#define PASSTHRU           0x0000   //0b00000000       // Pass through mode
#define FILTER_IN          0x0001   //0b00000001       // Filter only
#define FILTER_OUT         0xfffe   //0b11111110       // Filter out of circuit
#define DENOISE_ON         0x0002   //0b00000010       // Denoise on
#define DENOISE_OFF        0xfffd   //0b11111101       // Denoise off
#define AUTONOTCH_ON       0x0004   //0b00000100       // Autonotch on
#define AUTONOTCH_OFF      0xfffb   //0b11111011       // Autonotch off
#define BINAURAL_ON        0x0008   //0b00001000       // Binaural on
#define BINAURAL_OFF       0xfff7   //0b11110111       // Binaural off
#define TONES              0x0080   //0b10000000       // Tones only

// Audio Routing States
#define NONE                PASSTHRU
#define FILTER              FILTER_IN
#define DENOISE             (FILTER_IN|DENOISE_ON)
#define AUTONOTCH           (FILTER_IN|AUTONOTCH_ON)
#define ALL                 (FILTER_IN|DENOISE_ON|AUTONOTCH_ON)
#define BIN_FILTER          (FILTER_IN|BINAURAL_ON)
#define BIN_DENOISE         (FILTER_IN|DENOISE_ON|BINAURAL_ON)
#define BIN_AUTONOTCH       (FILTER_IN|AUTONOTCH_ON|BINAURAL_ON)
#define BIN_ALL             (FILTER_IN|DENOISE_ON|AUTONOTCH_ON|BINAURAL_ON)

 // Declarations
    
    
// Prototypes    
void tone_path();
void restore_path();
void limit_high_tone();
void limit_low_tone();
void preset_saved();
int button_beep();
int af_press_tone();
int menu_filter();
void init_filter();
int filter_upper();             // Upper skirt adjust
int filter_lower();             // Lower skirt adjust    
int menu_denoise();
int menu_anotch();
int menu_binaural();
int menu_tone_gain();
int menu_preset();
void start_msg();
int freq_1_sel();
int freq_2_sel();
int filter_menu();                       // Drop back to menu level leaving operation unchanged
int freq_1_inc(); 
int freq_1_dec(); 
int freq_2_inc();
int freq_2_dec();
int denoiser_in();                       // Engage denoiser
int denoiser_out();                      // Disengage denoiser
int denoiser_inc();                      // Increase denoise
int denoiser_dec();                      // Decrease denoise
int autonotch_toggle();                  // Autonotch on/off
int binaural_in();                       // Enable binaural output
int binaural_out();                      // Disable binaural output
int binaural_bal();                      // Select balance adjustment
int bin_right_bal();                     // Binaural balance right
int bin_left_bal();                      // Binaural balance left   
int tone_gain_adj();                     // Enter adjustment
int tone_gain_exit();                    // Save and exit to top menu   
int tone_gain_inc();                     // Increase tone gain
int tone_gain_dec();                     // Decrease tone gain
int load_preset();                       // Load preset configuration    
int save_preset();                       // Save preset configuration
int init_fsm();
int init_af_fsm();

   
#ifdef	__cplusplus
}
#endif

#endif	/* CONTROLS_H */