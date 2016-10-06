        .width 96
;******************************************************************************
;*                                                                            *
;*  FS_ADD v15.12.3                                                           *
;*                                                                            *
;* Copyright (c) 2000-2016 Texas Instruments Incorporated                     *
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
;*  FS$$ADD/FS$$SUB - add/sub two floating point numbers                      *
;*                                                                            *
;******************************************************************************
*;*****************************************************************************
*;                                                                            *
*;  revision:  original                                                       *
*;                                                                            *
*;*****************************************************************************
*;                                                                            *
*;       FS$$ADD/FS$$SUB                                        HI MEMORY     *
*;                                                                            *
*;                                                               stack        *
*;                                                         +----------------+ *
*;       This routine adds/subs two floating          SP-->|                | *
*;       poing numbers.  Upon entry one operand (OP1) is   +----------------+ *
*;       in ACC and the other (OP2) is on the        -1 -2 |     XAR1       | *
*;       stack as shown.  SP is moved to accomodate        +----------------+ *
*;       locals.  When add is finished the SP is     -3 -4 |     XAR2       | *
*;       returned to point at the return address.          +----------------+ *
*;                                                   -5 -6 | (return addr)  | *
*;       inputs:  OP1 - in ACC, OP2 - on stack             +----------------+ *
*;                                                      -7 |   MSW of OP2   | *
*;       implementation:  OP1 and OP2 are each unpacked    +----------------+ *
*;            into sign, exponent, and two words of     -8 |   LSW of OP2   | *
*;            mantissa.  If either exponent is zero        +----------------+ *
*;            special case processing is initiated.                           *
*;            In the general case, the exponents are           LO MEMORY      *
*;            compared and the mantissa of the lower                          *
*;            exponent is renormalized according to                           *
*;            the number with the larger exponent.           register file    *
*;            The mantissas are also converted to          +----------------+ *
*;            a two's compliment format to perform    XAR0 | OP2 mantissa   | *
*;            the actual addition.  The result of          +----------------+ *
*;            the addition is then renormalized with  XAR1 | OP1 mantissa   | *
*;            corresponding adjustment in the exponent.    +----------------+ *
*;            The resulting mantissa is converted     XAR2 | Result mantissa| *
*;            back to its original sign-magnitude          +----------------+ *
*;            format and the result is repacked into   AR4 | OP1 sign & exp | *
*;            the floating point representation.           +----------------+ *
*;            For cases in which the difference in     AR5 | OP2 sign & exp | *
*;            the exponents of the two input numbers,      +----------------+ *
*;            OP1 and OP2, is larger than 24 the       AR6 | res sign       | *
*;            result is the larger of the two numbers.     +----------------+ *
*;                                                     AR7 | res exponent   | *
*;                                                         +----------------+ *
*;            Subtraction is implemented by negting the second operand        *
*;            and adding the operands.                                        *
*;                                                                            *
*;       result:  returned in ACC                                             *
*;                                                                            *
*;    NOTE: The ordering of the locals are placed to take advantage           *
*;          of long word loads and stores which require the hi and lo         *
*;          words to be at certain addresses. Any future modifications        *
*;          which involve the stack must take this quirk into account         *
*;                                                                            *
*;                                                                            *
*;*****************************************************************************
        .page
*;*****************************************************************************
*;                                                                            *
*;  Floating Point Format - Single Precision                                  *
*;                                                                            *
*;                                                                            *
*;       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
*;       |31 |30 |29 |28 |27 |26 |25 |24 |23 |22 |21 |20 |19 |18 |17 |16 |    *
*;       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
*;       |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |    *
*;       | S |E7 |E6 |E5 |E4 |E3 |E2 |E1 |E0 |M22|M21|M20|M19|M18|M17|M16|    *
*;       |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |    *
*;       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
*;                                                                            *
*;       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
*;       |15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |    *
*;       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
*;       |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |    *
*;       |M15|M14|M13|M12|M11|M10|M9 |M8 |M7 |M6 |M5 |M4 |M3 |M2 |M1 |M0 |    *
*;       |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |    *
*;       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
*;                                                                            *
*;                                                                            *
*;       Single precision floating point format is a 32 bit format            *
*;       consisting of a 1 bit sign field, an 8 bit exponent field, and a     *
*;       23 bit mantissa field.  The fields are defined as follows.           *
*;                                                                            *
*;            Sign <S>          : 0 = positive values; 1 = negative values    *
*;                                                                            *
*;            Exponent <E7-E0>  : offset binary format                        *
*;                                00 = special cases (i.e. zero)              *
*;                                01 = exponent value + 127 = -126            *
*;                                FE = exponent value + 127 = +127            *
*;                                FF = special cases (not implemented)        *
*;                                                                            *
*;            Mantissa <M22-M0> : fractional magnitude format with implied 1  *
*;                                1.M22M21...M1M0                             *
*;                                                                            *
*;            Range             : -1.9999998 e+127 to -1.0000000 e-126        *
*;                                +1.0000000 e-126 to +1.9999998 e+127        *
*;                                (where e represents 2 to the power of)      *
*;                                -3.4028236 e+38  to -1.1754944 e-38         *
*;                                +1.1754944 e-38  to +3.4028236 e+38         *
*;                                (where e represents 10 to the power of)     *
*;*****************************************************************************
        .page
        .global   FS$$SUB
        .global   FS$$ADD

        .sect     ".text"

        .asg     XAR0,    OP2_M
        .asg     XAR1,    OP1_M 
        .asg     XAR2,    RES_M
        .asg      AR4,    OP1_SE
        .asg     XAR4,    XOP1_SE
        .asg      AR5,    OP2_SE
        .asg      AR6,    RES_SIGN
        .asg      AR7,    RES_EXP

        .asg    *-SP[7], OP2_MSW
        .asg    *-SP[8], OP2_LSW
        .asg    *-SP[3], S_OP2_MSW
        .asg    *-SP[4], S_OP2_LSW

