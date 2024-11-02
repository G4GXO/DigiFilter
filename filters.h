// DigiFilter 
//
// (c)Ron Taylor, G4GXO  
// 
// File:   filters.h
//  

#ifndef FILTERS_H
#define	FILTERS_H

#ifdef	__cplusplus
extern "C" {
#endif

// Declarations    

// Filter Definitions   
        
// FIR Length
#define ML                  501         // Filter delay line reserved length in WORDS (must match assembler file BYTES/2)
//#define BL                  121         // Binaural delay length
#define NFIR                501         // FIR delay length
#define LIMIT               0x7FFF      // Limit alarm value for signalling out of range     

// Filter Control Definitions
#define FREQ_2_MAX          5000        // Upper corner frequency max
#define FREQ_1_MIN          100         // Lower corner frequency min
#define FREQ_STEP           50          // Step used for independent corner changes
#define BW_STEP             25          // Step used for bandwidth changes, both skirts give 50Hz step
#define BW_MAX              3400        // Maximum bandwidth
#define BW_MIN              50          // Minimum bandwidth
    
// Denoise table limits 
#define DEN_MAX             4    
#define DEN_MIN             0           // Minimum table index
#define DEN_DEFAULT         0           // Default setting
    
// Autonotcher Constants
#define AUT_INPUT_TAPS      48          // Input delay line taps
#define AUT_TAPS            33          // Number of filter taps
#define	AUT_DECAY           0x7ff0      // Autonotch decay factor
#define AUT_BETA            0x1000  	// Beta value

//Filter variables
#define LN                  64          // Denoiser delay
#define LA                  64          // Autonotcher delay
#define LIA                 64          // Autonotcher decorrelation delay

// Binaural Delay
#define BIN_DLY             100         // Fixed delay    
    
// Reciprocal series for 1/[I-M/2] (1/i for i=251 to 501)
extern const unsigned int den_parameters[][6]; 

// General
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
extern int	fir_coeff[ML];		// Coefficient table
extern int	fir_coeff_end;		// Last address of coefficients
extern int	fir_delay[ML];		// Filter delay line
extern int	fir_delay_end;		// Last address of tap vector
extern int 	fir_delay_ptr;		// Delay line pointer
extern int  fir_in;             // Filter input
extern int  fir_out;            // Filter output

// Binaural Delay
extern int  bin_delay[BIN_DLY]; // Binaural delay line
extern int  bin_delay_end;      // Binaural delay line end
extern int  bin_delay_ptr;      // Binaural delay line pointer
extern int  bin_n;              // Binaural delay line length
extern int  binaural_IN;        // Binaural input
extern int  binaural_R;         // Right output
extern int  binaural_L;         // Left output

// Denoiser Variables
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
extern int  den_FIR_out;        // FIR output
extern int  denoise_out;        // Output sample
extern int  den_1dB;            // Gain compensation 1dB
extern int  den_6dB;            // Gain compensation 6dB
extern int  den_index;          // Table pointer for denoise levels
extern int	den_dec_end;        // Decorrelation Delay line end
extern int	den_dec_ptr;        // Decorrelation Delay line pointer

// Autonotcher Variables
extern int	aut_coeff[LA];		// Autonotcher coeff storage
extern int	aut_coeff_end;		// End of coeff storage
extern int	aut_coeff_ptr;		// Coeff pointer
extern int	aut_n;				// Filter length
extern int	aut_delay[LA];		// Delay line
extern int	aut_delay_end;		// Delay line end
extern int	aut_delay_ptr;		// Delay line pointer
extern int	aut_decay;			// Decay factor
extern int	aut_beta;			// Beta factor
extern int	aut_dec_delay[LIA]; // Input delay line
extern int	aut_dec_end;		// End of input delay line
extern int	aut_dec_ptr;		// Input delay line pointer
extern int	aut_dec_n;          // Input delay length
extern signed int aut_FIR_in;	// Delayed input to FIR delay line
extern signed int aut_FIR_out;  // FIR output
extern signed int autonotch_in;	// Input sample
extern signed int autonotch_out;// Output sample

// General Variables
extern signed int	AF_in;		// AF samples into filter
extern signed int	AF_out;		// AF samples out of filter

extern int i_reciprocal[250];
extern int window[250];

// Prototypes
void fir_clear(void);
void init_fir(void);                                                    //
void set_den_taps(void);
void load_den(int);
void init_den(void);
//void write_den_index(int);
int read_den_index();
void init_aut(void);
void init_binaural();
void bin_delay_set();
void write_bin_delay(int);
int read_bin_delay();
int binaural_inc();
int binaural_dec();
int inc_freq_1();
int dec_freq_1();
int inc_freq_2();
int dec_freq_2();
void write_freq(int,int);
int read_freq_1();
int read_freq_2();
int den_inc();
int den_dec();
void A_coeff_build();
void B_coeff_build();
void asm_def();
#ifdef	__cplusplus
}
#endif

#endif	/* FILTERS_H */