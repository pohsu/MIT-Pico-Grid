        .width 96
;******************************************************************************
;*                                                                            *
;*  FD_DIV v15.12.3 
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
;*  FD$$DIV - IEEE 754 format double precision number division                *
;*                                                                            *
;******************************************************************************
;******************************************************************************
;*                                                                            *
;*  revision:  original                                                       *
;*                                                                            *
;******************************************************************************
;*
;*   o NUMERATOR IS IN *XAR4
;*   o DENOMINATOR IS IN *XAR5                   
;*   o QUOTIENT IS RETURNED IN *XAR6
;*   o INPUT NUMERATOR AND DENOMINATOR ARE PRESERVED                    
;*
;*   o SIGNALING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS +/- INFINITY
;*       (0x7ff00000:00000000) or (0xfff00000:00000000)
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o UNDERFLOW RETURNS ZERO (0x00000000:00000000)
;*   o ROUNDING MODE:  ROUND TO NEAREST
;*   o IF NUMERATOR IS ZERO RETURN ZERO
;*   o IF NUMERATOR IS INFINITY RETURN INFINITY
;*   o DIVIDE BY ZERO OVERFLOWS
;*   o DIVIDE BY INFINITY UNDERFLOWS
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
;*      SP-->|                |             XAR0  | NUM hign mantissa |       *
;*           +----------------+                   +-------------------+       *
;*        -1 |    NUM EXP     |             XAR1  | NUM low mantissa  |       *
;*           +----------------+                   +-------------------+       *
;*        -2 |    DEN EXP     |             XAR2  | DEN high mantissa |       *
;*           +----------------+                   +-------------------+       *
;*     -3 -4 |     XAR3       |             XAR3  | DEN low mantissa  |       *
;*           +----------------+                   +-------------------+       *
;*     -5 -6 |     XAR2       |             XAR4  |    PTR TO NUM     |       *
;*           +----------------+                   +-------------------+       *
;*     -7 -8 |     XAR1       |             XAR5  |    PTR TO DEN     |       *
;*           +----------------+                   +-------------------+       *
;*     -9 -10|  Return addr   |             XAR6  |  PTR TO QUOTIENT  |       *
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
        .global   FD$$DIV
        .sect     ".text"

FD$$DIV:	.asmfunc
        .asg      XAR0,    NUM_HM
        .asg      XAR1,    NUM_LM
        .asg      XAR2,    DEN_HM
        .asg      XAR3,    DEN_LM
        .asg      XAR3,    TMP32  
        .asg       AR4,    RES_EXP
        .asg       AR5,    CNT     
        .asg       AR7,    RES_SIGN
        .asg   *-SP[1],    TMP16  
	.asg   *-SP[1],    NUM_EXP
	.asg   *-SP[2],    DEN_EXP

	.asg   *+XAR4[3],  NUM_SIGN
        .asg   *+XAR4[2],  NUM_MSW
        .asg   *+XAR4[0],  NUM_LSW
	.asg   *+XAR5[3],  DEN_SIGN
        .asg   *+XAR5[2],  DEN_MSW
        .asg   *+XAR5[0],  DEN_LSW
	.asg   *+XAR6[2],  RES_HQ  
	.asg   *+XAR6[0],  RES_LQ  

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
*;  Exclusive OR sign bits of NUM and DEN to determine sign of result.        *
*;*****************************************************************************
*
	MOV	RES_SIGN, DEN_SIGN
	MOV	AH, NUM_SIGN
	XOR	RES_SIGN, AH
*
*;*****************************************************************************
*;      UNPACK NUM INTO EXPONENT AND MANTISSA                                 *
*;  1. Extract the exponent in NUM_EXP                                        *
*;  2. Extract the mantissa in ACC:P in bits 60 to 9                          *
*;  3. Add the implied one at bit 61                                          *
*;  4. Store mantissa in NUM_HM:NUM_LM                                        *
*;*****************************************************************************
*
        UNPACK64  XAR4, NUM_EXP ; Unpack NUM
	LSR64	ACC:P, #10	; Move mantissa with implied one to bits 61-9
	MOVL	NUM_HM, ACC	; Store NUM mantissa
	MOVL	NUM_LM, P	;
