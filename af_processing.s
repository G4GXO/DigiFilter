; DigiFilter
;
; (c)Ron Taylor, G4GXO
;
; Audio Processing Block
;
; Register used
; w0,w1,w2,w3,w4,w5,w6,w7,w8,w10,
;
;*********************************************************************************************
	.text
		.global _af_processing		; Make the program visible to the C routines
_af_processing:
; Save critical registers
    push    MODCON                          ; save context of MODCON
    push    XMODSRT                         ; save context of YMODSRT
    push    XMODEND                         ; save context of YMODEND
    push    CORCON                          ; save context of CORCON
    push    PSVPAG                          ; save context of PSVPAG
    push    RCOUNT                          ; save repeat counter

    push    w8
    push    w10

;*********************************************************************************************
;
; Prepare DSP Core and Modulo Addressing for filter delay lines and coefficients
;
;*******************************************************************************
;
; Set core mode for DSP calculations
;
       mov     #0x00B0,w8     		; Enable accumulator A Super Saturation and Data Space write Saturation,
       mov     w8, CORCON               ; signed fractional multiplies

       mov     #0x8FF8, w8              ; Enable modulo addressing in X-Memory, bit reversing disabled,
       mov     w8, MODCON               ; w8 used for modulo addressing, Y modulo addressing disabled
       
;*******************************************************************************       
; Signal Power Indicator
;
;*******************************************************************************       
	mov _sig_pwr,w4			; Copy signal power to w4
	lac w4,A			; Copy to accumulator A
	mov _fir_in,w4			; Copy current sample to w4
	mac w4*w4,A			; Square and add to running signal power in A
	sac.r A, #1, w4			; Round, halve and save instantaneous signal power back to w4
	mov #0x7f00, w5			; Load a decay factor into w5
	mpy w4*w5,A			; Decay result and save to A
	sac.r A, w4			; Round and save to w4
	mov w4, _sig_pwr		; Save to signal power       
       
;*******************************************************************************
;
; FIR Filter
;
; Universal FIR Filter
;
;*******************************************************************************  

	mov _fir_in, w0                 ; Get input and load into w0
	mov #_fir_delay,w8              ; Load delay line start address into w8
	mov w8, XMODSRT                 ; XMODSRT = start address of delay line
	mov _fir_delay_end, w8		; Load delay line end address
	mov w8, XMODEND                 ; XMODEND = end address of delay line
	    mov _fir_m,w1		; Load loop counter with taps-1
	    mov _fir_delay_ptr, w8	; Load delay line pointer into w8
	    mov w0, [w8++]              ; Move new sample into position pointed to by w8
	    mov w8, _fir_delay_ptr	; Update delay line pointer
	    mov #_fir_coeff, w10        ; Point to start of coefficients
	    clr A, [w8]+=2, w5, [w10]+=2, w6; Clear Acc A, [w8]=> w5, [w10]=>w6
	    repeat w1                   ; Repeat MAC instruction w1 times = Number of taps - 1
	    mac w5*w6, A, [w8]+=2, w5, [w10]+=2, w6
	    sac.r a,w0			; Round and save to w0
	    mov w0, _fir_out            ; Copy output word to filter output

;*******************************************************************************
;
; NLMS Denoiser
;
; LMS noise reduction routine based upon an FIR filter with kernel tap values 
; driven to reduce the noise content at the summing point of the filter output 
; and the source signal. The LMS error between the input signal and the filter 
; output is used to drive the filter tap algotrithm.
;
; The decay factor is applied to all coefficients within the coefficient updating 
; process. The overall algorithm is;
;
; h(n) = decay*h(n) * beta_norm * error * x(n)
;
; The direction used to read and write the coefficients in the FIR filter and 
; adjustment routines is not important provided that it is the same for both 
; routines. In this implementation, coefficients are operated on in the same 
; direction as delay line values, (see MAC instruction).
;
; Developed from the ADSP-21xx scheme employed in the DSP-10 transceiver by 
; Bob Larkin which in turn is based upon a Sept 1996 QEX article by 
; Johan Forrer, KC7WW.
;
; Normalisation of beta factor to signal power
;
; The purpose of normalisation is to vary the correction step size according to
; signal power such that with high signal power the step is reduced and with low
; signal power the step is increased. The algorithm used is;
;
; beta_norm = beta/(d + Pe) where Pe is the delay line signal power estimate
; and d is a small value added to prevent a divide by zero error.
;	    
; During initialisation and for cases where Pe is zero, the value of 1/(d+Pe)
; will cause a fractional division error, as the result must be <1.
; To overcome this the numerator and denominator are compared before division
; and if the numerator is greater or equal to the denominator the division is
; is bypassed and the unmodified beta is used, implying a division result of 1.	    
;
; The delay line power estimate Pe is calcuated as a Euclidean Norm^2 by
; squaring and summing the delay line sample values.
;
; ||x(n)||^2= x(0)^2 + x(1)^2 + ... x(n-1)^2
;
; The approach used here is a compromise to simplify normalisation using
; fractional maths. A normalisation factor is calculated which is used to scale
; the fixed den_beta value. The factor is calculated by setting a threshold
; value above which normalisation takes place. The threshold forms the numerator
; in the normalisation calculation and the power estimate of the FIR delay line
; the denominator such that for a fractional divides;
;
;			    numerator<denominator
;
; beta_norm=beta*(1/||x(n)||^2) for Pe>1
;	    	    
;*******************************************************************************
	    
