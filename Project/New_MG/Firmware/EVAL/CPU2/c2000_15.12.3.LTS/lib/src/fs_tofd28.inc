        .width 96
;******************************************************************************
;*                                                                            *
;*  FS_TOFD v15.12.3 
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
;*  FS$$TOFD - Convert an IEEE 754 format single precision floating point     *
;*             number to 754 format double precision floating point number    *
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
;*   o INPUT OP1 IS IN ACC
;*   o RESULT IS RETURNED IN *XAR6
;*
;*   o SIGNALING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS +/- INFINITY
;*       (0x7ff00000:00000000) or (0xfff00000:00000000)
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
;*                             						      *
;*                              		    register file	      *
;*                                                +-------------------+       *
;*                                           AR4  |        | RES_EXP  |       *
;*                                                +-------------------+       *
;*                                           AR5  |        | RES_SIGN |       *
;*                                                +-------------------+       *
;*                                          XAR6  | PTR TO RET VALUE  |       *
;*                                                +-------------------+       *
;*                                                                            *
;******************************************************************************
        .page
	.include  "fd_util28.inc"
        .global   FS$$TOFD
        .sect     ".text"

FS$$TOFD:	.asmfunc
        .asg       AR4,    RES_EXP
        .asg       AR5,    RES_SIGN
        .asg   *+XAR6[2],  RES_MSW
        .asg   *+XAR6[0],  RES_LSW

*
*;*****************************************************************************
*;       CHECK FOR NULL POINTER                                               *
*;*****************************************************************************
*
	MOVL	P, ACC		  ; Save input float
        MOVL    ACC, XAR6         ;
        B       RETURN, EQ        ; Return if return pointer is null
*
*;*****************************************************************************
*;    1 Save the sign of the input                                            *
*;    2 Extract the exponent                                                  *
*;*****************************************************************************
*
	MOVL	ACC, P		; Restore input in ACC
	MOV	RES_SIGN, AH	; Save the sign of the input float

	MOV	P, #0		; Zero out P 

	LSR64   ACC:P, 7 	; Move the 8 bit exponent into AH bits 7 - 0 
	ANDB	AH, #0xFF	; Mask out the sign; keep only the exponent
*
*;*****************************************************************************
*;  Check input for the following special cases.                              *
*;   - If exp is zero, input is zero or denormal - Underflow                  *
*;   - If exp is 0xFF, input is infinite or NaN - Overflow                    *
*;*****************************************************************************
*
	B	UNDERFLOW, EQ	; Underflow if input is zero/denormals

	CMPB	AH, #0xFF	; Check if input number is infinite
	B  	OVERFLOW, EQ	; Overflow if input is infinite
*
*;*****************************************************************************
*;    1 Adjust the bias to make it a double precision bias(1023)              *
*;    2 Pack the sign, exponent and mantissa in double precision format       *
*;*****************************************************************************
*
	ADD	AH, #0x380	; Adjust the bias for double precision float

	LSL64	ACC:P, 4	; Move exponent and mantissa to bits 62 -      

	AND	RES_SIGN, #0x8000 ; Mask the sign bit
	OR	AH, RES_SIGN	; Set the sign bit
*
RETURN_VALUE:
        MOVL    RES_LSW, P     ;
        MOVL    RES_MSW, ACC   ; Load the return value in the return area.
RETURN:
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
