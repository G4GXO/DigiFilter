// DigiFilter
//
// (c)Ron Taylor, G4GXO
//
// Filter Routines

// Filter Kernel Files
#include <xc.h>
#include <stdio.h>
#include "main.h"
#include "filters.h"
#include "gains.h"

// General
static int freq_1;              // Lower corner frequency  
static int freq_2;              // Upper corner frequency  

// Filter Builder
extern int  fs;                 // Sampling rate
extern int  fc;                 // Low pass corner frequency
extern int  Fc;                 // Fractional frequency fc/fs
extern int  A_dc_coeff;         // A kernel central coefficient address
extern int  A_coeff[ML];        // A Kernel
extern int  A_coeff_end;        // End of A coefficients
extern int  B_dc_coeff;         // B kernel central coefficient address
extern int  B_coeff[ML];        // B Kernel
extern int  B_coeff_end;        // End of B coefficients

extern int  C_coeff[ML];        // C Kernel for holding new filter results
extern int  C_dc_coeff;         // C kernel central coefficient address
extern int  C_coeff_end;        // End of C coefficients

// Main FIR Filter
extern int  fir_n;              // Filter length
extern int  fir_m;              // FIR length -1
extern int	fir_coeff[ML];		// Coefficient table
extern int	fir_coeff_end;		// Last address of coefficients
extern int	fir_delay[ML];		// Filter delay line
extern int	fir_delay_end;		// Last address of tap vector
extern int 	fir_delay_ptr;		// Delay line pointer
extern int  fir_in;             // Filter input
extern int  fir_out;            // Filter output

// Binaural Delay
extern int  bin_n;              // Variable delay line length
extern int  bin_delay[BIN_DLY]; // Binaural delay line
extern int  bin_delay_end;      // Binaural delay line end
extern int  bin_delay_ptr;      // Binaural delay line pointer
extern int  bin_delay_den;      // Centre tap
extern int  binaural_IN;        // Binaural input (capital IN to avoid conflict)
extern int  binaural_R;         // Right output
extern int  binaural_L;         // Left output    

// De-noiser Variables
extern int	den_coeff[LN];		// De-noiser coeff storage
extern int	den_coeff_end;		// End of coeff storage
extern int	den_coeff_ptr;		// Coeff pointer
extern int	den_n;				// Filter length
extern int  den_m;              // N-1   
extern int	den_delay[LN];		// Delay line
extern int	den_delay_end;		// Delay line end
extern int	den_delay_ptr;		// Delay line pointer
extern int	den_decay;			// Decay factor
extern int	den_beta;       	// Beta factor
extern int  denoise_in;         // Input sample
extern int  denoise_out;        // Output sample
extern int  den_FIR_out;        // FIR output
extern int  den_1dB;            // Gain compensation 1dB
extern int  den_6dB;            // Gain compensation 6dB
int         den_index;          // Table pointer for denoise levels

// Autonotcher Variables
extern int	aut_coeff[LA];		// Autonotcher coeff storage
extern int	aut_coeff_end;		// End of coeff storage
extern int	aut_coeff_ptr;		// Coeff pointer
extern int	aut_n;				// Filter length
extern int  aut_m;              // FIR loop count
extern int	aut_delay[LA];		// Delay line
extern int	aut_delay_end;		// Delay line end
extern int	aut_delay_ptr;		// Delay line pointer
extern int	aut_decay;			// Decay factor
extern int	aut_beta;			// Beta factor
extern int  autonotch_in_dlyd;  // Delayed input
extern int	aut_dec_delay[LIA]; // Input delay line
extern int	aut_dec_end;		// End of input delay line
extern int	aut_dec_ptr;		// Input delay line pointer
extern int	aut_dec_n;          // Input delay length
extern signed int aut_FIR_in;	// Delayed input to FIR delay line
extern signed int autonotch_in;	// Input sample
extern signed int autonotch_out;// Output sample

// General Variables
extern signed int	AF_in;		// AF samples into filter
extern signed int	AF_out;		// AF samples out of filter

// Filter Selection
unsigned int	filter;			// Filter number for fir_load


// Values for Taylor series sin polynomial calculation (EMRFD Ch10.9)
// Dummy value used for last coefficient to fit with computation loop

//int sin_coeff[6]={0x3240, 0x0053, 0xAACC, 0x08B7, 0x1CCE, 0x0000};