*
*;*****************************************************************************
*;  Check NUM for the following special cases.                                *
*;   - If exp is zero NUM is zero or denormal - Underflow                     *
*;   - If exp is 0x7FF NUM is infinite or NaN - Overflow                      *
*;*****************************************************************************
*
	CMP	NUM_EXP, #0	;
	B	UNDERFLOW, EQ	; Underflow if NUM exponent is zero 

	CMP	NUM_EXP, #0x7FF	; 
	B	OVERFLOW, EQ	; Overflow if infinity or NaN; ie exp == 0x7FF
*
*;*****************************************************************************
*;      UNPACK NUM INTO EXPONENT AND MANTISSA                                 *
*;  1. Extract the exponent in DEN_EXP                                        *
*;  2. Extract the mantissa in ACC:P in bits 61 to 10                         *
*;  3. Add the implied one at bit 62                                          *
*;  4. Store mantissa in DEN_HM:DEN_LM                                        *
*;*****************************************************************************
*
        UNPACK64  XAR5, DEN_EXP ; Unpack DEN
	LSR64	ACC:P, #10	; Move mantissa with implied one to bits 62-9
	MOVL	DEN_HM, ACC	; Store DEN mantissa
	MOVL	DEN_LM, P	;
*
*;*****************************************************************************
*;  Check DEN for the following special cases.                                *
*;   - If exp is zero DEN is zero or denormal - Underflow                     *
*;   - If exp is 0x7FF DEN is infinite or NaN - Overflow                      *
*;*****************************************************************************
*
        CMP     DEN_EXP, #0     ;
        B       OVERFLOW, EQ    ; Overflow if DEN exponent is zero 

        CMP     DEN_EXP, #0x7FF ; 
        B       UNDERFLOW, EQ   ; Underflow if infinity or NaN; ie exp == 0x7FF
*
*;*****************************************************************************
*;  Subtract DEN_EXP from NUM_EXP and store in RES_EXP for later processing   *
*;*****************************************************************************
*
	MOV	AH, NUM_EXP	; Load Numerator exponent
	SUB	AH, DEN_EXP	; Subtract denominator exponent from it
	MOV	RES_EXP, AH	; Store the result in RES_EXP
*
*;*****************************************************************************
*;       DIVISION                                                             *
*;  Division is implemented by recursive subtraction. The following algorithm *
*;  is used.                                                                  *
*;  1. Zero out the Result Quotient (RES_QH:RES_QL or RES_Q)                  *
*;  2. Load 53 to CNT, the loop counter.                                      *
*;  3. If numerator mantissa (NUM_HM:NUM_LM or NUM_M) is less than            *
*;     denaminator mantissa (DEN_HM:DEN_LM or DEN_M) goto step 6.             *
*;  4. Set NUM_M to NUM_M - DEN_M.                                            *
*;  5. Left shift RES_Q by 1 and add 1. Then goto step 7.                     *
*;  6. Left shift RES_Q by 1.                                                 *
*;  7. Left shift NUM_M by 1.                                                 *
*;  8. If NUM_M is zero goto step 10.                                         *
*;  9. If CNT-- is zero goto step 3.                                          *
*; 10. If CNT is -1 left shift RES_Q by 10                                    *
*;     else left shift RES_Q by CNT + 10                                      *
*;                                                                            *
*;     The result quotient is in bits 63 - 11.                                *
*;                                                                            *
*;*****************************************************************************
*
	MOVB	ACC, #0		;
	MOVL	RES_HQ, ACC	; Zero out the Quotient RES_HQ:RES_LQ
	MOVL	RES_LQ, ACC	;
	MOV 	CNT, #53	; Load the loop count         
	MOVL	ACC, NUM_HM	; Load the NUM_M     
	MOVL	P, NUM_LM	;
