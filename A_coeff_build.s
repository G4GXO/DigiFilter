; DigiFilter
;
; (c)Ron Tyalor, G4GXO
;
; Low Pass Filter Builder
;
; Assembler routine to generate a low pass kernel from a cut off frequency
;
; Things to do
;
; Files Used
; w0,w1,w2,w3,w4,w5,w6,w7,w8,w9,
;
	.text
		.global _A_coeff_build      ; Make the program visible to the C routines
_A_coeff_build:
    
; Save critical registers

    push    MODCON                          ; save context of MODCON
    push    XMODSRT                         ; save context of YMODSRT
    push    XMODEND                         ; save context of YMODEND
    push    CORCON                          ; save context of CORCON
    push    PSVPAG                          ; save context of PSVPAG
    push    RCOUNT                          ; save repeat counter

    push    w8                              ; save context of w8
    push    w9                              ; save context of w9	

;*********************************************************************************************
;
; Prepare DSP Core and Modulo Addressing for filter delay lines and coefficients
;
;*********************************************************************************************
;
; Set core mode for DSP calculations
;
       mov     #0x00B2,w8     			; Accumulator A, B and Data Space write Saturation enabled
       mov     w8, CORCON                      	; signed fractional multiplies
       
; Step 1 Compute fc as a fraction of sampling rate fs
;
; 32bit by 16 bit unsigned divide with fractional output. The corner frequency is loaded into w5 and w4 is cleared
; to form a 32bit value of fc*65536. The 32 bit by 16bit unsigned divide produces a 16 bit result which is the
; fractional value of fc/fs is stored to w0.

    mov _fs,w3                                  ; Store sampling rate to w3
    clr w4                                      ; Clear w4
    mov _fc,w5                                  ; Copy corner frequency to w5 which is treated as the upper word
    repeat #17                                  ;
    div.ud  w4,w3                               ; Divide w5:w4 by w3, result in w0, remainder in w1
    mov #0x7fff,w2                              ; Test 16 bit remainder for rounding
    cpslt, w1,w2                                ; If remainder less than 0x7fff skip increment
    inc w0,w0                                   ; Remainder greater, increment result
    lsr w0,w0                                   ; Shift right one place
    mov w0, _Fc                                 ; Store fractional value to Fc for kernel calculations

; Step 2 Calculate central DC term
;
; The central DC coefficient of the kernel is given by kc=2*Pi*fc/fs where fc is the corner frequency and
; fs is the sampling frequency. Replacing fc/fs with the fractional term gives kc=2*Pi*Fc. For use with
; fractional maths, no term in the kernel can exceed 0x7fff, the central term will exceed 0x7fff for all
; values of Fc that are greater than 1/(2*Pi) therefore in these cases we need to hold kc at 0x7fff and
; apply a scaling factor to the remaining kernel values to keep their relative values intact. If kc is set
; to 0x7fff then the remaining factors must be scaled by 1/(2*Pi*Fc). The scaling factor is calculated by
; dividing the pre-computed value 1/(2*Pi) by Fc. 1/(2*Pi)= 0.15915494 = 0x145f
;
;
    mov _A_dc_coeff,w9                          ; Load position of DC term into w9
    mov #0x7fff,w1                              ; Pre load scaling with 1
    mov w1, _scale                              ; for cases where Fc<1/(2*Pi)
    mov #0x145f, w1                             ; Load 1/(2*Pi) into w1
    cpsgt   w0,w1                               ; Compare Fc with 1/(2*Pi), skip if Fc>1/(2*Pi)
    goto    A_dc_coeff                          ; Fc<=1/(2*Pi), calculate coefficient
A_dc_scaling:                                   ; Fc>1/(2*Pi), set unity DC term and compute scaling factor for remaining terms
    mov #0x7fff,w0
    mov w0, [w9++]                              ; Write 0x7fff (1) to DC term and post increment
    mov _Fc,w2                                  ; Copy Fc to w2
    repeat #17                                  ; Repeat divf 18 times
    divf    w1,w2                               ; Fractional divide of 1/(2*Pi) by Fc, result in w0
    mov #0x7fff,w2                              ; Test 16 bit remainder for rounding
    cpslt   w1,w2                               ; If remainder less than 0x7fff skip increment
    inc w0,w0                                   ; Remainder greater, increment result
    mov w0, _scale                              ; Store fractional value to scaling for coefficient adjustment
    goto A_coeff_calc                           ; Jump to coefficient calculator

