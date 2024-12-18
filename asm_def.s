; DigiFilter
;
; (c)Ron Taylor, G4GXO
;
;*********************************************************************************************
; ASSEMBLER VARIABLES AND MEMORY ALLOCATION
;
; These definitions allow the assembler files to be integrated within the C program. The global 
; defintions allow variables to be shared by the C and assembler routines. Memory is reserved
; here within the X and Y data space for the filter delay lines and their associated variables.
;
;*********************************************************************************************

; Reserve Y Memory for Filter Kernels and Variables
	.section yfir, bss, ymemory, align(4096)

; Filter Coefficients
    .global _A_coeff          ;
_A_coeff: .space 1002
    .global _B_coeff          ;
_B_coeff: .space 1002
    .global _C_coeff          ;
_C_coeff: .space 1002 
 
; Main Filter Coefficients
	.global	_fir_coeff	; Coefficients start
_fir_coeff:	.space 1002	; Storage for coefficients in bytes
	.global _fir_coeff_end  ; Coefficients end
_fir_coeff_end: .space 2        ;
	.global	_fir_n		; Kernel length (N)
_fir_n:		.space 2        ;
	.global _fir_m		;   Delay line length -1
_fir_m:		.space 2	;	
    .global _fir_dc_coeff       ; Central DC coefficient address
_fir_dc_coeff:  .space 2        ;

; Reserve Y Memory for Denoiser and Autonotch Filters and Variables
	.section yweiner, bss, ymemory, align(512)  
  
; Denoiser Coefficient
	.global	_den_coeff	; Coefficients start
_den_coeff:	.space 256	; Storage for coefficients in bytes
	.global	_den_coeff_end	; Coefficients end (required for adaptive changes)
	.global	_den_n		; Kernel length
	.global _den_m		; FIR loop count
	.global	_den_coeff_ptr	; Coeff pointer
_den_coeff_end:	.space 2	;
_den_coeff_ptr:	.space 2	;
_den_n:		.space 2	;
_den_m:		.space 2	;		

; Autonotcher Coefficient
    .global	_aut_coeff	; Coefficients start
_aut_coeff:	.space 128	; Storage for coefficients in bytes
	.global	_aut_coeff_end	; Coefficients end (required for adaptive changes)
	.global	_aut_n		; Kernel length
	.global	_aut_coeff_ptr	; Coeff pointer
_aut_coeff_end:	.space 2	;
_aut_n:		.space 2	;	
_aut_coeff_ptr:	.space 2	;	

;
;*********************************************************************************************
; Reserve X Memory for Delay Line and Variables
	.section xfir, bss, xmemory, align(2048)

; Main FIR Delay Line
	.global _fir_delay			; Filter delay line start 
_fir_delay:		.space 1002		; Delay line in bytes
; Main FIR Variables
	.global	_fir_delay_end		; End of delay line
	.global _fir_delay_ptr		; Delay line pointer
	.global	_fir_in         	; Right FIR input word
	.global _fir_out       		; Right FIR output word
_fir_delay_end:	.space 	2		; Storage for FIR Variables
_fir_delay_ptr:	.space	2
_fir_in:        .space	2
_fir_out:       .space	2
;*********************************************************************************************
;
; Storage in X Memory for Denoiser, Autonotch and Binaural delay lines
	.global _den_delay		; Denoiser delay line start
_den_delay:	.space 128		; Delay line in bytes
;	.global _den_dec_delay		; Denoiser decorrelation delay 
;_den_dec_delay:	.space 128		; Delay line in bytes
	
; Autonotcher FIR Filter
	.global _aut_delay		; Autonotcher delay line start
_aut_delay:	.space 128		; Delay line in bytes
	.global	_aut_dec_delay		; Autontocher input delay line
_aut_dec_delay: .space 128		; Delay line in bytes
 
; Binaural Delay
	.global _bin_delay		; Binaural delay line 
