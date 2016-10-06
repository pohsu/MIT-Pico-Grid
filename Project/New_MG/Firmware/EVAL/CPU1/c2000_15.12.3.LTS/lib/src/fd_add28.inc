        .width 96
;******************************************************************************
;*                                                                            *
;*  FD_ADD v15.12.3 
;*                                                                            *
;* Copyright (c) 2002-2016 Texas Instruments Incorporated                     *
;* http://www.ti.com/                                                         *
;*                                                                            *
;*  Redistribution and  use in source  and binary forms, with  or without     *
;*  modification,  are permitted provided  that the  following conditions     *
;*  are met:                                                                  *
;*                                                                            *
;*     Redistributions  of source  code must  retain the  above copyright     *
;*     notice, this list of conditions and the following disclaimer.          *
;*                                                                            *
;*     Redistributions in binary form  must reproduce the above copyright     *
;*     notice, this  list of conditions  and the following  disclaimer in     *
;*     the  documentation  and/or   other  materials  provided  with  the     *
;*     distribution.                                                          *
;*                                                                            *
;*     Neither the  name of Texas Instruments Incorporated  nor the names     *
;*     of its  contributors may  be used to  endorse or  promote products     *
;*     derived  from   this  software  without   specific  prior  written     *
;*     permission.                                                            *
;*                                                                            *
;*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS     *
;*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT     *
;*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR     *
;*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT     *
;*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
;*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT     *
;*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
;*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
;*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT     *
;*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
;*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
;*                                                                            *
;*  FD$$ADD - Add two IEEE 754 format double precision floating point numbers *
;*                                                                            *
;******************************************************************************
;******************************************************************************
;*                                                                            *
;*  revision:  original                                                       *
;*                                                                            *
;******************************************************************************
;*
;*   o INPUT OP1 IS IN *XAR4
;*   o INPUT OP2 IS IN *XAR5                  
;*   o RESULT IS RETURNED IN *XAR6
;*   o INPUT OP1 AND OP2 ARE PRESERVED                                 
;*
;*   o SIGNALING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS +/- INFINITY
;*       (0x7ff00000:00000000) or (0xfff00000:00000000)
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o UNDERFLOW RETURNS ZERO (0x00000000:00000000)
;*   o ROUNDING MODE:  ROUND TO NEAREST
;*
;*   o IF OPERATION INVOLVES INFINITY AS AN INPUT, THE FOLLOWING SUMMARIZES
;*     THE RESULT:
;*                   +----------+----------+----------+
;*         ADDITION  + OP2 !INF | OP2 -INF + OP2 +INF +
;*        +----------+==========+==========+==========+
;*        + OP1 !INF +    -     |   -INF   +   +INF   +
;*        +----------+----------+----------+----------+
;*        + OP1 -INF +   -INF   |   -INF   +   -INF   +
;*        +----------+----------+----------+----------+
;*        + OP1 +INF +   +INF   |   +INF   +   +INF   +
;*        +----------+----------+----------+----------+
;*
;******************************************************************************
;*
;* +------------------------------------------------------------------+
;* | DOUBLE PRECISION FLOATING POINT FORMAT                           |
;* |   64-bit representation                                          |
;* |   31 30      20 19                  0                            |
;* |   +-+----------+---------------------+                           |
;* |   |S|     E    |        M1           |                           |
;* |   +-+----------+---------------------+                           |
;* |                                                                  |
;* |   31                                0                            |
;* |   +----------------------------------+                           |
;* |   |             M2                   |                           |
;* |   +----------------------------------+                           |
;* |                                                                  |
;* |   <S>  SIGN FIELD    :          0 - POSITIVE VALUE               |
;* |                                 1 - NEGATIVE VALUE               |
;* |                                                                  |
;* |   <E>  EXPONENT FIELD: 0000000000 - ZERO IFF M == 0              |
;* |            0000000001..1111111110 - EXPONENT VALUE(1023 BIAS)    |
;* |                        1111111111 - INFINITY                     |
;* |                                                                  |
;* |   <M1:M2>  MANTISSA FIELDS:  FRACTIONAL MAGNITUDE WITH IMPLIED 1 |
;* +------------------------------------------------------------------+
;*
;******************************************************************************
;*                                                                            *
;*               HI MEMORY                                                    *
;*                                                                            *
;*                 stack                            register file             *
;*           +----------------+                   +-------------------+       *
;*      SP-->|                |             XAR0  | OP1 hign mantissa |       *
;*           +----------------+                   +-------------------+       *
;*        -1 |    OP1_EXP     |             XAR1  | OP1 low mantissa  |       *
;*           +----------------+                   +-------------------+       *
;*        -2 |    OP2_EXP     |             XAR2  | OP2 high mantissa |       *
;*           +----------------+                   +-------------------+       *
;*     -3 -4 |     XAR3       |             XAR3  | OP2 low mantissa  |       *
;*           +----------------+                   +-------------------+       *
;*     -5 -6 |     XAR2       |             XAR4  |    PTR to OP1     |       *
;*           +----------------+                   +-------------------+       *
;*     -7 -8 |     XAR1       |             XAR5  |    PTR to OP2     |       *
;*           +----------------+                   +-------------------+       *
;*     -9 -10| Retrun addr    |             XAR6  |  PTR to RET VALUE |       *
;*           +----------------+                   +-------------------+       *
;*                                           AR7  |        | RES_SIGN |       *
;*                                                +-------------------+       *
;*               LO MEMORY                                                    *
;*                                                                            *
;*    NOTE: The ordering of the locals are placed to take advantage           *
;*          of long word loads and stores which require the hi and lo         *
;*          words to be at certain addresses. Any future modifications        *
;*          which involve the stack must take this quirk into account         *
;*                                                                            *
;*                                                                            *
;******************************************************************************
        .page
	.include  "fd_util28.inc"
        .global   FD$$ADD
        .sect     ".text"