DIV_START:   
	MOV	TMP16, #0	; Assume NUM_M < DEN_M and set TMP16 to 0
	CMPL	ACC, DEN_HM	; Test if NUM_M < DEN_M  
	B	$1, NEQ		;
	MOVL	ACC, P
	CMPL	ACC, DEN_LM	;
$1:	B	$2, LO		; If NUM_M < DEN_M go to $2 
	MOVL	ACC, NUM_LM	; Subtract DEN_M from NUM_M                 
	SUBUL	ACC, DEN_LM	;
	MOVL	NUM_LM, ACC	; and store the result back in NUM_M
	MOVL	ACC, NUM_HM	;
	SUBBL	ACC, DEN_HM	;
	MOVL	NUM_HM, ACC	;  
	MOV 	TMP16, #1	; Set TMP16 to 1 to indicate NUM_M > DEN_M
$2:
	MOVL	ACC, RES_HQ	; Left shift the quotient in RES_HQ:RES_LQ by 1
	MOVL	P,   RES_LQ	;
	LSL64	ACC:P, #1	;
	MOVL	RES_HQ, ACC	; 
	MOVL	ACC, P		;
	ADD     AL, TMP16	; and add TMP16 to it
	MOVL	RES_LQ, ACC  	; Store the quotient back in RES_HQ:RES_LQ
	
        MOVL    ACC, NUM_HM     ; Load NUM_M into ACC:P
	MOVL    P, NUM_LM       ;
	LSL64	ACC:P, #1	; Left shift NUM_M by 1
	MOVL	NUM_HM, ACC	; Store NUM_M            
	MOVL	NUM_LM, P	; 
	B	DIV_END, EQ	; If NUM_M become zero, exit the loop.
	BANZ	DIV_START, CNT--; Continue the loop till --CNT becomes zero.
*
*;*****************************************************************************
*;  Move the quotient to bits 63 - 11 and normalize.                          *
*;*****************************************************************************
*
DIV_END:
	INC	CNT		; If ++CNT 
	B       NORMALIZE, NEQ	; is not zero add 9 to CNT
	INC	CNT		; else (++CNT is zero), add 10 to CNT
NORMALIZE:
	ADD 	CNT, #9		; 
	MOV	T, CNT		; 
	MOVL	ACC, RES_HQ	; Load the result quotient to ACC:P            
	MOVL	P,   RES_LQ	;
	LSL64	ACC:P, T	; Shift RES_Q by required number of bits 
	B	ROUND, LT 	; If the MSB of RES_Q is 0 
	LSL64	ACC:P, 1	; shift left RES_Q by 1 and 
	DEC	RES_EXP		; adjust the exponent.
*
*;*****************************************************************************
*;  Round the result mantissa to the nearest value by using ROUND64 macro.    *
*;*****************************************************************************
*
ROUND:
	LSR64	ACC:P, 1	; Move the results to 62 - 10 for rounding.
        ROUND64 RES_EXP, TMP32  ; Round the result
*
*;*****************************************************************************
*;  Then place the mantissa in bits 52 - 0 and remove the implied one         *
*;  for packing.                                                              *
*;*****************************************************************************
*
        LSR64   ACC:P, #10      ; Shift the mantissa with imp 1 to bits 53-0
        AND     AH, #0x000F     ; Removed the implied one
*
*;*****************************************************************************
*;  Adjust the bias by adding 1023 to the exponent and check for underflow    *
*;  and overflow.                                                             *
*;*****************************************************************************
*
	ADD	RES_EXP, #0x3FF
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
* ACC:P - final result
*
RETURN_VALUE:
        MOVL    RES_LQ, P     ;
        MOVL    RES_HQ, ACC   ; Load the return value in the return area.
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