// Denoiser Parameters
//
// Table of denoiser variables used to apply different levels of denoise. The
// 6dB gain entries represent the number of left shifts of denoiser out at 6dB 
// per shift. The hex value is a negative integer which is used by the accumulator 
// shift function to invoke a left shift. The gain 1dB is a factor used to provide
// 1dB gain interpolation between the coarse 6dB steps.
//
// In the current version the decorrelation delay is not used.

const unsigned int den_parameters[][6]={
//   Beta    Decay     FIR      DLY     Gain    Gain           Gain  
//           factor    Taps     Taps    6dB     1dB
   {0x0100,  0x7ff0,   24,      4,    0xFFFE,   0x7fff, }, // 
   {0x0200,  0x7ff0,   24,      4,    0xFFFE,   0x7fff, }, //  
   {0x0400,  0x7ff0,   24,      4,    0xFFFE,   0x5A9D, }, // 
   {0x0800,  0x7fd0,   24,      4,    0xFFFF,   0x7fff, }, //  
   {0x1000,  0x7fb0,   24,      4,    0xFFFF,   0x5A9D, }, //            
};    

// Table of reciprocals used in filter coefficient calculations
//
int i_reciprocal[250]={
0x7FFF,	0x4000,	0x2AAA,	0x2000,	0x1999,	0x1555,	0x1249,	0x1000,	0x0E39,	0x0CCD,
0x0BA3,	0x0AAB,	0x09D9,	0x0925,	0x0888,	0x0800,	0x0787,	0x071C,	0x06BD,	0x0666,
0x0618,	0x05D1,	0x0591,	0x0555,	0x051F,	0x04EC,	0x04BE,	0x0492,	0x046A,	0x0444,
0x0421,	0x0400,	0x03E1,	0x03C4,	0x03A8,	0x038E,	0x0376,	0x035E,	0x0348,	0x0333,
0x031F,	0x030C,	0x02FA,	0x02E9,	0x02D8,	0x02C8,	0x02B9,	0x02AB,	0x029D,	0x028F,
0x0282,	0x0276,	0x026A,	0x025F,	0x0254,	0x0249,	0x023F,	0x0235,	0x022B,	0x0222,
0x0219,	0x0211,	0x0208,	0x0200,	0x01F8,	0x01F0,	0x01E9,	0x01E2,	0x01DB,	0x01D4,
0x01CE,	0x01C7,	0x01C1,	0x01BB,	0x01B5,	0x01AF,	0x01AA,	0x01A4,	0x019F,	0x019A,
0x0195,	0x0190,	0x018B,	0x0186,	0x0181,	0x017D,	0x0179,	0x0174,	0x0170,	0x016C,
0x0168,	0x0164,	0x0160,	0x015D,	0x0159,	0x0155,	0x0152,	0x014E,	0x014B,	0x0148,
0x0144,	0x0141,	0x013E,	0x013B,	0x0138,	0x0135,	0x0132,	0x012F,	0x012D,	0x012A,
0x0127,	0x0125,	0x0122,	0x011F,	0x011D,	0x011A,	0x0118,	0x0116,	0x0113,	0x0111,
0x010F,	0x010D,	0x010A,	0x0108,	0x0106,	0x0104,	0x0102,	0x0100,	0x00FE,	0x00FC,
0x00FA,	0x00F8,	0x00F6,	0x00F5,	0x00F3,	0x00F1,	0x00EF,	0x00ED,	0x00EC,	0x00EA,
0x00E8,	0x00E7,	0x00E5,	0x00E4,	0x00E2,	0x00E0,	0x00DF,	0x00DD,	0x00DC,	0x00DA,
0x00D9,	0x00D8,	0x00D6,	0x00D5,	0x00D3,	0x00D2,	0x00D1,	0x00CF,	0x00CE,	0x00CD,
0x00CC,	0x00CA,	0x00C9,	0x00C8,	0x00C7,	0x00C5,	0x00C4,	0x00C3,	0x00C2,	0x00C1,
0x00C0,	0x00BF,	0x00BD,	0x00BC,	0x00BB,	0x00BA,	0x00B9,	0x00B8,	0x00B7,	0x00B6,
0x00B5,	0x00B4,	0x00B3,	0x00B2,	0x00B1,	0x00B0,	0x00AF,	0x00AE,	0x00AD,	0x00AC,
0x00AC,	0x00AB,	0x00AA,	0x00A9,	0x00A8,	0x00A7,	0x00A6,	0x00A5,	0x00A5,	0x00A4,
0x00A3,	0x00A2,	0x00A1,	0x00A1,	0x00A0,	0x009F,	0x009E,	0x009E,	0x009D,	0x009C,
0x009B,	0x009B,	0x009A,	0x0099,	0x0098,	0x0098,	0x0097,	0x0096,	0x0096,	0x0095,
0x0094,	0x0094,	0x0093,	0x0092,	0x0092,	0x0091,	0x0090,	0x0090,	0x008F,	0x008E,
0x008E,	0x008D,	0x008D,	0x008C,	0x008B,	0x008B,	0x008A,	0x008A,	0x0089,	0x0089,
0x0088,	0x0087,	0x0087,	0x0086,	0x0086,	0x0085,	0x0085,	0x0084,	0x0084,	0x0083,
};