FS$$SUB:	.asmfunc

*;*****************************************************************************
*;       SETUP                                                                *
*;       Test OP2 for zero, negate OP2 , branch to FS$$ADD                     *
*;*****************************************************************************
	CMP S_OP2_MSW, #0     ; test if OP2 == 0
        B   FS$$ADD, EQ       ; zero is a special case, don't negate
        XOR S_OP2_MSW, #8000h ; negate OP2                        
			      ; fall through to perform add 

FS$$ADD:
*
*;*****************************************************************************
*;       CONTEXT SAVE                                                         *
*;       Save contents of XAR1 and XAR2                                       *
*;*****************************************************************************
*
        PUSH    XAR1
        PUSH    XAR2

*
*;*****************************************************************************
*;       CONFIGURE STATUS BITS                                                *
*;*****************************************************************************
*
        CLRC    SXM
*
*;*****************************************************************************
*;       CONVERSION OF FLOATING POINT FORMAT - UNPACK                         *
*;  Test OP1 for special case treatment of zero.                              *
*;  Split the MSW of A in the accumulator.                                    *
*;    Save the sign and exponent on the stack [xxxx xxxS EEEE EEEE].          *
*;    Add the implied one to the mantissa value.                              *
*;    Store entire mantissa with a long word store                            *
*;*****************************************************************************
*
        TEST    ACC
        B       OP1_ZERO, EQ      ; if OP1 is 0, jump to special case
	MOVL	RES_M, ACC	  ; Save OP1 in case OP2 is zero.
        AND     AH, #07Fh         ; mask off sign & exp to get high mantissa
        ADD     AH, #080h         ; add implied 1 to mantissa
	LSL	ACC,#6		  ; Leave room for sign and overflow.
	MOVL	OP1_M, ACC	  ;
        MOVL    ACC, RES_M        ; restore high part containing sign&exp to AH
        PUSH    ST0               ; remember N and Z for OP1
        LSR     AH, 7             ; remove high mantissa     
        AND     AH, #0FFh         ; remove sign
        MOVZ    OP1_SE, AH        ; store exponent
        POP     ST0               ; restore N and Z for OP1
        B       OP1_NONNEG, GEQ   ; if OP1 was non-negative, we're done
	MOVL	ACC, OP1_M
        NEG     ACC               ; negate OP1 mantissa for negative values
	MOVL	OP1_M, ACC	  ;
OP1_NONNEG  
*
*;*****************************************************************************
*;       CONVERSION OF FLOATING POINT FORMAT - UNPACK                         *
*;  Test OP2 for special case treatment of zero.                              *
*;  Split the MSW of A in the accumulator.                                    *
*;    Save the sign and exponent on the stack [xxxx xxxS EEEE EEEE].          *
*;    Add the implied one to the mantissa value.                              *
*;    Store entire mantissa                                                   *
*;*****************************************************************************
*
        MOVL    ACC, OP2_LSW      ; load ACC with OP2
        B       OP2_ZERO, EQ      ; if OP2 is 0, jump to special case
        MOVZ    OP2_SE, AH        ; store AH before sign & exp are removed
        AND     AH, #07Fh         ; mask off sign & exp to get high mantissa
        ADD     AH, #080h         ; add implied 1 to mantissa
	LSL	ACC,#6		  ; Leave room for sign and overflow.
	MOVL	OP2_M, ACC	  ; Store mantissa
        MOV     AH, OP2_SE        ; restore high part containing sign & exp
        PUSH    ST0               ; remember N and Z for OP2
        LSR     AH, 7             ; remove high mantissa     
        AND     AH, #0FFh         ; remove sign
        MOVZ    OP2_SE, AH        ; store exponent
        POP     ST0               ; restore N and Z for OP2
        B       OP2_NONNEG, GEQ   ; if OP2 was non-negative, we're done
	MOVL	ACC,OP2_M	  ; restore mantissa.
        NEG     ACC               ; negate OP2 mantissa for negative values
	MOVL	OP2_M, ACC	  ; store negated mantissa.
