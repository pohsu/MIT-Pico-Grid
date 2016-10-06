        .width 96
;******************************************************************************
;*                                                                            *
;*  FD_MPY v15.12.3 
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
;*  FD$$MPY - Multiply two IEEE 754 format double precision floating point    *
;*            numbers                                                         *
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
;*   o IF THE OPERATION INVOLVES INFINITY OR ZERO AS INPUT, FIRST OPERAND     
;*     DETERMINES THE RESULT AS FOLLOWS:                                    
;*        INFINITY * ANYTHING = INF   (SIGN DETERMINED AS USUAL)
;*        0        * ANYTHING = UNDERFLOW - RETURN ZERO
;*        DENORMAL * ANYTHING = UNDERFLOW - RETURN ZERO
;*
;****************************************************************************
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
;*     -5 -6 |     XAR2       |             XAR4  |    PTR TO OP1     |       *
;*           +----------------+                   +-------------------+       *
;*     -7 -8 |     XAR1       |             XAR5  |    PTR TO OP2     |       *
;*           +----------------+                   +-------------------+       *
;*     -9 -10|  Retrun addr   |             XAR6  |   PTR TO RESULT   |       *
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
        .global   FD$$MPY
        .sect     ".text"

FD$$MPY:	.asmfunc
        .asg      XAR0,    OP1_HM
        .asg      XAR1,    OP1_LM
        .asg      XAR2,    OP2_HM
        .asg      XAR3,    OP2_LM
        .asg      XAR3,    TMP32  
        .asg       AR4,    RES_EXP
        .asg       AR7,    RES_SIGN
        .asg   *-SP[1],    OP1_EXP
        .asg   *-SP[2],    OP2_EXP

        .asg   *+XAR4[3],  OP1_SIGN
        .asg   *+XAR4[2],  OP1_MSW
        .asg   *+XAR4[0],  OP1_LSW
        .asg   *+XAR5[3],  OP2_SIGN
        .asg   *+XAR5[2],  OP2_MSW
        .asg   *+XAR5[0],  OP2_LSW
        .asg   *+XAR6[2],  RES_H
        .asg   *+XAR6[0],  RES_L

*
*;*****************************************************************************
*;       CONTEXT SAVE                                                         *
*;       Save contents of XAR1 - XAR3, and allocate stack space for locals    *
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
        MOVL    ACC, XAR6         ;
        B       RETURN, EQ        ; Return if return pointer is null
*
*;*****************************************************************************
*;       SIGN EVALUATION                                                      *
*;  Exclusive OR sign bits of OP1 and OP2 to determine sign of result.        *
*;*****************************************************************************
*
	MOV	RES_SIGN, OP2_SIGN
        MOV     AH, OP1_SIGN
	XOR	RES_SIGN, AH
*
*;*****************************************************************************
*;      UNPACK OP1 INTO EXPONENT AND MANTISSA                                 *
*;  1. Extract the exponent in OP1_EXP                                        *
*;  2. Extract the mantissa in ACC:P in bits 62 to 11                         *
*;  3. Add the implied one at bit 63                                          *
*;  4. Store mantissa in OP1_HM:OP1_LM                                        *
*;*****************************************************************************
*
        UNPACK64  XAR4, OP1_EXP ; Unpack OP1
	MOVL	OP1_HM, ACC	; Save OP1 high mantissa 
	MOVL	OP1_LM, P	; Save OP1 low mantissa 
*
*;*****************************************************************************
*;  Check OP1 for the following special cases.                                *
*;   - If exp is zero OP1 is zero or denormal - Underflow                     *
*;   - If exp is 0x7FF OP1 is infinite or NaN - Overflow                      *
*;*****************************************************************************
*
	CMP	OP1_EXP, #0	;
	B	UNDERFLOW, EQ	; Underflow if OP1 exponent is zero 

	CMP	OP1_EXP, #0x7FF	; 
	B	OVERFLOW, EQ	; Overflow if infinity or NaN; ie exp == 0x7FF
*
*;*****************************************************************************
*;      UNPACK OP1 INTO EXPONENT AND MANTISSA                                 *
*;  1. Extract the exponent in OP2_EXP                                        *
*;  2. Extract the mantissa in ACC:P in bits 62 to 11                         *
*;  3. Add the implied one at bit 63                                          *
*;  4. Store mantissa in OP2_HM:OP2_LM                                        *
*;*****************************************************************************
*
        UNPACK64  XAR5, OP2_EXP ; Unpack OP2
	MOVL	OP2_HM, ACC	; Save OP2 high mantissa 
	MOVL	OP2_LM, P	; Save OP2 low mantissa 
*
*;*****************************************************************************
*;  Check OP2 for the following special cases.                                *
*;   - If exp is zero OP2 is zero or denormal - Underflow                     *
*;   - If exp is 0x7FF OP2 is infinite or NaN - Overflow                      *
*;*****************************************************************************
*
        CMP     OP2_EXP, #0     ;
        B       UNDERFLOW, EQ   ; Underflow if OP2 exponent is zero 

        CMP     OP2_EXP, #0x7FF ; 
        B       OVERFLOW, EQ    ; Overflow if infinity or NaN; ie exp == 0x7FF
