// DigiFilter
//
// (c)Ron Taylor G4GXO
//
// Encoder Routine
//
// Encoder Channel Assignment
//
// The encoder channel comprises of two bits, in the examples below the encoder 
// is connected to bits 0 and 1. For clarity only the associated byte is shown.
//
// Detection
//
// The encoder pins may generate interrupts to force capture and translation or
// be regularly scanned and examined for change. In both cases a state word is
// formed from the current and previous encoder pin states. For convenience the
// state word is assembled into two 4bit nibbles to produce an immediately
// visible number sequence. The formation of the state word is illustrated below
//
// Previous Encoder State       00000010
// New Encoder State            00000011
//
// Shift new bits left 4 bits into upper nibble and OR in previous state to form
// the state word;
//
// Previous Encoder State       00000010
// Shifted New Encoder State    00110000                              
// OR in Previous state         00110010
//
// The state word is 00110010 which in hex is 32 
//
// The valid rotational states produce by this method are as follows;
//
// Clockwise            0x0023, 0x0002, 0x0010, 0x0031
//
// Anti-clockwise       0x0032, 0x0013, 0x0020, 0x0001
//
// By acting only upon these states the effects of contact bounce and false
// stepping are greatly reduced. The simple state machine looks for these states
// and if present sets a flag bit in the encoder output word to signal if a
// rotation has been detected and in what direction it occurred.
//
// Switch Section
//
// The encoder switch is regularly scanned to detect operation. When pressed a
// memory flag is set which is used on release to decode whether a press/release
// action has occurred or a longer press and hold event. The resulting event is
// encoded into the encoder flag word to be interpreted by the calling routine.
//

#include <xc.h>
#include <stdio.h>
#include "main.h"
#include "encoder.h"              // Encoder definitions

static int    enc_A_new;          // Current encoder state
static int    enc_A_last;         // Previous encoder state
static int    enc_A_sw_mem;       // Memory flag to detect first operation 
static int    enc_A_sw_count;     // Counter for press and hold event
static int    enc_A_flags;        // Encoder A flags  
static int    enc_A_rate=0;       // Step rate (Slow=0, Fast=1)
static int    enc_A_rel_mask=0;   // Mask encoder release    

// Rotary Encoder

int enc_A(){
    unsigned int enc_temp;          // Temporary storage    
    
// Run encoder A processes
//
// Encoder A Push switch section
// Normal state high, low = operated. Check switch state before encoder channels
// to prevent unintentional encoder change whilst operating switch.
    enc_A_flags=0;                              // Clear flags word
    
    if(!ENC_A_SW){                              // Is switch pressed?
        if(!enc_A_sw_mem){                      // If the memory flag is clear this is new operation
            enc_A_sw_mem=1;                     // Set memory flag
            enc_A_flags=ENC_PRESS;              // Yes, set press flag  
            enc_A_sw_count=0;                   // Zero press and hold counter                
        }
        enc_A_sw_count++;                       // Increment press and hold counter
        if(enc_A_sw_count==HOLD_COUNT){         // If press reaches hold count
            enc_A_flags=ENC_HOLD;               // Set hold flag
            enc_A_rel_mask=1;                   // Set release masking
        }
    }
    else if(enc_A_sw_mem){                      // Switch released, if memory flag set this is first release
        enc_A_sw_mem=0;                         // Clear memory flag
        if(enc_A_rel_mask){                     // If previous action was a HOLD, skip release flag
            enc_A_rel_mask=0;                   // Clear release masking             
        }       
        else if(enc_A_sw_count<HOLD_COUNT){     // If masking not set and we did not reach hold state during press, 
            enc_A_flags=ENC_REL;                // set release flag                          
        }      
    }
// Encoder A Rotary Section
//
// Encoder A bits
//   
// Encoder A Rotary Encoder Section
// enc_A_new=0b0000 0000 bb00 0000, Masked encoder bits
// Shifted   0b0000 0000 00bb 0000, Shifted into upper nibble of state byte    
//                       **** ****  Two digit state byte
    
    enc_A_new=(ENC_A_PORT & ENC_A_MASK)>>2;     // Get new encoder state and shift to upper nibble
    if(enc_A_new^enc_A_last<<4){                // If there is change, process direction    
        enc_temp=enc_A_new|enc_A_last;          // Form two digit state word
        if(enc_A_rate){                         // If rate non zero, select fast step
            switch (enc_temp){                  // Decode number into direction
                case 0x0023: case 0x0002: case 0x0010: case 0x0031: // Up states
                    enc_A_flags=ENC_UP;         // Set up flag
                    break;

                case 0x0032: case 0x0013: case 0x0020: case 0x0001: // Down states
                    enc_A_flags=ENC_DOWN;       // Set down flag               
                    break;
                }            
        }
        else{                                   // Rate is zero, step on single change
            switch (enc_temp){                  // Decode number into direction                                 
                case 0x0023:                    // Up state
                    enc_A_flags=ENC_UP;         // Set up flag
                    break;

                case 0x0032:                    // Down state
                    enc_A_flags=ENC_DOWN;       // Set down flag               
                    break;
            }                       
        }
        enc_A_last=enc_A_new>>4;                // Shift new value into last bits   
    }        
return(enc_A_flags);                            // Return flags to caller    
}

//******************************************************************************
// Step rate Change Functions
// Encoder step rate is changed by setting the number of states in the encoder
// state tables. Typically, slow stepping is used for menu navigation and fast
// stepping for adjustments. This makes encoder use easier and more intuitive.
//
void enc_A_fast(){                  // Generate step flag on each state change
    enc_A_rate=ENC_FAST;
}

void enc_A_slow(){                  // Generate step flag on single state change
    enc_A_rate=ENC_SLOW;
}
//******************************************************************************
// Function to clear any encoder changes to prevent false states. Typical usage 
// might be between changes in encoder use or transmit/receive states in radio 
// applications
//
void reset_enc_A(void){
    enc_A_new=ENC_A_PORT&ENC_A_MASK;    // Get current encoder state
    enc_A_last=enc_A_new;               // Copy to old and new states 
    enc_A_flags=0;                      // Clear flags   
}
//******************************************************************************
// Prepare encoder for start up with no false steps
//
void init_enc_A(){
    reset_enc_A();
    enc_A_rate=ENC_SLOW;
}
//******************************************************************************