// Blackman Window from DC+1 to DC+251, 250 terms
const int blackman[250]={
0x7FFD,	0x7FF7,	0x7FEC,	0x7FDD,	0x7FCA,	0x7FB3,	0x7F97,	0x7F77,	0x7F54,	0x7F2B,
0x7EFF,	0x7ECF,	0x7E9A,	0x7E62,	0x7E25,	0x7DE4,	0x7D9F,	0x7D56,	0x7D09,	0x7CB8,
0x7C63,	0x7C0A,	0x7BAE,	0x7B4D,	0x7AE8,	0x7A80,	0x7A14,	0x79A5,	0x7931,	0x78BA,
0x783F,	0x77C1,	0x773F,	0x76BA,	0x7631,	0x75A5,	0x7516,	0x7483,	0x73ED,	0x7354,
0x72B7,	0x7218,	0x7175,	0x70D0,	0x7027,	0x6F7C,	0x6ECE,	0x6E1D,	0x6D69,	0x6CB3,
0x6BFA,	0x6B3E,	0x6A80,	0x69C0,	0x68FD,	0x6838,	0x6771,	0x66A7,	0x65DB,	0x650E,
0x643E,	0x636D,	0x6299,	0x61C4,	0x60ED,	0x6014,	0x5F3A,	0x5E5E,	0x5D81,	0x5CA2,
0x5BC2,	0x5AE1,	0x59FE,	0x591B,	0x5836,	0x5750,	0x566A,	0x5582,	0x549A,	0x53B1,
0x52C7,	0x51DD,	0x50F2,	0x5006,	0x4F1A,	0x4E2E,	0x4D42,	0x4C55,	0x4B68,	0x4A7B,
0x498E,	0x48A1,	0x47B4,	0x46C7,	0x45DA,	0x44EE,	0x4402,	0x4316,	0x422B,	0x4140,
0x4056,	0x3F6C,	0x3E83,	0x3D9B,	0x3CB3,	0x3BCD,	0x3AE7,	0x3A02,	0x391E,	0x383B,
0x3759,	0x3678,	0x3598,	0x34BA,	0x33DD,	0x3301,	0x3226,	0x314D,	0x3075,	0x2F9E,
0x2EC9,	0x2DF6,	0x2D24,	0x2C53,	0x2B85,	0x2AB8,	0x29EC,	0x2923,	0x285B,	0x2795,
0x26D0,	0x260E,	0x254D,	0x248F,	0x23D2,	0x2317,	0x225E,	0x21A7,	0x20F2,	0x203F,
0x1F8E,	0x1EDF,	0x1E32,	0x1D87,	0x1CDF,	0x1C38,	0x1B93,	0x1AF1,	0x1A51,	0x19B3,
0x1917,	0x187D,	0x17E5,	0x174F,	0x16BC,	0x162B,	0x159C,	0x150F,	0x1484,	0x13FB,
0x1375,	0x12F1,	0x126F,	0x11EF,	0x1171,	0x10F5,	0x107B,	0x1004,	0x0F8F,	0x0F1B,
0x0EAA,	0x0E3B,	0x0DCE,	0x0D63,	0x0CFA,	0x0C93,	0x0C2E,	0x0BCB,	0x0B6B,	0x0B0C,
0x0AAF,	0x0A54,	0x09FB,	0x09A4,	0x094E,	0x08FB,	0x08A9,	0x085A,	0x080C,	0x07C0,
0x0775,	0x072D,	0x06E6,	0x06A1,	0x065E,	0x061C,	0x05DC,	0x059E,	0x0561,	0x0526,
0x04EC,	0x04B4,	0x047E,	0x0449,	0x0415,	0x03E3,	0x03B3,	0x0384,	0x0356,	0x032A,
0x02FF,	0x02D5,	0x02AD,	0x0286,	0x0261,	0x023D,	0x021A,	0x01F8,	0x01D7,	0x01B8,
0x019A,	0x017D,	0x0161,	0x0147,	0x012D,	0x0115,	0x00FE,	0x00E8,	0x00D2,	0x00BE,
0x00AC,	0x009A,	0x0089,	0x0079,	0x006A,	0x005C,	0x004F,	0x0044,	0x0039,	0x002F,
0x0026,	0x001E,	0x0017,	0x0011,	0x000C,	0x0007,	0x0004,	0x0002,	0x0000,	0x0000,
};