FD$$ADD:	.asmfunc
	.asg	  XAR0,	   OP1_HM
	.asg	  XAR1,	   OP1_LM
	.asg	  XAR2,	   OP2_HM
	.asg	  XAR3,	   OP2_LM
        .asg       AR4,    RES_EXP 
        .asg       AR7,    RES_SIGN
	.asg	  XAR3,	   TMP32 
	.asg	   AR3,	   TMP16 
        .asg   *-SP[1],    OP1_EXP 
        .asg   *-SP[2],    OP2_EXP 

	.asg   *+XAR4[3],  OP1_SIGN
        .asg   *+XAR4[2],  OP1_MSW
        .asg   *+XAR4[0],  OP1_LSW
	.asg   *+XAR5[3],  OP2_SIGN
        .asg   *+XAR5[2],  OP2_MSW
        .asg   *+XAR5[0],  OP2_LSW
        .asg   *+XAR6[2],  RES_MSW
        .asg   *+XAR6[0],  RES_LSW

*
*;*****************************************************************************
*;       CONTEXT SAVE                                                         *
*;       Save contents of XAR0 - XAR3, and allocate stack space for locals    *
*;*****************************************************************************
*
        PUSH    XAR1
        PUSH    XAR2
        PUSH    XAR3
        ADDB    SP, #2            ; Allocate space for locals
*
*;*****************************************************************************
*;       CHECK FOR NULL POINTER                                               *
*;*****************************************************************************
*
	MOVL	ACC, XAR6	  ;
	B	RETURN, EQ	  ; Return if return pointer is null