A_dc_coeff:                                     ; DC coefficient is 2*Pi*Fc
    ; 2*Pi=6.283185, multiply by 32768 and convert to hex, 2*Pi*32768=3243f
    ; Two words 0x0003, and 0x243f, multiply each by Fc, sum the LSW of the
    ; product of 0x0006 and the MSW for the product of 0x243f to get
    ; the 16bit value of 2*Pi*Fc
    mov _Fc,w0                                  ; Fc
    mov #0x0006,w4                              ; MSW of 2*Pi
    mul.uu  w0,w4,w2                            ; Result in w2:w3
    mov #0x487e,w4                              ; LSW of 2*Pi
    mul.uu  w0,w4,w6                            ; Result in w6:w7
    add w2,w7,w1                                ; Add LSW of x1 to MSW of x2 store result to w1
    mov w1,[w9++]                               ; Load result into DC term and post increment

; Step 3 Coefficent Calculator
;
;
A_coeff_calc:
;*********************************************************************************************
; Generate and scale a symmetrical 501 tap half kernel for each coefficient in the range
; DC term+1 to N according to the formula;
;
; H[i]=scale*sin(2*Pi*Fc*[I-M/2])/[I-M/2]
;
; Where
;
; 2*Pi = 0xffff for the sine generator
; M=501-1, M/2=250
; I=251..500
; [I-M/2] replaced by i=1..250
; k=1/[I-M/2] stored in table
;
; DC term calculations leave w9 pointing to first tap after DC term
;
; Register usage
;    w0	Data transfer
;    w1 Reciprocal Table index
;    w2 Angle in radians
;    w3 Calculations
;    w4 2*Pi*Fc*i
;    w5	Sin coefficients
;    w6 Power raising    
;    w7 Tap counter
;    w8 Sin coefficient pointer
;    w9 Kernel coefficient pointer
;    
;*********************************************************************************************
    
; Calculate 2*Pi*Fc
mov _Fc, w4                     ; Copy Fc to w4
mov #0xffff,w5,                 ; load w5 with 0xffff (2*Pi)
mul.uu w4,w5,w2                 ; Multiply, overlfow into w3
sl w2,w2			; x2
rlc w3,w3			;
btsc w2, #15                    ; Rounding, test the ms bit of w2
inc w3,w3                       ; if set , set the ls bit of w3   
inc w3, w3
mov w3, _2PiFc                  ; Save upper 16 bits to 2PiFc
    
; Enable Program Space Visibility (PSV)
;bset CORCON,#2			; Set PSV bit
;mov #psvpage(i_reciprocal),w0	; Set PSV Page to the page containing _i_reciprocal    
;mov w0,PSVPAG			;
;mov #psvoffset(i_reciprocal),w1	; Make a pointer to data in PSV window    

mov #_i_reciprocal, w1          ; Load start of reciprocal table into w1
       
; Calculation loop start
mov #0x0001, w7                 ; Assign w7 as i counter, set to zero in preparation for first increment
A_coeff_loop:			; Software loop to avoid nested do statement with Sine Gen
    mov _2PiFc, w4              ; Put 2*Pi*Fc into w4
    mul.uu w4,w7,w2             ; 2*Pi*Fc*i
                                ; Angle range 0..2Pi runs from 0x0000..0xffff, use LSW in w2      
    inc w7, w7                  ; Increment tap counter
    mov #252,w0
    cpslt w7,w0
    bra A_coeff_end
   
;*********************************************************************************************
; SINE GENERATOR
;
; Calculate sine(x)
;
; Maclaurin Series sine approximation
;
; sin(x)=a*x + b*x^2 + c*x^3 + d*x^4 + e*x^5
;
; Phase input in 1.15, coefficients scales to 4.12, output in 1.15. Calculation operates
; on first quadrant and is reflected and mirrored into the upper three quadrants determined
; by the states of the upper two bits of the phase word.
;
; As memory is not constrained in this application, the sine generator routine is duplicated
; to save several instruction through calls to a common routine.
;
; Input 	- Angle in radians where 2*Pi=0xffff
; Output 	- Sine value in w0
;
;*********************************************************************************************
; Sine Calculator
; Modify angle so that it can be computed as a first quadrant value
;
; Set core mode for DSP calculations
;
;       mov     #0x00F0,w8     			        ; Accumulator A, B and Data Space write Saturation enabled
       ;mov      #0x0000,w8