//==============================================================================
//
// Filter Functions
//
//==============================================================================
//
// Clear Filter Delay
//
// Fill delay line with zeros to clear it of data.

void fir_clear(void){
	int i;                          // Declare a counter
	for (i=0; i<ML; i++){           // Loop through all taps
			fir_delay[i]=0;         // Clear the delay line tap register at [i]
		}    
}

// Intialisation Routine
// Filter
void init_fir(void){
    int i;
        // Initialise FIR Filter
        fir_n=NFIR;                                 // Filter length
        fir_m=fir_n-1;                              // FIR Loop count
        // Define central DC terms and ends of calculation kernels
        A_dc_coeff=(int)A_coeff+((2*ML)-2)/2;       // A kernel central DC term address
        A_coeff_end=(int)A_coeff+((2*ML)-2);        // A kernel coefficients end for reflection
        B_dc_coeff=(int)B_coeff+((2*ML)-2)/2;       // B kernel central DC term address
        B_coeff_end=(int)B_coeff+((2*ML)-2);        // B kernel coefficients end for reflection      
        C_dc_coeff=(int)C_coeff+((2*ML)-2)/2;       // C kernel central DC term address
        C_coeff_end=(int)C_coeff+((2*ML)-2);        // C kernel coefficients end for reflection
        // Define end of operating kernel
        fir_coeff_end=(int)fir_coeff+((2*ML-2));
        fir_delay_ptr=(int)fir_delay;               // Set pointer to start position of left delay line
        fir_delay_end=fir_delay_ptr+((2*ML)-1);     // Calculate delay line end
        // Copy Window data into x-memory
        for(i=0;i<250;i++){
            window[i]=blackman[i];
        }
}

// Populate Denoise index from settings.c
//void write_den_index(int index){                    // Load index
//    den_index=index;
//} 

int read_den_index(){                               // Return current value to caller
    return(den_index);
}

// Set Denoiser Taps
void set_den_taps(void){
    // FIR
	den_delay_ptr=(int)den_delay;                   // Set pointer to start position of delay line
	den_delay_end=(int)den_delay_ptr+((2*den_n)-1); // Calculate end of delay line
	den_coeff_ptr=(int)den_coeff;                   // Set pointer to start position of coefficients
	den_coeff_end=(int)den_coeff_ptr+((2*den_n)-1); // Calculate end of coefficients
}

// Load Denoiser Parameters from table
void load_den(int den_index){
    den_beta=den_parameters[den_index][0];      // Load new beta factor
    den_decay=den_parameters[den_index][1];     // Load new decay factor
    den_n=den_parameters[den_index][2];         // Load new taps value
    den_m=den_n-1;                              // Set M as N-1     
    den_6dB=den_parameters[den_index][4];       // Load new coarse gain
    den_1dB=den_parameters[den_index][5];       // Load new fine gain
}

void init_den(void){
    int i;
    den_index=0;                                // Load parameter set index
    load_den(den_index);                        // Load new parameter set
    set_den_taps();                             // Configure filter
    for (i=0; i<33; i++){
        den_coeff[i]=1234;
    }
}

