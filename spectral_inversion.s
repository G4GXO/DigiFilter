; DigiFilter
;
; (c)Ron Taylor, G4GXO
;	
; Band Pass Filter Builder
;
; Assembler routine to generate a band pass kernel from two low pass kernels
;
; Things to do
;
; 1. DONE   Prefetch for MAC and CLR not working in Simulator, test in hardware
; 2. DONE   Sharpen normalistion to come closer to unity gain. Issues is with Fc<1/(2*PI)
; 3.        Document program to explain operation.
;
;
; Registers Used
; w0,w1,w2,w3,w4,w5,w8,	
;	
.text
	.global _spectral_inversion       	; Make the program visible to the C routines
_spectral_inversion:

; Save critical registers

	push    MODCON                          ; save context of MODCON
	push    XMODSRT                         ; save context of YMODSRT
	push    XMODEND                         ; save context of YMODEND
	push    CORCON                          ; save context of CORCON
	push    PSVPAG                          ; save context of PSVPAG
	push	RCOUNT				; save repeat counter
 
	push    w8                              ; save context of w8

;*********************************************************************************************
;
; Prepare DSP Core and Modulo Addressing for filter delay lines and coefficients
;
;*********************************************************************************************
;
; Set core mode for DSP calculations
;
       mov     #0x00B0,w8     			; Enable accumulator A Saturation and Data Space write Saturation,
       mov     w8, CORCON                      	; signed fractional multiplies
       
; Band Pass Filter Routine       
;
; Kernel Usage
;
; A	    Low pass lower corner frequency
; B	    Low pass upper corner frequency
; C	    Band pass and spectral inversion
; fir_coeff Working kernel       
;       
; This is a three step process which operates on two low pass kernels to make a band pass kernel.
; The lower corner frequency is set by kernel A, the upper by kernel B.
;
; 1. Convert upper low pass kernel into a high pass kernel by spectrum inversion      
; 2. Add to lower low pass kernel to make a band stop kernel      
; 3. Apply spectrum inversion to convert band stop kernel to band pass kernel
;
; Both upper and lower kernels are preserved such that when only one corner frequency is adjusted
; the other kernel does not need to be re-computed.
      
; Step 1    Convert upper low pass kernel B to high pass by Spectral Inversion and save to
;	    output kernel C.      
    mov #_B_coeff, w2				; Load start of HP coefficients into w2
    mov #_C_coeff, w3				; Load C kernel start into w3
    do #500, Step_1				; Inversion loop
    neg [w2++],[w3++]				; Negate coefficient, store kernel C and increment address
Step_1:						; Loop round
    nop
    mov _C_dc_coeff,w3				; Load position of central DC term into w3
    mov [w3],w0					; Get value and copy to w0 
    mov #0x7fff, w1				; Load w1 with 1
    add w1,w0,w0				; Add 1 to w0 
    mov w0, [w3]				; and store back to central coefficient
    ; Upper kernel is now stored to C as a high pass response
    
; Step 2 Add lower low pass kernel A to upper high pass kernel held in C to make a band stop kernel
    mov #_A_coeff,  w2				; Load A kernel start into w2 
    mov #_C_coeff,  w3				; Load C kernel start into w3    
    ; Loop through each A & C coefficient and place sum in C kernel
    do #500, Step_2				; Summing loop				
    mov [w2++], w4				; Load A coefficient to w4 and post increment
    mov [w3],w5					; Load C coefficient to w5 
    add w4,w5,[w3++]				; Add and save result to C coefficient and post increment
Step_2:
    nop
    ; New kernel in C in now band stop
    
; Step 3 Convert band stop kernel in C to band pass by spectral inversion    
   mov #_C_coeff,w1				; Load C kernel start into w1					 
    do #500, Step_3				; Inversion loop
    neg [w1],[w1++]				; Negate coefficient, store back to location and increment address
Step_3:						; Loop round
    nop
    mov _C_dc_coeff,w1				; Load position of central DC term into w1
    mov [w1],w0					; Get value and copy to w0 
    mov #0x7fff, w2				; Load w2 with 1
    add w2,w0,[w1]				; Add 1 to w0 and save to back to central coefficient in [w1]
    ; C kernel is now band pass

; Step 4 Load new kernel into working coefficients
    mov #_fir_coeff,w1				; Load start of working kernel
    mov #_C_coeff,w2				; Load start of new kernel
    do #500, Step_4				; Step through each kernel tap
    mov [w2++],w0				; Get new coefficient
    mov w0,[w1++]				; Write to working kernel
Step_4:
    nop     
;
;*********************************************************************************************
;
;
; Restore critical registers
;
    pop w8                              ; restore context of w8

    pop	    RCOUNT			; restore repeat counter
    pop     PSVPAG                      ; restore context of PSVPAG
    pop     CORCON                      ; restore context of CORCON
    pop     XMODEND                     ; restore context of YMODEND
    pop     XMODSRT                     ; restore context of YMODSRT
    pop     MODCON                      ; restore context of MODCON
    
    return

.end