;       mov     w8, CORCON                      	;
;
; Sine Generator
; Modify phase so that it can be computed as a first quadrant value
    mov w2, _angle_rads                         ; 1 Load angle
    btsc _angle_rads, #14                       ; 2 Test phase quadrant bit
    neg	w2, w2                                  ; 3 In quadrants 2 or 3, complement phase
    mov	#0x7FFF, w0                             ; 4 Mask off sign bit
    and	w0, w2, w4                              ; 5 Save to w4
; Compute sine polynomial
    mov	#_sin_coeff,w8                          ; 6 Point to start of sine coeff table
    clr	A,[w8]+=2,w5                            ; 7 Clear ACCA and load first sine coeff into w5, post increment coeff address  
    mov	w4, w6                                  ; 8 Copy phase word to w6 for power raising
    mac	w4*w5, A, [w8]+=2, w5                   ; 9 First multiply of phase (w4) and coefficient (w5), prefetch next sine coeff to w5
    do	#3, A_sin_end                           ; 10 Loop 4 times to calculate remaining polynomial terms
	mpy w4*w6, B                            ; 20 Raise power of phase factor
	sac.r	B, w6                           ; 21 Store rounded result to w6 for next polynomial
	mac	w5*w6, A, [w8]+=2, w5           ; 22 Multiply raised phase (w6) and coefficient (w5), prefetch next sine coeff to w5
A_sin_end:
    nop     
    sac.r A, #-3,w0				; 23 Multiply by 8 to account for coefficient scaling and store rounded ACCA to w0
    btsc _angle_rads, #15                       ; 24 Test the msb of the current phase word, if it is set then
    neg	w0, w0                                  ; 25 phase >180 degrees and output is negative, save to w0
    mov w0, w4                                  ; For unknown reason not possible to go direct to w4 in earlier instructions!    
; Multiply by 1/[I-M/2]
    mov [w1++],w5                               ; Get reciprocal value, save to w5 and increment pointer
    mpy w4*w5, A                                ; Mutliply by fractional reciprocal
    sac.r A,  w0                                ; Round and save to w0
    mov w0, w4                                  ; Copy to w4
    
; Scale coefficient
    mov _scale,w5                               ; Load scale factor into w5
    mpy w4*w5, A                                ; Multiply by scaling factor
    sac.r A,  w0                                ; Round and save to w0
    mov w0, [w9++]                              ; Write to [w9] and post increment address
    bra A_coeff_loop                            ; Back round again
A_coeff_end:                                    ; Loop exit point
    nop
 
; Step 4 Windowing
;
; Multiply upper coefficients (DC+1=251 to 501) by window function
;    goto A_win_loop;************************
; Enable Program Space Visibility (PSV)
;    bset  CORCON,#2			; Set PSV bit
;    mov #psvpage(window),w0		; Set PSV Page to the page containing _window    
;    mov w0,PSVPAG			;
;    mov #psvoffset(window),w8		; Make a pointer to data in PSV window       
  
    mov #_window,w8                             ; Load start of window table to w8
    mov [w8],w5                                 ; Copy window value at [w8] to w5
    mov _A_dc_coeff,w2                          ; Load central DC point of half kernel to w2
    inc2 w2,w2                                  ; Increment to DC+1 (word address hence inc2)   
    do #249,A_win_loop                          ; Multiply upper coefficients by half window
    mov [w2],w4                                 ; Get coefficent value and place in w4   
    mpy w4*w5,A,[w8]+=2,w5                      ; Multiply coefficient by window value, prefetch next window value
    sac.r A,w0                                  ; Round result and place in w0
    mov w0,[w2++]                               ; Place result in w2 and post increment to point to next value
A_win_loop:
    nop

; Step 5 Reflect
;
; Reflect upper coefficients into lower range to form a kernel symmetrical about the DC point
    mov #_A_coeff,w1                            ; Load start position into w1
    mov _A_coeff_end,w2                         ; Load end position into w2
    do #249, A_reflect_end
    mov [w2--], w0                              ; Copy upper axis coefficient
    mov w0,[w1++]                               ; Write into lower axis