int den_inc(){
    int limit=0;
    den_index++;                                // Increment table index
    if(den_index>DEN_MAX){                      // Test upper limit
        den_index=DEN_MAX;                      // and hold if exceeded
        limit=1;
    }
    load_den(den_index);                        // Load new parameter set
    set_den_taps();                             // Apply new filter length        
    return(limit);     
}

int den_dec(){
    int limit=0;
    den_index--;                                // Increment table index
    if(den_index<DEN_MIN){                      // Test upper limit
        den_index=DEN_MIN;                      // and hold if exceeded
        limit=1;                                // Signal lower limit
    }
    load_den(den_index);                        // Load new parameter set
    set_den_taps();                             // Apply new filter length        
    return(limit);
}

// Initialise Autonotch
void init_aut(void){
	aut_n=AUT_TAPS;                                 // Filter length
    aut_m=aut_n-1;                                  // M
	aut_delay_ptr=(int)aut_delay;					// Set pointer to start position of delay line
	aut_delay_end=(int)aut_delay_ptr+((2*aut_n)-1);	// Calculate end of delay line
	aut_coeff_ptr=(int)aut_coeff;					// Set pointer to start position of coefficients
	aut_coeff_end=(int)aut_coeff_ptr+((2*aut_n)-1); // Calculate end of coefficients
	aut_decay=AUT_DECAY;							// Load decay factor
	aut_beta=AUT_BETA;								// Load beta factor
	aut_dec_n=AUT_INPUT_TAPS;                       // Input delay line length
	aut_dec_ptr=(int)aut_dec_delay;                 // Set input pointer to start of input delay
	aut_dec_end=(int)aut_dec_ptr+((2*aut_dec_n)-1); // Calculate end of input delay line
}

// Populate Binaural Delay index from settings.c
void write_bin_delay(int dly){
    bin_n=dly;                                      // Load delay line length 
}

int read_bin_delay(){                               // Return current value to caller
    return(bin_n);
}

// Initialise Binaural
void init_binaural(){
    bin_gains_init();                               // Load centre gains
    bin_n=BIN_DLY;                                  // Load delay    
	bin_delay_ptr=(int)bin_delay;                   // Set pointer to start position of delay line
    bin_delay_set();                                // Calculate delay taps
}

void bin_delay_set(){
    int i;    
    bin_delay_end=(int)bin_delay+((2*bin_n)-1);     // Calculate delay end   
    for (i=0;i<bin_n;i++){                          // Clear delay line
        bin_delay[i]=0;                             // Set records to zero   
    }      
}
//------------------------------------------------------------------------------
// Populate freq_1 and freq_2
void write_freq(int f1, int f2){
    freq_1=f1;
    freq_2=f2;
}
// Read freq_1
int read_freq_1(){
   return (freq_1);                     // Return value to caller
}
// Read freq_2
int read_freq_2(){
   return (freq_2);                     // Return value to caller
}
//------------------------------------------------------------------------------
// FIR Corner Frequency Changes
//
// Increment lower corner frequency
int inc_freq_1(){
    freq_1=freq_1+FREQ_STEP;            // Increment
    if (freq_1>(freq_2-100)){           // Test for upper limit of 100Hz below upper frequency
        freq_1=(freq_2-100);            // If exceeded hold at limit
    }
    return(freq_1);                     // Return freq_1 or limit
}
//
// Decrement lower corner frequency
int dec_freq_1(){
    freq_1=freq_1-FREQ_STEP;            // Increment
    if(freq_1<FREQ_1_MIN){              // Test for lower limit
        freq_1=FREQ_1_MIN;              // If less than, hold at limit
        }    
    return(freq_1);
}
//------------------------------------------------------------------------------
// Increment upper corner frequency
int inc_freq_2(){
    freq_2=freq_2+FREQ_STEP;            // Increment
    if (freq_2>FREQ_2_MAX){             // Test for upper limit 
        freq_2=FREQ_2_MAX;              // If exceeded hold at limit
        }
    return(freq_2);                       // Return freq_2 or limit
}
//------------------------------------------------------------------------------
// Decrement upper corner frequency
int dec_freq_2(){
    freq_2=freq_2-FREQ_STEP;            // Increment
    if(freq_2<(freq_1+100)){            // Test for lower limit 100Hz above freq_1
        freq_2=freq_1+100;              // If less than hold at limit
    }    
    return(freq_2);
}
//------------------------------------------------------------------------------
