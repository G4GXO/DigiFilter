// DigiFilter
//
// (c)Ron Taylor, G4GXO
//
// Gains
//

#include "gains.h"

static int tone_gain;
extern int tone_gain_factor;

static int balance=0;               // Initialise balance at zero, (equal gain)
static int right_gain;
static int left_gain;
extern int right_gain_factor;
extern int left_gain_factor;

// Gain factors in 1dB steps from -43dB to -0dB
const int dB_table[44]={0x00E7,0x0104,0x0124,0x0147,
                        0x016F,0x019C,0x01CE,0x0207,
                        0x0246,0x028D,0x02DD,0x0337,
                        0x039B,0x040C,0x048A,0x0518,
                        0x05B7,0x066A,0x0732,0x0813,
                        0x090F,0x0A2A,0x0B68,0x0CCC,
                        0x0E5C,0x101D,0x1214,0x1449,
                        0x16C2,0x1989,0x1CA7,0x2026,
                        0x2412,0x2879,0x2D6A,0x32F4,
                        0x392C,0x4026,0x47FA,0x50C2,
                        0x5A9D,0x65AB,0x7213,0x7FFF,};

// Install gain factors from index values, (loaded from settings)
//
void init_gains(){
    tone_gain_load();
    bin_gains_init();
}

// Set Tone Gain
void tone_gain_load(){
    tone_gain_factor=dB_table[tone_gain];    
}
// Populate static tone gain variable
void write_tone_gain(int gain_index){
    tone_gain=gain_index;
    tone_gain_factor=dB_table[tone_gain];      // Load factor from table     
}
// Return tone gain index to caller
int read_tone_gain(){
    return(tone_gain);
}
// Load Gain Factor
//void tone_gain_init(int index){
//    tone_gain_factor=dB_table[tone_gain];      // Load factor from table 
//}

// Increase tone gain
//
int tone_inc_gain(){
    int limit=0;
    tone_gain++;                               // Increment table index
    if(tone_gain>TONE_GAIN_MAX){               // Test upper limit
        tone_gain=TONE_GAIN_MAX;               // and hold if exceeded
        limit=1;                               // Signal limit reached 
    }
    else{
        tone_gain_load();                      // Load new parameter set        
    }
    return(limit);     
}
//
// Decrease tone gain
//
int tone_dec_gain(){
    int limit=0;
    tone_gain--;                               // Increment table index
    if(tone_gain<TONE_GAIN_MIN){               // Test upper limit
        tone_gain=TONE_GAIN_MIN;               // and hold if exceeded
        limit=1;                               // Signal limit reached 
    }
    else{
        tone_gain_load();                      // Load new parameter set        
    }
    return(limit);     
}
//
// Binaural Balance
//
//void bin_bal_init(int left_gain, int index_right){
//    left_gain_factor=dB_table[index_left];      // Load left gain factor
//    right_gain_factor=dB_table[index_right];    // Load right gain factor       
//}

int read_bin_bal(){                             // Return balance value to caller
    return(balance);
}

void write_bin_bal(int index){                  // Populate balance index values
    balance=index;                              //
    bin_gains_load();                           // Apply gain factors
}

void bin_gains_load(){                          // Load binaural gain factors    
    if(balance>0){                              // If positive
        right_gain=BIN_GAIN_MAX;                // Set right gain to max
        left_gain=(BIN_GAIN_MAX-balance);       // Set left gain to difference
    }
    else if(balance<0){                         // If negative
        right_gain=(BIN_GAIN_MAX+balance);      // Set right gain to difference     
        left_gain=BIN_GAIN_MAX;                 // Set left gain to max  
    }
    else{                                       // Balance at zero
        left_gain=BIN_GAIN_MAX;                 // Set left channel to max
        right_gain=BIN_GAIN_MAX;                // Set right channel to max        
    }
    left_gain_factor=dB_table[left_gain];       // Load left gain factor
    right_gain_factor=dB_table[right_gain];     // Load right gain factor    
}

void bin_gains_init(){                          // Centre gains
    balance=0;                                  // Zero balance
    left_gain=BIN_GAIN_MAX;                     // Set left channel to max
    right_gain=BIN_GAIN_MAX;                    // Set right channel to max
    left_gain_factor=dB_table[left_gain];       // Load left gain factor
    right_gain_factor=dB_table[right_gain];     // Load right gain factor          
}

//
// Increase Gain Right
//
int bin_bal_right(){
    int limit=0;

    if(balance<BIN_BAL_MAX){                    // If we are below max
        balance++;                              // Increment balance
        bin_gains_load();                       // Load gain factors
    }
    else{                                       // If we already at max, signal
        limit=1;                                // adjustment limit reached
    }
    return(limit);    
}
//
// Increase Gain Left
//
int bin_bal_left(){
    int limit=0;    
    
    if(balance>BIN_BAL_MIN){                    // If we are above min
        balance--;                              // Decrement balance
        bin_gains_load();                       // Load gain factors
    }
    else{                                       // If we already at max, signal
        limit=1;                                // adjustment limit reached
    }
    return(limit); 
}