Denoiser:
    
; Decay entire filter vector by decay constant
	mov #_den_coeff,w10			; Point to start of coefficients
	mov _den_decay,w4			; Load decay factor into w4
	mov _den_m,w1				; Load filter length-1 into w1
	do w1,den_decay_loop			; Decay each coefficient
	mov [w10],w5				; Copy coefficient to w5
	mpy w4*w5,A				; Decay coefficient	
den_decay_loop: sac.r A,[w10++]			; Round and save back to vector	

;-------------------------------------------------------------------------------    
; Decorrelation delay
;
; A delayed version of the input signal is used in the error calculation. The 
; delay is set to be slightly less than the filter delay to correlate the audio
; but to decorrelate the noise. For the denoiser, the delay of the FIR filter is
; enough so no additional delay mechansim is required.    
;-------------------------------------------------------------------------------
    
;-------------------------------------------------------------------------------    
; Prepare Denoise FIR filter pointer, load new sample and gain scale output
;-------------------------------------------------------------------------------
	mov	_denoise_in,w0			; Load sample into delay line	
	mov     #_den_delay, w8			; Load delay line start address into w8
        mov     w8, XMODSRT			; XMODSRT = start address of delay line
	mov	_den_delay_end, w8		; Load delay line end address
        mov     w8, XMODEND			; XMODEND = end address of delay line
	    mov	_den_m, w1			; Load loop counter with number of taps-1 (M)	
	    mov	_den_delay_ptr, w8		; Load de-noise delay line pointer into w8
	    mov	w0, [w8++]			; Move new sample into position pointed to by w8 and post increment
	    mov	w8, _den_delay_ptr		; Update delay line pointer   	    	    
	    mov	#_den_coeff, w10		; Coefficient start to w10
	    clr	A, [w8]+=2, w5, [w10]+=2, w6	; Clear MAC Accumulator A, load w5, w6 with initial values and increment addresses
	    repeat w1				; Repeat MAC instruction w1+1 times
	    mac w5*w6,A,[w8]+=2, w5,[w10]+=2,w6 ; Data in X memory, Coefficients in Y memory
	    sac.r A,w0				; Save accumulator to w0
	    mov	w0, _den_FIR_out		; Save to den_fir_out

den_out:    ; Apply output gain scaling to accumulator	
	    mov _den_6dB, w1                    ; Load negative (left shift) value into w1  
	    sftac   A,w1                        ; Left shift by 6dB integer	
	    sac.r A,w4				; Store accumulator A to w4	    
	    mov _den_1dB,w5                     ; Copy 1dB factor to w5
            mpy w4*w5,A                         ; Multiply output by gain factor and save to A
	    sac.r A,w0				; Round and save to w0, +6dB gain
	    mov w0, _denoise_out                ; Place the result in denoise_out        
;-------------------------------------------------------------------------------	    
; Normalisation of beta
;
; Calculate beta normalisation factor as fractional value of 1/||xn||^2, if
; numerator is greater than divisor, use the unmodified beta. For this instance,
; pre-populate _den_beta_norm with beta before normalisation. If the numerator
; is less than the divisor, it will be overwritten with the normalised value.
;-------------------------------------------------------------------------------
den_normalise:	    
	mov _den_beta,w2		; Load beta
	mov w2,_den_beta_norm		; Copy to normalised beta		    
	   