_bin_delay:       .space 200		; Length in bytes	       
;*********************************************************************************************
; Data Tables Used By Assembler Functions
;  
; Sine coefficient table
;	.global _sin_coeff		;
;_sin_coeff:	.space 16		; 
; i reciprocal table	
;	.global _i_reciprocal		;
;_i_reciprocal:	.space 500		;
; window table
	.global _window			;
_window:	.space 500		;	
;	
;*********************************************************************************************        
             
	.section xosc, bss, xmemory, align(1024)

; De-noiser FIR Filter
;	.global _den_delay			; Denoiser delay line start
;_den_delay:	.space 256			; Delay line in bytes
; De-noiser Variables
	.global	_den_delay_end	    ; Delay line end
	.global _den_delay_ptr	    ; Delay line pointer
	.global	_denoise_in	    ; Denoiser input
	.global _den_FIR_out	    ; Denoise filter output for scaling
	.global	_denoise_out	    ; Denoiser output
	.global _den_last           ; Previous input sample for premephasis
	.global _den_6dB            ; Coarse output gain in 6dB steps
	.global _den_1dB            ; Fine output gain in 1dB steps
	.global	_den_dec_end	    ; Decorrelation delay end
	.global _den_dec_ptr	    ; Decorrelation delay pointer
_den_delay_end:	.space	2	    ;
_den_delay_ptr:	.space	2	    ;
_denoise_in:	.space	2	    ; Denoiser input
_den_FIR_out:	.space	2	    ; Denoise filter output for scaling	
_denoise_out:	.space  2	    ; Denoiser output
_den_last:      .space  2	    ;
_den_6dB:       .space  2	    ; Coarse output gain in 6dB steps
_den_1dB:       .space  2	    ; Fine output gain in 1dB steps
_den_dec_end:	.space 2	    ; Decorrelation delay end
_den_dec_ptr:	.space 2	    ; Decorrelation delay pointer       
	.global	_den_beta	    ; Beta value
	.global _den_beta_norm	    ; Normalised beta
	.global _den_e_beta         ; error beta
	.global _den_decay	    ; Decay factor
        .global _den_num            ; Numerator normalised by taps
_den_beta:	.space 2	    ;
_den_beta_norm: .space 2	    ;
_den_e_beta:    .space 2	    ;
_den_decay:	.space 2	    ;
_den_num:       .space 2	    ;
	.global	_den_input_delay    ; Denoiser input delay line
_den_input_delay: .space 2	    ; Delay line in bytes
	.global _denoise_in_dlyd    ; Delayed input for eror calculation
_denoise_in_dlyd: .space 2	    ;  

; Autonotcher FIR Filter
;	.global _aut_delay			; Autonotcher delay line start
;_aut_delay:	.space 128			; Delay line in bytes
; Autonotcher Variables
	.global	_aut_delay_end	    ; Delay line end
	.global _aut_delay_ptr	    ; Delay line pointer
	.global	_autonotch_in	    ; Autonotcher input
	.global	_autonotch_out	    ; Autonotchter output
	.global _aut_FIR_in	    ; Input to FIR delay line
	.global _aut_FIR_out	    ; FIR output
	.global _aut_input_dlyd	    ; Delayed input
_aut_delay_end:	.space	2	    ;
_aut_delay_ptr:	.space	2	    ;
_autonotch_in:	.space	2	    ; Autonotcher input
_autonotch_out:	.space  2	    ; Autonotcher output
_aut_FIR_in:	.space	2	    ;
_aut_FIR_out:	.space  2	    ;	
_aut_input_dlyd:.space  2	    ;	
	.global	_aut_beta	    ; Beta value
	.global _aut_beta_norm	    ; Normalised beta
	.global _aut_e_beta	    ; error beta
	.global _aut_decay	    ; Decay factor
	.global _aut_m		    ; M
	.global _aut_num	    ; Normalised numerator		
