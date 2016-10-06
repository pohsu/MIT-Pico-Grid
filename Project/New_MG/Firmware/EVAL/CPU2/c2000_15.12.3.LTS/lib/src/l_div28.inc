;*****************************************************************************
;* l_div28.inc  v15.12.3
;* 
;* Copyright (c) 1997-2016 Texas Instruments Incorporated
;* http://www.ti.com/ 
;* 
;*  Redistribution and  use in source  and binary forms, with  or without
;*  modification,  are permitted provided  that the  following conditions
;*  are met:
;* 
;*     Redistributions  of source  code must  retain the  above copyright
;*     notice, this list of conditions and the following disclaimer.
;* 
;*     Redistributions in binary form  must reproduce the above copyright
;*     notice, this  list of conditions  and the following  disclaimer in
;*     the  documentation  and/or   other  materials  provided  with  the
;*     distribution.
;* 
;*     Neither the  name of Texas Instruments Incorporated  nor the names
;*     of its  contributors may  be used to  endorse or  promote products
;*     derived  from   this  software  without   specific  prior  written
;*     permission.
;* 
;*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS
;*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT
;*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
;*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
;*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT
;*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
;*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
;*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT
;*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;* 
;*****************************************************************************


******************************************************************************
* This module contains the functions for 32-bit divide and modulus, signed and
* unsigned.  This stack map and these defines apply to all the routines in 
* this module.               
*
* SYMBOL DEFINITIONS:
*
* DEN   - denominator 
*
******************************************************************************
; Numerator passed in ACC, denominator is on stack
;


******************************************************************************
* SET UP ALIASES FOR STACK REFERENCES AND REGISTERS USED
******************************************************************************
	.asg	*-SP[2],DEN	; denominator 

	.page
******************************************************************************
* 32-bit SIGNED DIVIDE, CALCULATE NUM / DEN AND RETURN IN ACCUMULATOR 
******************************************************************************
	.global	L$$DIV

L$$DIV:		.asmfunc stack_usage(0)
******************************************************************************
* DETERMINE SIGN OF RESULT, TAKE ABSOLUTE VALUE OF OPERANDS
******************************************************************************
	CLRC	OVM		; Clear the OVM bit to get desired effect on 
				; the ABS instruction.
	CLRC	TC		; Clear TC flag, used to store sign
	ABSTC	ACC		; Take absolute value, TC = sign ^ TC
	MOVL	P, ACC		; Load P register with the numerator
	MOVL	ACC, DEN	; Load the denominator
	ABSTC	ACC		; Take absolute value, TC = sign ^ TC
	MOVL	DEN, ACC	; Move |denomiator| to DEN

******************************************************************************
* PERFORM DIVIDE
******************************************************************************
	MOVB	ACC, #0		; Clear ACC to perform the division.
	RPT	#31		;
     || SUBCUL	ACC, DEN	; Divide numerator by the denominator; the 
				; quotient is in P and the remainder is in ACC

	MOVL	ACC, @P		; Load the quotient into ACC
	NEGTC	ACC		; Negate ACC if TC = 1

	CLRC	OVM		; Clear the OVM bit 
	FFCRET	*XAR7		; return
	.endasmfunc



	.page
******************************************************************************
* 32-bit SIGNED MODULUS, CALCULATE NUM % DEN AND RETURN IN ACCUMULATOR
******************************************************************************
	.global	L$$MOD

L$$MOD:		.asmfunc stack_usage(0)
******************************************************************************
* DETERMINE SIGN OF RESULT, TAKE ABSOLUTE VALUE OF OPERANDS
******************************************************************************
        CLRC    OVM             ; Clear the OVM bit to get desired effect on
                                ; the ABS instruction.
        CLRC    TC              ; Clear TC flag, used to store sign
        ABSTC   ACC             ; Take absolute value, TC = sign ^ TC
        MOVL    P, ACC          ; Load P register with the numerator
        MOVL    ACC, DEN        ; Load the denominator
        ABS     ACC             ; Take absolute value, TC = sign ^ TC
        MOVL    DEN, ACC        ; Move |denomiator| to DEN.

******************************************************************************
* PERFORM DIVIDE
******************************************************************************
        MOVB    ACC, #0         ; Clear ACC to perform the division.
        RPT     #31             ;
     || SUBCUL	ACC, DEN	; Divide numerator by the denominator; the 
				; quotient is in P and the remainder is in ACC

        NEGTC   ACC             ; Negate ACC if TC = 1

        CLRC    OVM             ; Clear the OVM bit
	FFCRET	*XAR7		; return
	.endasmfunc



	.page
******************************************************************************
* 32-bit UNSIGNED DIVIDE, CALCULATE NUM / DEN AND RETURN IN ACCUMULATOR
******************************************************************************
	.global	UL$$DIV

UL$$DIV:	.asmfunc stack_usage(0)
	MOVL	P, ACC		; Load the numerator into P 

******************************************************************************
* PERFORM DIVIDE
******************************************************************************
        MOVB    ACC, #0         ; Clear ACC to perform the division.
        RPT     #31             ;
     || SUBCUL	ACC, DEN	; Divide numerator by the denominator; the 
				; quotient is in P and the remainder is in ACC

	MOVL	ACC, P		; Load the quotient into return register.
	FFCRET	*XAR7		; return
	.endasmfunc

******************************************************************************
* 32-bit UNSIGNED MODULUS, CALCULATE NUM % DEN AND RETURN IN ACCUMULATOR
******************************************************************************
	.global	UL$$MOD

UL$$MOD:	.asmfunc stack_usage(0)
	MOVL	P, ACC		; Load the numerator into P 

******************************************************************************
* PERFORM DIVIDE
******************************************************************************
        MOVB    ACC, #0         ; Clear ACC to perform the division.
        RPT     #31             ;
     || SUBCUL	ACC, DEN	; Divide numerator by the denominator; the 
				; quotient is in P and the remainder is in ACC

	FFCRET	*XAR7		; return
	.endasmfunc