; Use the MAC instruction to estimate power in delay line by calculating the
; Euclidean Norm squared, (sum of squares).
;
	mov #_den_delay, w8		; Load delay line start address into w8
	mov _den_m,w1			; Load M into repeat counter w1
	clr A,[w8]+=2,w4		; Clear accumulator & prefetch first value
	repeat w1			; Set up tap counter
	mac w4*w4, A, [w8]+=2,w4	; Square, accummulate and prefetch next
;-------------------------------------------------------------------------------	
; The result may cause an accumulator overflow, (unless the power held int the 
; delay line is <1) and so the accumulator needs to be normalised to a fractional
; value. The numerator of the subsequent division is also reduced by the same
; amount to preserve the value of the reciptocal. If the power is <1 then 
; normalisation is skipped and the value of den_beta, which is preloaded into 
; den_beta_norm, is used.
;-------------------------------------------------------------------------------
	sftac A,#5			; Shift accumulator to clear any overflow	
	sac.r A,w3			; Round and save to w3
	add #0x000f,w3			; Add a small constant
	mov #0x02ff,w2			; Load numerator (0x7fff>>5)
	cpsgt w3,w2			; Check that divisor is greater than numerator
	bra den_calc_beta		; Low signal power, use den_beta
	repeat #17			; Load repeat counter
	divf w2,w3			; Fractional divide w2/w3
	mov w0,w4			; Copy result to w4
	mov _den_beta,w5		; Load den_beta into w5
	mpy w4*w5,A			; Multiply it by fractional result
	sac.r A, w0			; Round and save to w0
	mov w0,_den_beta_norm		; Copy to _den_beta_norm
;-------------------------------------------------------------------------------
; Calculate difference between FIR out and denoise_in to form error signal, 
; multiply this by beta to create e_beta.
;-------------------------------------------------------------------------------
den_calc_beta:
	mov _denoise_in,w2
	mov _den_FIR_out,w0             ; Place FIR output into w0
	sub w2,w0,w4			; Subtract FIR output from denoise input and save to w4, e=xin-FIR_out
	mov _den_beta_norm,w5		; Copy beta to w5
	mpy w4*w5,A			; Multiply error by beta
	sac.r A, w0			; Round and save to w0
	mov w0, _den_e_beta		; Copy to e_beta		
;-------------------------------------------------------------------------------
; Adjust all coefficients by h(n) = h(n) + e_beta*x(n), (pointer value held in w8)
;-------------------------------------------------------------------------------	     
	mov #_den_coeff, w10        	; Load start of coefficients into w10
	mov _den_e_beta, w7		; Copy error beta to w7
	mov _den_m,w1			; Load loop count
	mov _den_delay_ptr,w8		; Load current pointer address into w8
	mov [w8++], w5			; Load w5 with delay line sample and post increment address	
	do w1, den_adjust		; Loop round calculation for w1+1 times
	; New coefficient=(Delay Line tap value*error)+old coefficient
	mpy w5*w7, A, [w8]+=2, w5,[w10],w6 ; Multiply delay line data by error, prefetch and post increment delay line address
	add w6, A			; Add current coefficient value to accumulator A	
den_adjust:	sac.r A,[w10++]		; Write result back into current coefficient address and post increment address     	 	
;
; Coefficients adjusted for next pass    
;	
Denoiser_End:	
	
;*******************************************************************************
;
; Autonotcher
;
; Auto notch routine based upon an FIR filter with kernel tap values driven to 
; null out fixed frequency content at the summing point of the filter output and
; the source signal. The error between the input signal and the filter output is
; used to drive the filter tap algotrithm.
;
; The direction used to read and write the coefficients in the FIR filter and 
; adjustment routines is not important provided that it is the same for both 
; routines. In this implementation coefficients are operated on in the same 
; direction as delay line values, (see MAC instruction).
;
; Developed from the ADSP-21xx scheme employed in the DSP-10 transceiver by Bob 
; Larkin which in turn is based upon Sept 1996 QEX article by Johan Forrer, KC7WW.
;
;*******************************************************************************

Autonotch:
; Decay entire filter vector by decay constant
	mov #_aut_coeff,w10			; Point to start of coefficients
	mov _aut_decay,w4			; Load decay factor into w4
	mov _aut_m,w1				; Load filter length-1 into w1
	do w1,aut_decay_loop			; Decay each coefficient
	mov [w10],w5				; Copy coefficient to w5
	mpy w4*w5,A				; Decay coefficient
