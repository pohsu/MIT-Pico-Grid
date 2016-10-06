        .width 96
;******************************************************************************
;*                                                                            *
;*  FD_TOL v15.12.3 
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
;*  FD$$TOL  - Convert an IEEE 754 format double precision floating point     *
;*             number into a signed 32 bit integer			      *
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
;*   o RESULT IS RETURNED IN ACC  
;*
;*   o SIGNALING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS SATURATED VALUES (0x7fffffff or 0x80000000)
;*   o DENORMALIZED NUMBERS ARE TREATED AS UNDERFLOWS
;*   o UNDERFLOW RETURNS ZERO (0x00000000)
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
;*                                                  register file             *
;*                                                +-------------------+       *
;*                                          XAR4  | POINTER TO INPUT  |       *
;*                                                +-------------------+       *
;*                                           AR5  |        | OP1 EXP  |       *
;*                                                +-------------------+       *
;*                                           AR6  |        | RES_SIGN |       *
;*                                                +-------------------+       *
;*                                                                            *
;******************************************************************************
        .page
	.include  "fd_util28.inc"
        .global   FD$$TOL
        .sect     ".text"

FD$$TOL:	.asmfunc
        .asg       AR5,    OP1_EXP
        .asg       AR6,    RES_SIGN
        .asg   *+XAR4[3],  OP1_SIGN
        .asg   *+XAR4[2],  OP1_MSW
        .asg   *+XAR4[0],  OP1_LSW

*
*;*****************************************************************************
*;      SAVE SIGN OF INPUT                                                    *
*;*****************************************************************************
*
        MOV     RES_SIGN, OP1_SIGN  ; Save the sign
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
*;    Case 2:  exponents > 041Dh; 041Dh is the exponent for integer values    *
*;             in the absolute range from 1073741824 to 2147483648.           *
*;             Result returned is 7FFFFFFFh or 80000000h                      *
*;    Case 3:  exponents in the range of 03FFh to 041Dh inclusive will result *
*;             in 32-bit signed integer values from -2MB to +2MB              *
*;*****************************************************************************
*
	SUB	OP1_EXP, #0x3FF	; if exponent is < 0x3FF, underflow
	B	UNDERFLOW, LT	;

	MOV 	PH, AH 		; Save Mantissa Hi16 
	MOVB	AH, #31		; Subtract exponent from 31 to find shift value
				; for denormalization. 
	SUB	AH, OP1_EXP	; If unbiased exponent is >= 31, overflow. That
	B	OVERFLOW, LEQ	; is overflow, if shift value in AH is negative 
				; or zero.
	MOV	T, AH  		; Move the shift value to T for denormalization
	MOV	AH, PH		; Restore Mantissa Hi16
	LSRL	ACC, T		; Denormalize 

	CMP	RES_SIGN, #0	; If input is negative,     
	B	RETURN_VALUE, GEQ	
	NEG	ACC		; NEG will not overflow due to the LSRL
  
RETURN_VALUE:
        LRETR

        .page
*;*****************************************************************************
*;       OVERFLOW PROCESSING                                                  *
*;  Set ACC to 0x8000:0000 if sign is negative; otherwise to 0x7fff:ffff      *
*;*****************************************************************************
*
OVERFLOW:
	MOV	OVC, RES_SIGN	; Load OVC with bits 15-10 of RES_SIGN, which
				; is sign bit and 5 MSB of exponent 
	SAT	ACC		; If OVC > 0 ACC = 0x7fff:ffff
				; else if OVC < 0, ACC = 0x8000:0000 
				; When OVC = 0, ACC is not modified. For this 
				; case to happen, 5 MSBs of exp should be zero
				; which means the exponent is less than 0x3FF 
				; and should have been underflowed.           
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

