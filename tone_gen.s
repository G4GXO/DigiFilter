; DigiFilter
;
; (c)Ron Taylor, G4GXO
;
;*********************************************************************************************
; SINGLE TONE GENERATOR
;
; Calculate AF sine wave and apply gain control.
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
; Input 	- Phase increment passed in w0
; Output 	- Sine value in w0
;
; Registers Used
; w0,w1,w4,w5,w6,w8,
;	
;*********************************************************************************************
	.text
	.global _tone_gen
_tone_gen:
;
; Save critical registers
;
    push    MODCON                      ; save context of MODCON
    push    XMODSRT                     ; save context of YMODSRT
    push    XMODEND                     ; save context of YMODEND
    push    CORCON                      ; save context of CORCON
    push    PSVPAG                      ; save context of PSVPAG
    push    RCOUNT			; save repeat counter
                       
;    push    w0
;    push    w1
;    push    w4
;    push    w5
;    push    w6
    push    w8				; Save contect of w8
;    push    w9				; Save contect of w9
;    push    w10				; Save contect of w10
;    push    w11				; Save contect of w11
;    push    w12				; Save contect of w12
;    push    w13				; Save contect of w13
;    push    w14				; Save contect of w14
;    push    w15				; Save contect of w15
    
;
; Set core mode for DSP calculations
;
    mov     #0x00F0,w2     		; Accumulator A, B and Data Space write Saturation enabled
    mov     w2, CORCON			;
    
;    mov	    #0x8ff8,w2
;    mov	    w2,MODCON			; Enable modulo addressing for sine coefficients
;
; Oscillator
; Add phase increment passed in w0 to accumulator value
    mov _phase_inc,w0			; 1 Copy phase increment to w0
    mov _phase_word,w1	                ; 2 Copy phase accumulator word to w1    
    add w0,w1,w1                        ; 3 Add phase increment to accumulator
    mov w1,_phase_word			; 4 Copy back to phase accumulator
; Modify phase so that it can be computed as a first quadrant value
    btsc _phase_word, #14       	; 5 Test phase quadrant bit
    neg	w1, w1				; 6 In quadrants 2 or 3, complement phase
    mov	#0x7FFF, w0			; 7 Mask off sign bit
    and	w0, w1, w4			; 8 Save to w4    
; Compute sine polynomial (5 terms)      
    mov	#_sin_coeff,w8			; 6 Point to start of sine coeff table
    clr	A, [w8]+=2, w5			; 7 Clear ACCA and load first sine coeff into w5, post increment coeff address
    mov	w4, w6				; 8 Copy phase word to w6 for power raising
    mac	w4*w5, A, [w8]+=2, w5		; 9 First multiply of phase (w4) and coefficient (w5), prefetch next sine coeff to w5    
    do	#3, sine_end                    ; 10 Loop 4 times to calculate remaining polynomial terms    
	mpy w4*w6, B			; 20 Raise power of phase factor
	sac.r	B, w6			; 21 Store rounded result to w6 for next polynomial
	mac	w5*w6, A, [w8]+=2, w5	; 22 Multiply raised phase (w6) and coefficient (w5), prefetch next sine coeff to w5	
sine_end:
    sac.r   A, #-3,w0			; 23 Multiply by 8 to account for coefficient scaling and store rounded ACCA to w4
;    btsc    w4,#15			; Output here should be positive, if rounding errors take the result past 0x7fff 
;    mov #0x7fff, w4			; (negative in Q1.15), then hold at 0x7fff       
    btsc _phase_word, #15		; 24 Test the msb of the current phase word, if it is set then
    neg	w0, w0      			; 25 phase >180 degrees and output is negative, save to w4
    mov w0, w4
    
; Apply tone shaping and gain
    mov _tone_shaping,w5		; Load tone shaping factor into w5
    mpy w4*w5,A                         ; DSP fractional multiply to incorporate left shift
    sac.r A,w4                          ; Round and store to w4
    mov _tone_gain_factor,w5		; Load tone gain factor
    mpy w4*w5,A				; Scale tone amplitude
    sac.r A,w0				; Round and save to w0
    mov w0, _tone_out                   ; Copy to output
;
; Restore critical registers
;
;
tone_exit:    
;    pop	    w15				 ; restore contect of w15
;    pop	    w14				 ; restore contect of w14 
;    pop	    w13				 ; restore contect of w13
;    pop	    w12				 ; restore contect of w12
;    pop	    w11				 ; restore contect of w11
;    pop	    w10				 ; restore contect of w10
;    pop	    w9				 ; restore contect of w9
    pop	    w8                           ; restore context of w8
;    pop    w6                           ; restore context of w6
;    pop    w5                           ; restore context of w5
;    pop    w4                           ; restore context of w4
;    pop    w1
;    pop    w0
    
    pop	    RCOUNT			; restore repeat counter
    pop     PSVPAG                      ; restore context of PSVPAG
    pop     CORCON                      ; restore context of CORCON
    pop     XMODEND                     ; restore context of YMODEND
    pop     XMODSRT                     ; restore context of YMODSRT
    pop     MODCON                      ; restore context of MODCON
    nop

    return
    
.end