aut_decay_loop: 	sac.r A,[w10++]		; Round and save back to vector	    
;-------------------------------------------------------------------------------
; Decorrelation Delay    
;
; This delay path creates the reference signal used in the error calculation. 
; The delay differs to that of the denoiser. In this implementation the delay
; is added to that of the FIR (t=N/(2*fs)) such that the error is calaculated
; as the difference between the autonotch input and FIR output.    
;------------------------------------------------------------------------------- 
        mov	_autonotch_in, w0		; Copy input to w0 for delay	    
	mov	#_aut_dec_delay,w8		; Point to decorrelation delay
	mov	w8,XMODSRT			; XMODSRT = start of delay line
	mov	_aut_dec_end, w8		; Load delay line end address
        mov     w8,XMODEND			; XMODEND = end of delay line	
	mov	_aut_dec_ptr,w8			; Load pointer
	mov	[w8],w2				; Get delayed record
;	mov	w1,_autonotch_in_dlyd		; Copy to delayed input
	mov	w0, [w8++]			; Write new record and increment
	mov	w8,_aut_dec_ptr			; Update pointer	
	; Delayed sample in w2
;-------------------------------------------------------------------------------    
; Prepare Autonotcher FIR pointer and load new sample
;-------------------------------------------------------------------------------	
        mov     #_aut_delay, w8			; Load delay line start address into w8
        mov     w8, XMODSRT                     ; XMODSRT = start address of delay line
	mov	_aut_delay_end, w8		; Load delay line end address
        mov     w8, XMODEND                     ; XMODEND = end address of delay line
 	    mov	_aut_m, w1			; Load filter length -1 (M)	
	    mov _aut_delay_ptr, w8		; Load autonotch delay line pointer into w8
	    mov w2, [w8++]			; Move delayed sample into position pointed to by w8
	    mov	w8, _aut_delay_ptr		; Update delay line pointer       
	    mov #_aut_coeff, w10		; Coefficient start to w10
	    clr    A, [w8]+=2, w5, [w10]+=2, w6	; Clear MAC Accumulator A, load w5, w6 with initial values and increment addresses
	    repeat w1				; Repeat MAC instruction w1+1 times
	    mac w5*w6, A, [w8]+=2, w5, [w10]+=2, w6	; Data in X memory, Coefficients in Y memory
	    sac.r   A,w0			; Save accumulator to w0
	    mov w0, _aut_FIR_out		; Filter output for error calculation
;-------------------------------------------------------------------------------	    
; Normalisation of beta
;
; Calculate beta normalisation factor as fractional value of 1/||xn||^2, if
; numerator is greater than divisor, use the unmodified beta. For this instance,
; pre-populate _aut_beta_norm with beta before normalisation. If the numerator
; is less than the divisor, it will be overwritten with the normalised value.
;-------------------------------------------------------------------------------
aut_normalise:
	mov _aut_beta,w2		; Load beta
	mov w2,_aut_beta_norm		; Copy to normalised beta		    
;-------------------------------------------------------------------------------	   
; Use the MAC instruction to estimate power in delay line by calculating the
; Euclidean Norm squared, (sum of squares).
;-------------------------------------------------------------------------------
	mov #_aut_delay, w8		; Load delay line start address into w8
	mov _aut_m,w1			; Load M into repeat counter w1
	clr A,[w8]+=2,w4		; Clear accumulator & prefetch first value
	repeat w1			; Set up tap counter
	mac w4*w4, A, [w8]+=2,w4	; Square, accummulate and prefetch next
;-------------------------------------------------------------------------------	
; If the power held in the delay line causes an accumulator overflow, the 
; accumulator needs to be normalised to a fractional value. The numerator of the
; subsequent division is also reduced by the same amount to preserve the value 
; of the reciprocal. If the power is <1 then normalisation is skipped and the 
; value of aut_beta, which is preloaded into den_beta_norm, is used.
;-------------------------------------------------------------------------------
	sftac A,#5			; Shift accumulator to clear any overflow	
	sac.r A,w3			; Round and save to w3
	add #0x000f,w3			; Add a small constant
	mov #0x02ff,w2			; Load numerator (0x7fff>>5)
	cpsgt w3,w2			; Check that divisor is greater than numerator
	bra aut_calc_beta		; Low signal power, use den_beta
	repeat #17			; Load repeat counter
	divf w2,w3			; Fractional divide w2/w3
	mov w0,w4			; Copy result to w4
	mov _aut_beta,w5		; Load den_beta into w5
	mpy w4*w5,A			; Multiply it by fractional result
	sac.r A, w0			; Round and save to w0
	mov w0,_aut_beta_norm		; Copy to _aut_beta_norm    