*
*;*****************************************************************************
*;      UNPACK AND SIGN ADJUST MANTISSA                                       *
*;  1. Extract the exponent in OP1_EXP                                        *
*;  2. Extract the mantissa in ACC:P in bits 60 to 9                          *
*;  3. Add the implied one at bit 61                                          *
*;  4. Negate the value if sign bit is set                                    *
*;  5. Store mantissa in OP1_HM:OP1_LM                                        *
*;*****************************************************************************
*
        MOV     RES_SIGN, OP1_SIGN    ; Save the sign
        UNPACK64   XAR4, OP1_EXP ; Unpack OP1 into OP1_EXP and ACC:P
        LSR64   ACC:P, 2        ; Allow room for sign and overflow 
        CMP     RES_SIGN, #0    ; Check if OP1 is negative  
        B       SAVE_MN1, GEQ   ;
        NEG64   ACC:P           ; Negate OP1 mantissa if OP1 is negative
SAVE_MN1:
	MOVL	OP1_HM, ACC	; Save OP1 hign mantissa 
	MOVL	OP1_LM, P	; Save OP1 low mantissa

*
*;*****************************************************************************
*;  Check OP1 for the following special cases.                                *
*;   - If exp is zero and mantissa is zero, OP1 is zero - return OP2          *
*;   - If exp is zero and mantissa is nonzero, OP1 is denormal - Underflow    *
*;   - If exp is 0x7FF OP1 is infinite or NaN - Overflow                      *
*;*****************************************************************************
*
	CMP	OP1_EXP, #0	; Check if OP1 exponent is zero
	B	EXP1_NZ, NEQ	; if OP1 exponent is not zero check for overflow
	LSL64	ACC:P, 3	; Check if mantissa is zero
	B	OP1_ZERO, EQ	; exp == 0 && mantissa == 0 so OP1 is zero;
				; return OP2.
	B	UNDERFLOW, UNC	; mantissa != 0 && exp == 0; Underflow for
				; Denormals.

EXP1_NZ:
	CMP	OP1_EXP, #0x7FF	; Check for infinity and NaN
	B	OVERFLOW, EQ    ; return overflow.
*
*;*****************************************************************************
*;      UNPACK AND SIGN ADJUST MANTISSA                                       *
*;  1. Extract the exponent in OP2_EXP                                        *
*;  2. Extract the mantissa in ACC:P in bits 60 to 9                          *
*;  3. Add the implied one at bit 61                                          *
*;  4. Negate the value if sign bit is set                                    *
*;  5. Store mantissa in OP2_HM:OP2_LM                                        *
*;*****************************************************************************
*
        MOV     RES_SIGN, OP2_SIGN     ; Save the sign
        UNPACK64   XAR5, OP2_EXP ; Unpack OP1 into OP2_EXP and ACC:P
        LSR64   ACC:P, 2        ; Allow room for sign and overflow 
        CMP     RES_SIGN, #0    ; Check if OP1 is negative  
        B       SAVE_MN2, GEQ   ;
        NEG64   ACC:P           ; Negate OP1 mantissa if OP1 sign is negative
SAVE_MN2:
	MOVL	OP2_HM, ACC	; Save OP2 hign mantissa 
	MOVL	OP2_LM, P	; Save OP2 low mantissa
*
*;*****************************************************************************
*;  Check OP2 for the following special cases.                                *
*;   - If exp is zero and mantissa is zero, OP2 is zero - return OP2          *
*;   - If exp is zero and mantissa is nonzero, OP2 is denormal - Underflow    *
*;   - If exp is 0x7FF OP2 is infinite or NaN - Overflow                      *
*;*****************************************************************************
*
        CMP	OP2_EXP, #0 	; Check if OP2_EXP is zero
	B	EXP2_NZ, NEQ	; if OP2 exponent is not zero check for overflow
	LSL64	ACC:P, 3	; Check if mantissa is zero
	B	OP2_ZERO, EQ	; exp == 0 && mantissa == 0 so OP2 is zero; 
				; return OP1
      	B	UNDERFLOW, UNC	; mantissa != 0 && exp == 0; Underflow for
				; Denormals
