// DigiFilter
//
// (c)Ron Taylor, G4GXO
//
// Description
//
// An audio processor comprising of brick wall filter with adjustable upper and
// lower corner frequencies, an NLMS Denoiser, Autonotch and Binaural output.
// The menu system is announced in Morse code and comprises the following options;
//
//
// Menu Abbreviations
// F    Filter, Press/release to enter, turn to adjust
//      U Upper corner frequency adjust
//      L Lower corner frequency adjust
// A    Autonotch, Press/release to toggle on/off
// D    Denoiser, Press/release to toggle on/off, turn to adjust, press and hold to exit
// B    Binaural, Press/release to toggle on/off, turn to adjust channel balance.
// P    Preset, Set preset address by grounding pin(s) and press/release to save
//      filter corner frequencies to preset address.
//
// Operating Tones
// E    Signals encoder press or rotation increments in certain menus.
// S    Stop. Range limit reached. High pitch for upper, low pitch for lower.
// K    Setting saved signal.
//
//------------------------------------------------------------------------------
// Things To Do
//
//
//------------------------------------------------------------------------------
// History
// Version 1.0  Initial Release. Variable Binaural removed in favour of fixed
//              delay. In Binaural encoder now only changes channel balance.
//              Press/hold to exit with Binaural on. Press/Rel to toggle on/off.
//------------------------------------------------------------------------------
//
// Include Files
#include <xc.h>
#include <math.h>	
#include <stdio.h>
#include <stdlib.h>
#include <libpic30.h>
            
#include "Main.h"                       // Main program definitions
#include "eeprom.h"                     // EEPROM routines
#include "settings.h"                   // Setup routines
#include "switches.h"                   // Switch inputs
#include "controls_fsm.h"               // Control finite state machine
#include "controls.h"                   // Controls header
#include "encoder.h"                    // Encoder inputs
#include "filters.h"                    // Filter configuration routines
#include "isr.h"                        // ISR
#include "delays.h"                     // Delay routines
#include "tones.h"                      // Signal tones
#include "analogues.h"                  // Analogue peripherals
#include "gains.h"                      // Gains

// Processor Configuration
#pragma config BWRP = WRPROTECT_OFF        
#pragma config SWRP = WRPROTECT_OFF         
#pragma config FNOSC = FRC        
#pragma config FCKSM = CSECMD  
#pragma config OSCIOFNC = ON        
#pragma config POSCMD = NONE         
#pragma config GSS = OFF        
#pragma config GWRP = OFF
#pragma config FWDTEN = OFF
#pragma config WINDIS = OFF  
#pragma config FPWRT = PWR128
#pragma config JTAGEN = OFF                     // Debug Enable
//#pragma config JTAGEN = ON                     // Debug Enable
#pragma config ICS = PGD1                       // Debug Port
#pragma config UID0 = 00000002                  // Version Number  

// Start of Main program

int main(void){
    // Configure Clock PLL prescaler, PLL postscaler, PLL divisor for 20MIPS operation 
    // with internal 7.37MHz oscillator
    PLLFBD=41;                  // M = 43,	158.455MHz VCO divided by 43 = 3.685MHz
    CLKDIVbits.PLLPOST = 0;		// N2=2, 158.455/2 = 79.225MHz = 39MIPS // N2 = 4,	158MHz/4 = 39MHz = 19.5MIPS
    CLKDIVbits.PLLPRE = 0; 		// N1 = 2,      Fin =3.685MHz

    // Initiate clock switch to FRC with PLL
    __builtin_write_OSCCONH(0x01);
    __builtin_write_OSCCONL(0x01);
    while(OSCCONbits.COSC!= 0b001);     // Wait for clock switch
    while(OSCCONbits.LOCK!=1) {};       // Wait for PLL to lock

    // Configure DAC clock source
    ACLKCON=0x0700;                 // VCO output to divider, ACLK disabled, Divide by 1
    PMD1=0x01FE;                    // Disable unused modules    
	// Configure I/O Port Directions
    TRISA=PORTA_DIR;                // Set up PORTA directions
    TRISB=PORTB_DIR;                // Set up PORTB directions
    AD1PCFGL=0xFFFD;                // Set up Port A digital pins for preset read      
    CNPU1=PULL_UPS1;                // Enable pulls ups on inputs
    CNPU2=PULL_UPS2;                //    
    _LATB8=0;                       // Set LED pin data register to low
    _LATB9=0; 
    DataEEInit();                   // Initialise EEPROM emulation
    fs=FS;                          // Sampling rate
    init_fir();                     // Initialise filters
    init_den();                     // Initialise denoiser
    init_aut();                     // Initialise autonotch
    init_binaural();                // Initialise binaural delay
    init_tones();                   // Initialise tones
    load_settings();                // Load settings and initialise 
    init_filter();                  // Set up filter
    init_gains();                   // Initialise gain factors    
    init_enc_A();                   // Initialise encoder    
    state=init_fsm();               // Initialise operation
    init_adc();                     // Initialise ADC
    init_dac();                     // Initialise DAC
    delay_ms(500);                  // Let it all settle
    interrupt_enable();             // Start interrupts
    // Ready to go, check to see if version is being requested
    if(!ENC_A_SW){                  // If encoder switch pressed
        start_msg();                // Announce version           
        do{                         // Wait for encoder release
            delay_ms(10);
        }while(!ENC_A_SW);
        delay_ms(100);              // Debounce and exit
    }
    
	// Main program Loop
	do{				
       Idle();                      // Wait for ISR
       af_processing();             // Run audio processes
       main_fsm();                  // Run FSM
        }while(1);
    return 0;
}