_aut_beta:	.space 2	    ;
_aut_beta_norm:	.space 2	    ;	
_aut_e_beta:	.space 2	    ;
_aut_decay:	.space 2	    ;
_aut_m:		.space 2	    ;
_aut_num:	.space 2	    ;		
	.global	_aut_dec_end	    ; Delay line end
	.global	_aut_dec_ptr	    ; Delay line pointer
	.global	_aut_dec_n	    ; Delay line length
	.global _autonotch_in_dlyd  ; Delayed input
_aut_dec_end:	.space 2	    ;
_aut_dec_ptr:	.space 2	    ;
_aut_dec_n:	.space 2	    ;
_autonotch_in_dlyd:	.space 2	    ;	
; Tone Generator
    .global _phase_inc          ; Tone phase increment
    .global _phase_word         ; Tone phase accumulator
    .global _tone_gain          ; Output level
    .global _tone_shaping       ; Shaping factor
    .global _tone_out           ; Tone output signal
    .global _tone_gain_factor	; Tone gain
_phase_inc:         .space 2    ;
_phase_word:        .space 2    ;
_tone_gain:         .space 2    ;
_tone_shaping:      .space 2    ;
_tone_out:          .space 2    ;
_tone_gain_factor:  .space 2	;	  
; Binaural Delay
    .global _bin_delay_end      ; Binaural delay line end
    .global _bin_delay_ptr      ; Binaural delay line pointer
    .global _bin_n		; Binaural delay adjustable tap
    .global _binaural_IN	; Binaural input
    .global _right_gain_factor	; Right binaural channel gain
    .global _left_gain_factor	; Left binaural channel gain
    .global _binaural_R		; Delayed right output 
    .global _binaural_L	        ; Delayed left output   
_bin_delay_end:     .space 2    ;
_bin_delay_ptr:     .space 2    ;
_bin_n:		    .space 2    ;     
_binaural_IN:  	    .space 2	;	    
_right_gain_factor: .space 2	;	 
_left_gain_factor:  .space 2	;
_binaural_R:	    .space 2    ;
_binaural_L:	    .space 2    ;  
;
;*********************************************************************************************
;
; General Variables

    .global _sig_pwr		; Input signal power
_sig_pwr:	    .space 2	;   
;
;*********************************************************************************************
;
; Filter builder
    .global _fs                 ; Sampling rate
    .global _fc                 ; Low Pass corner frequency
    .global _Fc                 ; Fractional frequency fc/fs
    .global _scale              ; Scaling factor where DC term greater than unity
    .global _angle_rads         ; Sine generator input in radians
    .global _2PiFc              ; Storage for 2*Pi*Fc
    .global _index_pointer      ; Pointer for 1/[I-M/2] reciprocal table
    .global _A_coeff_end      ; End of coefficients
    .global _A_dc_coeff       ; Central DC term
    .global _A_factor		;
    .global _B_coeff_end      ; End of coefficients
    .global _B_dc_coeff       ; Central DC term
    .global _B_factor		;
    .global _C_coeff_end      ; End of coefficients
    .global _C_dc_coeff       ; Central DC term
_fs:            .space 2        ;
_fc:            .space 2        ;
_Fc:            .space 2        ;
_scale:         .space 2        ;
_angle_rads:    .space 2        ;
_2PiFc:         .space 2        ;
_index_pointer: .space 2        ;
_A_coeff_end: .space 2        ;
_A_dc_coeff:  .space 2        ;
_A_factor:    .space 2	;
_B_coeff_end: .space 2        ;
_B_dc_coeff:  .space 2        ;
_B_factor:    .space 2	;
_C_coeff_end: .space 2        ;
_C_dc_coeff:  .space 2        ;      
 
;*********************************************************************************************
;
; Sine coefficient table
	.global _sin_coeff		;
_sin_coeff:	.space 16		; 
;*********************************************************************************************
;
; Tone taper table
	.global _tone_taper		;
_taper_table:	.space 160		; 	
	