EXP2_NZ:
	CMP	OP2_EXP, #0x7FF	; Check for infinity and NaN
	B	OVERFLOW, EQ    ; return overflow.
*
*;*****************************************************************************
*;  Compare exponents of OP1 and OP2 by subtracting: exp OP1 - exp OP2        *
*;  Branch to PREP_ADD for further processing if exp OP1 is greater than or   *
*;  equal to exp OP2. Otherwise swap OP1 and OP2 so that OP1 has larger value.*
*;*****************************************************************************
*
        MOV     AH, OP1_EXP      ; load OP1 exponent
        SUB     AH, OP2_EXP      ; Find OP1 exp - OP2 exp                 
	B	PREP_ADD, GEQ	 ; If OP1 >= OP2 proceed to adjust OP2 mantissa

	MOVL	P, OP1_LM	 ; Otherwise swap OP1 and OP2 so that OP1 has
	MOVL	OP1_LM, OP2_LM   ; larger value.
	MOVL	OP2_LM, P        ; 
	MOVL	P, OP1_HM	 ;                   
	MOVL	OP1_HM, OP2_HM   ; 
	MOVL	OP2_HM, P        ; 
;	MOV	OP1_EXP, OP2_EXP ; Move the OP2 exp to OP1 exp.
	MOVZ	AR5, OP2_EXP	 ; Move the OP2 exp to OP1 exp.
	MOV	OP1_EXP, AR5	 ;
	NEG	AH		 ; Since we swapped the operands, negate the
	              		 ; difference in the exponent.
*
*;*****************************************************************************
*;  Process the following exponent special cases:                             *
*;  1. If the exponents of OP1 and OP2 are same (exp_diff == 0) no need to    *
*;     align the mantissa; go directly to add the operands.                   *
*;  2. If exp_diff is greater than 53 (52 bit mantissa + 1 for implied one),  *
*;     then OP2 is insignificant so we do not need to add.                    *
*;  3. If 0 < exp_diff < 54, adjust the OP2 mantissa to align the exponents   *
*;*****************************************************************************
*
PREP_ADD:
	MOV	T, AH		; Load the exp_diff from AH
	MOVL	ACC, OP1_HM	; Load OP1 mantissa into ACC:P          
	MOVL	P, OP1_LM	; 
	CMP	T, #54		; Check exp_diff is less than 54    
	B	NO_ADD, GEQ	; If not go to processing after ADD 
	CMP	T, #0		; Check if exp_diff is zero 
	B	ADD_OP, EQ	; If so go directly to Add the operands 

	MOVL	ACC, OP2_HM	; Load OP2 mantissa into ACC:P
	MOVL	P, OP2_LM	;
	ASR64 	ACC:P, T	; Shift right OP2 mantissa by T bits 
	MOVL	OP2_HM, ACC	; Save adjusted OP2 mantissa 
	MOVL	OP2_LM, P	; 
	MOVL    P, OP1_LM	; Load OP1 mantissa into ACC:P 
	MOVL    ACC, OP1_HM	;

ADD_OP: 
	ADDUL   P, OP2_LM	; Add OP2 mantissa to OP1 mantissa 
	ADDCL   ACC, OP2_HM	; 

NO_ADD:
	CMP64	ACC:P		; Clear V flag
	CMP64	ACC:P		; Check if the result mantissa is zero
        B       RETURN_VALUE,EQ ; If result is zero, return the value.
*
*;*****************************************************************************
*;  Call NORMALIZE64 macro to normalize the value. This macro updates the     *
*;  OP1_EXP based on the normalization and returns the value in RES_EXP.      *
*;  The normalization includes the following:                                 *
*;    - Take the absolute value of the mantissa                               *
*;    - Left shift the mantissa so that the 63rd bit in ACC:P is 1.           *
*;    - Adjust the exponent by subtracting it by number of bits shifted out.  *
*;*****************************************************************************
*
        MOV     RES_SIGN, AH    ; save signed mantissa
	CMP64   ACC:P		; Clear V flag
	CMP64	ACC:P		; Get the absolute mantissa
	B	NORMALIZE, GEQ	;
	NEG64	ACC:P		; Negate if mantissa is negative, get abs value

