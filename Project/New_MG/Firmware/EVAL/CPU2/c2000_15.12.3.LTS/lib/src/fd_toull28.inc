        .width 96
;******************************************************************************
;*                                                                            *
;*  FD_TOULL v15.12.3 
;*                                                                            *
;* Copyright (c) 2003-2016 Texas Instruments Incorporated                     *
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
;*  FD$$TOULL  - Convert an IEEE 754 format double precision floating point   *
;*               number into an unsigned 64 bit long long integer	      *
;*                                                                            *
;******************************************************************************
;******************************************************************************
;*                                                                            *
;*  revision:  original                                                       *
;*                                                                            *
;******************************************************************************
;*
;*   o INPUT OP1 IS IN *XAR4
;*   o RESULT IS RETURNED IN ACC:P
;*
;*   o SIGNALING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS 0xFFFFFFFF:FFFFFFFF                         
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o UNDERFLOW RETURNS ZERO (0x00000000:00000000)
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
;*                             						      *
;*                              		    register file	      *
;*                                                +-------------------+       *
;*                                          XAR4  | POINTER TO INPUT  |       *
;*                                                +-------------------+       *
;*                                           AR5  |        | OP1 EXP  |       *
;*                                                +-------------------+       *
;*                                           AR6  |        | TMP16    |       *
;*                                                +-------------------+       *
;*                                           AR7  |        | RES_SIGN |       *
;*                                                +-------------------+       *
;*                                                                            *
;******************************************************************************
        .page
	.include  "fd_util28.inc"
        .global   FD$$TOULL
        .sect     ".text"

FD$$TOULL:	.asmfunc
        .asg       AR5,    OP1_EXP
        .asg       AR6,    TMP16
        .asg       AR7,    RES_SIGN
        .asg   *+XAR4[3],  OP1_SIGN
        .asg   *+XAR4[2],  OP1_MSW
        .asg   *+XAR4[0],  OP1_LSW

*
*;*****************************************************************************
*;      Test OP for negative and return zero if negative.                     *
*;*****************************************************************************
*
        MOVL    ACC, OP1_MSW    ; Load the upper half of the operand 
        B       UNDERFLOW, LT   ; return zero if negative
*
*;*****************************************************************************
*;      UNPACK AND SIGN ADJUST MANTISSA                                       *
*;  1. Extract the exponent in OP1_EXP                                        *
*;  2. Extract the mantissa in ACC:P in bits 62 to 11                         *
*;  3. Add the implied one at bit 63                                          *
*;  5. Store mantissa in OP1_HM:OP1_LM                                        *
*;*****************************************************************************
*
        UNPACK64  XAR4, OP1_EXP ; Unpack OP1 into OP1_EXP and ACC:P
*
*;*****************************************************************************
*;       EXPONENT EVALUATION                                                  *
*;  Test the exponent to determine into which of the three cases it belongs.  *
*;    Case 1:  exponents < 03FFh; 03FFh is the exponent for integer value 1.  *
*;             Result returned is 0 since the absolute value is less than 1.  *
*;    Case 2:  exponents > 043Eh;                                             *
*;             Result returned is FFFFFFFFh                                   *
*;    Case 3:  exponents in the range of 03FFh to 043Eh inclusive will result *
*;             in 64-bit unsigned integer values from 0 to 4MB                *
*;*****************************************************************************
*
	SUB	OP1_EXP, #0x3FF	; if exponent is < 0x3FF, underflow
	B	UNDERFLOW, LT	;

	MOV 	TMP16, AH 	; Save Mantissa Hi16 
	MOVB	AH, #63		; Subtract exponent from 63 to find shift value
				; for denormalization. 
	SUB	AH, OP1_EXP	; If unbiased exponent is > 63, overflow. That
	B	OVERFLOW, LT 	; is overflow, if shift value in AH is negative 

	MOV	T, AH  		; Move the shift value to T for denormalization
	MOV	AH, TMP16	; Restore Mantissa Hi16
	LSR64	ACC:P, T	; Denormalize 

        LRETR

        .page
*;*****************************************************************************
*;       OVERFLOW PROCESSING                                                  *
*;  Set ACC:P to 0xFFFFFFFF:FFFFFFFF      				      *
*;*****************************************************************************
*
OVERFLOW:
	MOVB	ACC, #0		; 
	SUBB	ACC, #1		; Load ACC with unsigned saturation value
	MOVL    P, ACC      	;
        LRETR
*
*;*****************************************************************************
*;       UNDERFLOW PROCESSING                                                 *
*;  Set ACC:P to 0x00000000:00000000                                          *
*;*****************************************************************************
*
UNDERFLOW:
        ZAPA	                  ; For underflow result in ACC:P = 0
        LRETR
	.endasmfunc