OP2_NONNEG  
*
*;*****************************************************************************
*;       EXPONENT COMPARISON                                                  *
*;  Compare exponents of OP1 and OP2 by subtracting: exp OP2 - exp OP1        *
*;  Branch to one of three blocks of processing                               *
*;    Case 1:  exp OP1 is less than exp OP2                                   *
*;    Case 2:  exp OP1 is equal to exp OP2                                    *
*;    Case 3:  exp OP1 is greater than exp OP2                                *
*;*****************************************************************************
*
        SETC    SXM
        MOV     ACC, OP2_SE     ; load OP2 exponent
        SUB     ACC, OP1_SE     ; AH = exp OP2 - exp OP1
        B       OP1_GT_OP2, LT  ; process OP1 > OP2
        B       OP2_GT_OP1, GT  ; process OP1 > OP2
      ; fall through if OP1 == OP2
*
*;*****************************************************************************
*;       exp OP1 == exp OP2                                                   *
*;  Mantissas of OP1 and OP2 are normalized identically.                      *
*;  Add mantissas:  mant OP1 + mant OP2                                       *
*;  If result is zero, special case processing must be executed.              *
*;  Load exponent for possible adjustment during normalization of result      *
*;*****************************************************************************
*
	MOVL	ACC, OP1_M	; load OP1 mantissa.
	ADDL	ACC, OP2_M	; add OP2 mantissa.
        B       RES_ZERO, EQ    ; If result is zero, process special case
*
*;*****************************************************************************
*;       NORMALIZE THE RESULT                                                 *
*;  Take the absolute value of the result.                                    *
*;  Set up to normalize the result.                                           *
*;    The MSB may be in any of bits 24 through 0.                             *
*;    Left shift by six bits; bit 24 moves to bit 30, etc.                    *
*;  Normalize resulting mantissa with exponent adjustment.                    *
*;*****************************************************************************
*
NORMALIZE
        MOV     RES_SIGN, AH    ; save signed mantissa
        ABS     ACC             ; create magnitude value of mantissa
        INC     OP1_SE          ; increment exp to account for implied carry

	RPT	#31
     || NORM	ACC, XOP1_SE--  ; normalize result and adjust exponent

	MOV	RES_EXP, OP1_SE	; save result exponent
	ADD	ACC,#0x40	; Add rounding bit.

	B	NO_CARRY, GEQ	; 
	SFR     ACC,#1		; Adjust mantissa if rounding generated a carry
	INC	RES_EXP		; Adjust exponent if rounding generated a carry
NO_CARRY:

*
*;*****************************************************************************
*;       POST-NORMALIZATION ADJUSTMENT AND STORAGE                            *
*;  Test result for underflow and overflow.                                   *
*;  Right shift mantissa by 7 bits.                                           *
*;  Mask implied 1                                                            *
*;  Store mantissa on stack.                                                  *
*;*****************************************************************************
*
	MOVL	RES_M,ACC	; save result mantissa
        MOV     AH, RES_EXP     ; test result exponent
        B       UNDERFLOW, LEQ  ; process underflow if occurs
        SUB     AH, #0FFh       ; adjust to check for overflow
        B       OVERFLOW, GEQ   ; process overflow if occurs
	MOVL	ACC, RES_M	; Restore result mantissa
        SFR     ACC, 7          ; shift right to place mantissa for splitting
        AND     AH, #07Fh       ; eliminate implied one
	MOVL	RES_M, ACC	; Store result mantissa.