;-------------------------------------------------------------------------------
; Calculate difference between FIR out and autonotch_in_dlyd to form the error 
; signal, multiply this by beta to create e_beta.
;-------------------------------------------------------------------------------
aut_calc_beta:	
	mov _autonotch_in,w2		; Copy input sample into w2
	mov _aut_FIR_out,w0             ; Copy FIR output into w0
	sub w2,w0,w4			; Subtract FIR output from denoise input and save to w4, e=xin-FIR_out	
	mov _aut_beta_norm,w5		; Copy beta to w5
	mpy w4*w5,A			; Multiply error by beta
	sac.r A, w0			; Round and save to w0
	mov w0, _aut_e_beta		; Copy to e_beta
	; For notch applications, error signal is the output		
aut_out: 	
	mov w4, _autonotch_out		; Copy difference to output	
;-------------------------------------------------------------------------------
; Adjust all coefficients by h(n) = h(n) + e_beta*x(n), (pointer value held in w8)
;-------------------------------------------------------------------------------	     
	mov #_aut_coeff, w10        	; Load start of coefficients into w10
	mov _aut_e_beta, w7		; Copy error beta to w7
	mov _aut_m,w1			; Load loop count
	mov _aut_delay_ptr,w8		; Load current pointer address into w8
	mov [w8++], w5			; Load w5 with delay line sample and post increment address	
	do w1, aut_adjust		; Loop round calculation for w1+1 times
	; New coefficient=(Delay Line tap value*error)+old coefficient
	mpy w5*w7, A, [w8]+=2, w5,[w10],w6 ; Multiply delay line data by error, prefetch and post increment delay line address
	add w6, A			; Add current coefficient value to accumulator A	
aut_adjust:	sac.r A,[w10++]		; Write result back into current coefficient address and post increment address    	
;-------------------------------------------------------------------------------
; Coefficients adjusted for next pass    
;-------------------------------------------------------------------------------
Autonotch_end:    
	
;*******************************************************************************
;	    
; Binaural delay to produce an adjustable delay between left and right channels.
;
; A circular buffer pointer is used to handle the insertion of new samples into 
; the delay line. The left channel is taken from the delay line mid point relative
; to the pointer. An adjustable +/- offset is added to the centre point address to
; point to the adjustable delay sample. Changing the adjustable delay allows it to
; be move either side of the mid point hence provide an advance or retarded delay
; address for the right channel.
;
; An audio balance features allows the relative gains of the left and right channel
; to be changed to compensate for any external gain imbalance.
;	
;*******************************************************************************
	    
binaural_delay:
        ; Set up modulo two addressing using w8
        mov #_bin_delay,w8              ; Load delay line start address into w8
        mov w8, XMODSRT                 ; XMODSRT = start address of delay line
        mov _bin_delay_end,w8		; Load delay line end address
        mov w8, XMODEND                 ; XMODEND = end address of delay line
	nop
	; Get delayed sample and write new one
        mov _binaural_IN, w4            ; Update delay line, get processed audio and load into w4	
        mov _bin_delay_ptr, w8		; Load delay line pointer into w8
	mov [w8],w6			; Get historic record place in w6
	mov w4,[w8++]			; Overwrite delay line record, post increment
	mov w8,_bin_delay_ptr		; Update pointer
	; Apply Balance Gains
	mov _left_gain_factor,w5	; Load left gain factor into w5
	mpy w5*w6,A			; Scale left audio sample
	sac.r A,w0			; Round and save to w0
	mov w0, _binaural_L		; Copy to left channel output	
	mov _right_gain_factor,w5	; Load right gain factor into w5
	mpy w4*w5,A			; Scale right audio sample
	sac.r A,w0			; Round and save to w1    	
        mov w0, _binaural_R	        ; Copy to right channel output			
;
;*********************************************************************************************
;
AF_proc_exit:
    nop
    pop    w10
    pop    w8
   
    pop     RCOUNT                          ; restore repeat counter
    pop     PSVPAG                          ; restore context of PSVPAG
    pop     CORCON                          ; restore context of CORCON
    pop     XMODEND                         ; restore context of YMODEND
    pop     XMODSRT                         ; restore context of YMODSRT
    pop     MODCON                          ; restore context of MODCON
    nop

    return

.end