NORMALIZE:
	MOVZ	RES_EXP, OP1_EXP	; Load the result exponent
	NORMALIZE64	RES_EXP, TMP16  ; The normalized mantissa is in ACC:P
*
*;*****************************************************************************
*;  Round the result mantissa to the nearest value by using ROUND64 macro.    *
*;*****************************************************************************
*
	ROUND64	RES_EXP, TMP32	;
*
*;*****************************************************************************
*;  Then place the mantissa in bits 52 - 0 and remove the implied one         *
*;  52 - 0 for packing.                                                       *
*;*****************************************************************************
*
        LSR64   ACC:P, #10      ; Shift the mantissa with imp 1 to bits 53-0
	AND     AH, #0x000F     ; Removed the implied one
*
*;*****************************************************************************
*;  Adjust the exponent and test result for underflow and overflow.           *
*;*****************************************************************************
*
	INC	RES_EXP         ; Adjust the exponent - this is corresponding 
				; to the right shift by 2 to allow room for 
				; sign and overflow.                          
        B       UNDERFLOW, LEQ  ; process underflow if occurs
        CMP     RES_EXP, #0x7FF ; check for overflow
        B       OVERFLOW, GEQ   ; process overflow if occurs
*
*;*****************************************************************************
*;       CONVERSION OF FLOATING POINT FORMAT - PACK                           *
*;  Load sign.                                                                *
*;  Pack exponent.                                                            *
*;  Pack mantissa.                                                            *
*;*****************************************************************************
*
	PACK64	RES_SIGN, RES_EXP
*
*;*****************************************************************************
*;       CONTEXT RESTORE                                                      *
*;  Pop local floating point variables.                                       *
*;  Pop XAR0 - XAR3                                                           *
*;*****************************************************************************
*
* ACC:P - final result
*
RETURN_VALUE:
	MOVL	RES_LSW, P     ;
	MOVL	RES_MSW, ACC   ; Load the return value in the return area.

RETURN:
        SUBB    SP, #2
        POP     XAR3
        POP     XAR2
        POP     XAR1
        LRETR

        .page
*;*****************************************************************************
*;       OP1 << OP2  or OP1 = 0                                               *
*;*****************************************************************************
*
OP1_ZERO:
        MOVL    ACC, OP2_MSW    ; put OP2 as result into ACC:P
        MOVL    P,   OP2_LSW    ; put OP2 as result into ACC:P
        B       RETURN_VALUE,UNC

*
*;*****************************************************************************
*;       OP1 << OP2  or  OP1 = 0                                              *
*;*****************************************************************************
*
OP2_ZERO:
        MOVL    ACC, OP1_MSW    ; put OP1 as result into ACC:P
        MOVL    P,   OP1_LSW    ; put OP1 as result into ACC:P
        B       RETURN_VALUE,UNC

         .page
*;*****************************************************************************
*;       OVERFLOW PROCESSING                                                  *
*;  Set ACC:P to 0xFFF0000:000000000 if sign is negative; otherwise set it to *
*;  0x7FF00000:00000000                                                       *
*;*****************************************************************************
*
OVERFLOW:
	OVERFLOW64	RES_SIGN
        B       RETURN_VALUE,UNC
*
*;*****************************************************************************
*;       UNDERFLOW PROCESSING                                                 *
*;  Set ACC:P to 0x00000000:00000000                                          *
*;*****************************************************************************
*
UNDERFLOW:
        ZAPA                    ; For underflow result (ACC:P)= 0
        B       RETURN_VALUE,UNC
	.endasmfunc