A_reflect_end:
    nop
 
; Step 6 Normalisation
;
; Sum all coefficients and multiply by a variable scaling factor until sum is near unity. An interative
; approach is used starting at half amplitude and progressively working towards near unity. The coefficients
; are multiplied by a test gain value and summed. If the accumulator overflows then the gain is too high
; and the gain factor is incrementally reduced for the next pass. If the accumulator does not overflow
; then the result is 0x7fff or less and it is compared against unity gain value 0x7fff. If it is equal 
; unity gain exists and the loop ends. If it is less then the gain is increased by and increment and
; the process is repeated. The gain increments satr at half the mid point values and halve with each
; pass. If the gain increment reaches zero without a match to 0x7fff (as in most cases) then the factor
; will be close to 0x7fff and the loop ends. The gain increment start value of 0x2000 supports 14 test
; cycles before reaching zero and ending the iteration.
;
    ; Sum coefficients
    clr A                               ; Clear accumulator A
    mov #_A_coeff,w0                    ; Load start of B coefficients into w0
    do #500, A_sum                      ; Summing loop
    add [w0++],A                        ; Sum address content into A and post increment
A_sum:
    nop                                 ; Coefficient sum in A
    btsc ACCAU,#7                       ; Is Acc A negative?
    neg A                               ; Yes, negate to make positive
    nop
    ; Normalise accumulator sum
    mov #ACCAH, w5                      ; w5 points to ACCAH
    fbcl [++w5], w0                     ; Extract exponent for right shift
    add.b w0, #15, w0                   ; Adjust the sign for right shift
    and #0x003f,w0                      ; Mask lower 6 bits, result in w0
A_Shift_Acc:                            ; Shift accumulator
    inc w0,w2                           ; Increment and save to w2
    sftac A, w2                         ; Shift ACCA by w2 to normalize to a 16 bit factional value
    mov #0x7fff,w1
    lsr w1,w2,w1                        ; Shift 0x7fff right by same number of places to form dividend
    mov ACCAH,w2                        ; Copy normalised sum to w2
    repeat #17                          ; Set up divide
    divf    w1,w2                       ; Fractional divide to get reciprocal, result in w0
    nop
    ; Apply gain factor to each coefficient
A_gain:
    mov w0,w5                           ; Store gain factor to w5
    mov #_A_coeff, w1                   ; Put start address of coefficients into w1
    do #500, A_gain_exit                ; Loop through all coefficients
    mov [w1],w4                         ; Get coefficient
	mpy w4*w5,A                         ; Multiply and store to Acc A and prefectch next coefficient
	sac.r A, w0                         ; Round result and copy to w0
	mov w0,[w1++]                       ; Write to [w1] and post increment
A_gain_exit:
    nop

;******************************************************************************************
; TEST NORMALISATION RESULT
;
;    mov #_A_coeff,w10	                            ; Load start of coefficients into w10
;    clr A                                           ; Clear Acc 
;    do #500 , Add_coeff                             ; Sum all coefficients
;    add [w10++], A
;Add_coeff:
;    nop

;A_test:
;    mov #_A_coeff,w10	                ; Load start of coefficients into w10
;    clr A                               ; Clear Acc
;    do #500 , A_sum_coeff               ; Sum all coefficients
;    add [w10++], A
;A_sum_coeff:
;    nop
;    sac A,w0
;    btss w0,#15
;     goto A_exit

;A_rescale:
;    mov 0x7998                          ; 0.95 rescale value
;    goto A_gain                         ; Apply scaling factor

;*******************************************************************************************   

;Builder_exit:
A_exit:
;
;*********************************************************************************************
;
;
; Restore critical registers
;
    pop     w9                              ; restore context of w9
    pop     w8                              ; restore context of w8

    pop     RCOUNT                          ; restore repeat counter
    pop     PSVPAG                          ; restore context of PSVPAG
    pop     CORCON                          ; restore context of CORCON
    pop     XMODEND                         ; restore context of YMODEND
    pop     XMODSRT                         ; restore context of YMODSRT
    pop     MODCON                          ; restore context of MODCON
;    nop
    return

.end




