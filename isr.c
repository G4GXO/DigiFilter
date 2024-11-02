// DigiFilter
//
// (c)Ron Taylor, G4GXO
//
// ISR Routine
//
// The ISR is triggered at the ADC sampling rate of 128kHz. Within the ISR, a new sample is
// captured, decimation filtering takes place and rate is reduced to the 8kHz sampling rate.
// On each 8kHz branch the DAC input registers are updated.

#include <xc.h>
#include "Main.h"
#include "isr.h"
#include "controls.h"
#include "filters.h"
#include "tones.h"
#include "settings.h"
#include "analogues.h"

volatile extern int sig_pwr;                    // Input signal power
unsigned int overload;                          // Input warning LED timer

static int audio_routing;                       // ISR audio path
static int audio_path_temp;                     // ISR audio path temp store
static int led_busy=1;                          // 0=LED available, 1=LED in use

static int r_ch;                                // Right output data
static int l_ch;                                // Left output data    

void __attribute__((__interrupt__, auto_psv)) _DAC1RInterrupt(void)
{
//    TEST_PIN_SET                // Probe to check ISR rate  
//   __asm__ volatile ("bset  LATB, #14");        
   AD1CON1bits.SAMP=0;                          // Stop sampling and trigger conversion
   while (!AD1CON1bits.DONE);                   // Check that conversion has completed   
   fir_in=ADC1BUF0;                              // Copy sample to FIR filter input
   AD1CON1bits.DONE=0;                          // Clear completion bit          
   AD1CON1bits.SAMP=1;                          // Start sampling for next ISR

   DAC1RDAT=r_ch;                               // Load right word (previous pass)
   DAC1LDAT=l_ch;                               // load left word (previous pass)
   _DAC1RIF=0;                                  // Clear DAC Interrupt  

   if(!led_busy){               // If LED is free use it as the signal indicator
    // Signal detector
    if(sig_pwr>CLIPPING){       // If we exceed input threshold, load overload counter
       overload=1000;           // Load "monostable"
    }
    if(overload){
        --overload;             // Decrement counter
       RED_LED=1;               // Illuminate Red LED
       GRN_LED=0;               //
        }
    else if(sig_pwr>LOW_SIGNAL){// If greater than low threshold
       RED_LED=0;               //
       GRN_LED=1;               // Illuminate Green LED
       }
    else{                       // Otherwise signal is low
       RED_LED=0;               // extinguish LEDs
       GRN_LED=0;               //
       }   
   }
   
   // Audio output routing
   switch (audio_routing){

       case FILTER:
           l_ch=fir_out;                        //
           r_ch=fir_out;                        //
           break;

       case DENOISE:
           denoise_in=fir_out;
           l_ch=denoise_out;                    //
           r_ch=denoise_out;                    //
           break;

       case AUTONOTCH:
           autonotch_in=fir_out;
           l_ch=autonotch_out;                  //
           r_ch=autonotch_out;                  //
           break;

       case ALL:
           denoise_in=fir_out;
           autonotch_in=denoise_out;
           l_ch=autonotch_out;                  //
           r_ch=autonotch_out;                  //
           break;

       case BIN_FILTER:
           binaural_IN=fir_out;
           l_ch=binaural_L;                     //
           r_ch=binaural_R;                     //
           break;

       case BIN_DENOISE:
           denoise_in=fir_out;
           binaural_IN=denoise_out;
           l_ch=binaural_L;                     //
           r_ch=binaural_R;                     //
           break;

       case BIN_AUTONOTCH:
           binaural_IN=fir_out;
           l_ch=binaural_L;                     //
           r_ch=binaural_R;                     //
           break;

       case BIN_ALL:
           denoise_in=fir_out;
           binaural_IN=denoise_out;
           l_ch=binaural_L;                     //
           r_ch=binaural_R;                     //
           break;

        case TONES:
           l_ch=tone_out;                       //
           r_ch=tone_out;                       //
           break;

        default:
           l_ch=fir_out;                        //
           r_ch=fir_out;                        //
           break;
        }   
}

void write_audio_routing(int path){         // Populate audio path
    audio_routing=path;
}

int read_audio_routing(){                  // Read current path
    return(audio_routing);
}

void save_audio_routing(){                  // Save audio path
    audio_path_temp=audio_routing;
}

void restore_audio_routing(){               // Restore audio path
    audio_routing=audio_path_temp;
}

void led_is_busy(){
    led_busy=1;
}

void led_is_free(){
    led_busy=0;
}