*
*;*****************************************************************************
*;  Add the exponents and store in RES_EXP for later processing               *
*;*****************************************************************************
*
	MOV	AH, OP1_EXP	  ; Load OP1 exponent
	ADD	AH, OP2_EXP	  ; Add OP2 exponent to it
	MOV	RES_EXP, AH	  ; Store the result
*
*;*****************************************************************************
*;       MULTIPLICATION                                                       *
*;  Multiplication is implemented by parts.  Mantissa for OP1 is in 64 bits   *
*;  represented by two 32 bit registers say A and B. And mantissa for OP2 is  *
*;  in two 32 bit registers say C and D. Then                                 *
*;                                                                            *
*;                          A    B      (mantissa of OP1)                     *
*;                   x      C    D      (mantissa of OP2)                     *
*;                     ===========                                            *
*;                             B*D      <-- Use only upper 32 bits of result  *
*;                           A*D                                              *
*;                           B*C                                              *
*;                         A*C                                                *
*;                     ===========                                            *
*;                        result        <-- result is always the upper 64 bits*
*;                                          of the possible 128 bits          *
*;  We need only the upper 64 bits of the possible 128 bit multiplication     *
*;  result. The upper 64 bits are calculated as follows.                      *
*;                                                                            *
*;  H64(A:B*C:D) = H32(A*D) + H32(B*C) + A64(A*C) + C                         *
*;  C = H32( H32(B*D) + L32(A*D) + L32(B*C) )                                 *
*;  Where H32 means High 32 bits of the 64 bit value                          *
*;        L32 means Low 32 bits of the 64 bit value                           *
*;        L32 means Low 32 bits of the 64 bit value                           *
*;        A64 means All 64 bits of the 64 bit value                           *
*;  Note that C is the carry of doing 32 bit addition of three 32 bit values  *
*;                                                                            *
*;*****************************************************************************
*
	ZAPA			; Zero out ACC, P and OVC
	MOVL	XT, OP2_LM	;      
	QMPYUL	P, XT, OP1_LM	; P = H32(B*D)
	MOVL	ACC, P		; ACC = H32(B*D)
	IMPYL	P, XT, OP1_HM	; P = L32(A*D)
	ADDUL	ACC, P		; ACC = L32(H32(B*D)+L32(A*D)); OVC has H32
	MOVL	XT, OP1_LM
	IMPYL	P, XT, OP2_HM	; P = L32(B*C)
	ADDUL	ACC, P		; OVC = H32(H32(B*D) + L32(A*D) + L32(B*C))
	MOVU	AL, OVC		; Move C to ACC
	MOVB	AH, #0

	ZAP	OVC		; Zero out OVC
	MOVL	XT, OP1_LM	; 
	QMPYUL	P, XT, OP2_HM	; P = H32(B*C)
	ADDUL	ACC, P		; ACC = H32(B*C) + C
	MOVL	XT, OP1_HM	; 
	QMPYUL	P, XT, OP2_LM	; P = H32(A*D)
	ADDUL	ACC, P		; ACC = H32(A*D) + H32(B*C) + C
	IMPYL	P, XT, OP2_HM	; P = L32(A*C)
	ADDUL	ACC, P		; ACC = L32(L32(A*C) + H32(A*D) + H32(B*C) + C)
	MOVL	RES_L, ACC	; Store L32(result)
	MOVU	AL, OVC		; ACC = H32(L32(A*C) + H32(A*D) + H32(B*C) + C)
	MOVB	AH, #0		;
	QMPYUL	P, XT, OP2_HM	; P = H32(A*C)
	ADDUL	ACC, P		; ACC = H32(result)
	MOVL	RES_H, ACC	; Store H32(result)
*
*;*****************************************************************************
*;  Adjust the mantissa if MSB in bit 63 for further processing.              *
*;*****************************************************************************
*
	MOVL	P, RES_L	; Restore result in ACC:P for rounding
	B	ROUND, GEQ	; Round the value if MSB is not in bit 63 
	LSR64	ACC:P, #1	; Adjust the mantissa 
	INC	RES_EXP		; Adjust the exponent
*
*;*****************************************************************************
*;  Round the result mantissa to the nearest value by using ROUND64 macro.    *
*;*****************************************************************************
*
ROUND:
        ROUND64 RES_EXP, TMP32  ;
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
*;  Adjust the bias by subtracting 1023 from the exponent and check for       *
*;  underflow and overflow.                                                   *
*;*****************************************************************************
*
	SUB	RES_EXP, #0x3FF
	B	UNDERFLOW, LEQ
	CMP	RES_EXP, #0x7FF
	B	OVERFLOW, GEQ
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
*;  Pop XAR1 - XAR3                                                           *
*;*****************************************************************************
*
RETURN_VALUE:
        MOVL    RES_L, P     ;
        MOVL    RES_H, ACC   ; Load the return value in the return area.
RETURN:
        SUBB    SP, #2
        POP     XAR3
	POP	XAR2
	POP	XAR1
        LRETR

        .page
*;*****************************************************************************
*;       OVERFLOW PROCESSING                                                  *
*;  Set ACC:P to 0xFFF0000:000000000 if sign is negative; otherwise set it to *
*;  0x7FF00000:00000000                                                       *
*;*****************************************************************************
*
OVERFLOW:
        OVERFLOW64      RES_SIGN
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