*
*;*****************************************************************************
*;       CONVERSION OF FLOATING POINT FORMAT - PACK                           *
*;  Load sign.                                                                *
*;  Pack exponent.                                                            *
*;  Pack mantissa.                                                            *
*;*****************************************************************************
*
        MOV     ACC, RES_SIGN << 9 ; 0000 000S ???? ???? ???? ???0 0000 0000
        AND     AH, #100h          ; 0000 000S 0000 0000 ???? ???0 0000 0000
        MOV     AL, #0             ; 0000 000S 0000 0000 0000 0000 0000 0000
        ADD     AH, RES_EXP        ; 0000 000S EEEE EEEE 0000 0000 0000 0000
        LSL     AH, 7              ; SEEE EEEE E000 0000 0000 0000 0000 0000
        ADDL    ACC, RES_M         ; SEEE EEEE EMMM MMMM MMMM MMMM MMMM MMMM
*
*;*****************************************************************************
*;       CONTEXT RESTORE                                                      *
*;  Pop local floating point variables.                                       *
*;  Pop XAR1 and XAR2                                                         *
*;*****************************************************************************
*
* entry: A - final result
*
RETURN_VALUE
        POP     XAR2
        POP     XAR1
        LRETR

        .page
*
*;*****************************************************************************
*;       exp OP1 > exp OP2                                                    *
*;  Test if the difference of the exponents is larger than 24 (precision of   *
*;  the mantissa).                                                            *
*;  Return OP1 as the result if OP2 is too small.                             *
*;  Mantissa of OP2 must be right shifted to match normalization of OP1.      *
*;  Add mantissas:  mant OP1 + mant op2                                       *
*;*****************************************************************************
*
OP1_GT_OP2
        ABS     ACC             ; if exp OP1 > exp OP2 + 24 then return OP1
        SUB     ACC, #24
        B       RETURN_OP1, GT
        ADD     ACC, #24        ; restore exponent difference value
        MOV     RES_SIGN, AL    ; store exponent difference to be used as RPC
	MOVL	ACC, OP2_M	;
        MOV     T, RES_SIGN
	ASRL	ACC,T		;

	ADDL	ACC, OP1_M	; add OP1 to OP2
        B       NORMALIZE,NEQ   ; branch to normalize result if ACC != 0
	B       RETURN_VALUE,UNC ; ACC = 0, gone beyond precision
*
*;*****************************************************************************
*;       OP1 < OP2                                                            *
*;  Test if the difference of the exponents is larger than 24 (precision of   *
*;  the mantissa).                                                            *
*;  Return OP2 as the result if OP1 is too small.                             *
*;  Mantissa of OP1 must be right shifted to match normalization of OP2.      *
*;  Add mantissas:  mant OP1 + mant OP2                                       *
*;  If A = 0 then have gone beyond precision available                        *
*;*****************************************************************************
*
OP2_GT_OP1
        SUB     ACC, #24        ; if exp OP2 > exp OP1 + 24 then return OP2
        B       RETURN_OP2, GT
        ADD     ACC, #24        ; restore exponent difference value
        MOV     RES_SIGN, AL    ; store exponent difference to be used as RPC
        MOVL    ACC, OP1_M      ; load OP1 mantissa
        MOV     T, RES_SIGN
	ASRL	ACC,T		;

        MOVZ    OP1_SE, OP2_SE  ; load exponent value to prep for normalization
	ADDL	ACC, OP2_M	; add OP2 to OP1
        B       NORMALIZE, NEQ  ; branch to normalize result if ACC != 0
        B       RETURN_VALUE,UNC ; ACC = 0, gone beyond precision

*
*;*****************************************************************************
*;       OP1 << OP2  or OP1 = 0                                               *
*;*****************************************************************************
*
RETURN_OP2
OP1_ZERO
        MOVL    ACC, OP2_LSW    ; put OP2 as result into A
        B       RETURN_VALUE,UNC

*
*;*****************************************************************************
*;       OP1 << OP2  or  OP1 = 0                                              *
*;*****************************************************************************
*
OP2_ZERO
RETURN_OP1
	MOVL	ACC, RES_M	  ; Restore OP1
        B       RETURN_VALUE,UNC

         .page
*;*****************************************************************************
*;       OVERFLOW PROCESSING                                                  *
*;  Load accumulator with return value.                                       *
*;*****************************************************************************
*
OVERFLOW
        MOV     AH, RES_SIGN    ; pack sign of result
        AND     AH, #8000h      ; mask to get sign
        ADD     AH, #07F7Fh     ; result exponent = 0FEh
                                ; result high mant = 07Fh
        MOV     AL, #0FFFFh     ; result low mantissa = 0FFFFh
        B       RETURN_VALUE,UNC
*
*;*****************************************************************************
*;       UNDERFLOW PROCESSING                                                 *
*;  Load accumulator with return value.                                       *
*;*****************************************************************************
*
UNDERFLOW
RES_ZERO
        MOV     ACC, #0         ; For underflow result = 0
        B       RETURN_VALUE,UNC
	.endasmfunc

