        .width 96
;******************************************************************************
;*                                                                            *
;*  FD_TOFS v15.12.3 
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
;*  FD$$TOFS  - Convert an IEEE 754 format double precision floating point    *
;*              number to 754 format single precision floating point number   *
;*                                                                            *
;******************************************************************************
;******************************************************************************
;*                                                                            *
;*  revision:  original                                                       *
;*                                                                            *
;******************************************************************************
;******************************************************************************
;******************************************************************************
;*
;*   o INPUT OP1 IS IN *XAR4
;*   o RESULT IS RETURNED IN R0H
;*   o P IS DESTROYED                 
;*
;*   o SIGNALING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS +/- INFINITY (0x7F800000/FF800000)
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o UNDERFLOW RETURNS ZERO (0x00000000)
;*   o ROUNDING MODE:  ROUND TO NEAREST
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
;****************************************************************************
;*
;* +--------------------------------------------------------------+
;* | SINGLE PRECISION FLOATING POINT FORMAT                       |
;* |   32-bit representation                                      |
;* |   31 30    23 22                    0                        |
;* |   +-+--------+-----------------------+                       |
;* |   |S|    E   |           M           +                       |
;* |   +-+--------+-----------------------+                       |
;* |                                                              |
;* |   <S>  SIGN FIELD    :        0 - POSITIVE VALUE             |
;* |                               1 - NEGATIVE VALUE             |
;* |                                                              |
;* |   <E>  EXPONENT FIELD:       00 - ZERO IFF M == 0            |
;* |                         01...FE - EXPONENT VALUE (127 BIAS)  |
;* |                              FF - INFINITY                   |
;* |                                                              |
;* |   <M>  MANTISSA FIELD:  FRACTIONAL MAGNITUDE WITH IMPLIED 1  |
;* +--------------------------------------------------------------+
;*
;******************************************************************************
;*                                                                            *
;*                                                  register file             *
;*                                                +-------------------+       *
;*                                          XAR4  |    PTR TO OP1     |       *
;*                                                +-------------------+       *
;*                                           AR4  |     RES_EXP       |       *
;*                                                +-------------------+       *
;*                                           AR6  |     RES_SIGN      |       *
;*                                                +-------------------+       *
;*                                                                            *
;******************************************************************************
        .page
	.include  "fd_util28.inc"
        .global   FD$$TOFS
        .sect     ".text"

FD$$TOFS:	.asmfunc
        .asg       AR4,    RES_EXP
        .asg       AR6,    RES_SIGN
        .asg   *+XAR4[3],  OP_SIGN
        .asg   *+XAR4[2],  OP_MSW
        .asg   *+XAR4[0],  OP_LSW

*
*;*****************************************************************************
*;      SAVE SIGN OF INPUT                                                    *
*;*****************************************************************************
*
        MOV     RES_SIGN, OP_SIGN ; Save the sign
*
*;*****************************************************************************
*;      UNPACK AND SIGN ADJUST MANTISSA                                       *
*;  1. Extract the exponent in RES_EXP                                        *
*;  2. Extract the mantissa in ACC:P in bits 62 to 11                         *
*;  3. Add the rounding constant to mantissa and adjust the exponent if needed*
*;*****************************************************************************
*
	MOVL	ACC, OP_MSW	;
	MOVL	P, OP_LSW	; Load OP to ACC:P

	LSL64	ACC:P, 1	; Shift out the sign bit
	LSR64	ACC:P, 5	; Shift so that AH has exp and AL:P has mantissa

	MOV	RES_EXP, AH	; Store the result exponent

	LSL64	ACC:P, 16	; Move SP mantissa to bits 31 to 9 of ACC
	LSR64	ACC:P, #1	; Clear the MSB to capture the overflow.
	ADDB 	ACC, #0x80	; Add 1/2 for rounding   

	B	EXP_EVAL, GEQ	; If rounding generates carry,
	AND	AH, #0x7FFF	; Clear the carry bit.
	INC	RES_EXP		; and increment the exponent 

*
*;*****************************************************************************
*;       EXPONENT EVALUATION                                                  *
*;  Test the exponent for underflow and overflow.                             *
*;*****************************************************************************
*
EXP_EVAL:
	LSR64	ACC:P, 15 	; Move the Mantissa to AL:P so that AH is free 

	MOV	AH, RES_EXP	; Restore exponent
	SUB	AH, #0x380	; Adjust the bias
	B	UNDERFLOW, LT	; If single precision biased exp is < 0
				; underflow
	CMP	AH, #0xFF	; If single precision biased exp is > 0xFF,
	B	OVERFLOW, GT	; overflow

	LSL64	ACC:P, 7	; Shift the exponent and mantissa into ACC 
	AND	RES_SIGN, #0x8000 ; Mask the sign bit
	OR	AH, RES_SIGN	; Set the sign bit

RETURN_VALUE:
        MOV32   R0H, ACC          ; Move the result to FPU return register
        LRETR

        .page
*;*****************************************************************************
*;       OVERFLOW PROCESSING                                                  *
*;  Set ACC to 0x8000:0000 if sign is negative; otherwise to 0x7fff:ffff      *
*;*****************************************************************************
*
OVERFLOW:
	AND	AH, RES_SIGN, #0x8000 ;
	OR	AH, #0x7F80	;
	MOVB	AL, #0		;
        B       RETURN_VALUE,UNC
*
*;*****************************************************************************
*;       UNDERFLOW PROCESSING                                                 *
*;  Set ACC to 0x0000:0000                                                    *
*;*****************************************************************************
*
UNDERFLOW:
        MOVB	ACC, #0           ; For underflow result in ACC = 0
        B       RETURN_VALUE,UNC
	.endasmfunc
