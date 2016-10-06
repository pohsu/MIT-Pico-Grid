        .width 96
;******************************************************************************
;*                                                                            *
;*  L_TOFD v15.12.3 
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
;*  L$$TOFD  - Convert a signed 32 bit integer into an IEEE 754 format double *
;*             precision floating point number    			      *
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
        .global   L$$TOFD
        .sect     ".text"

L$$TOFD		.asmfunc stack_usage(2)
        .asg      XAR4,    XRES_EXP
        .asg       AR4,    RES_EXP
        .asg       AR5,    RES_SIGN
        .asg   *+XAR6[2],  RES_MSW
        .asg   *+XAR6[0],  RES_LSW

*
*;*****************************************************************************
*;       CHECK FOR NULL POINTER                                               *
*;*****************************************************************************
*
        MOVL    P, ACC            ; Save input float
        MOVL    ACC, XAR6         ;
        B       RETURN, EQ        ; Return if return pointer is null
*
*;*****************************************************************************
*;    1 Handle the special case where input is zero                           *
*;    2 Save the sign of the input                                            *
*;    3 Get the absolute value of input                                       *
*;    4 Normalize the value and adjust the exponent                           *
*;*****************************************************************************
*
        MOVL    ACC, P          ; Restore input in ACC
	B	OP_ZERO, EQ	; Return zero if ACC is zero

	MOV	RES_SIGN, AH	; Save the sign

	MOVL	XRES_EXP, #0x41D; Normalize the value and adjust the exponent
	B	$10, GEQ	; 
        SB      1, OV           ; Clear V (overflow)                        
	CLRC	OVM		; Clear OVM to get desired result in NEG.
	NEG	ACC		; Take absolute value of input
	B	$10, NOV	; 
	INC	RES_EXP		; Negate overflows when input is 0x8000:0
				; Account for the overflow by incrementing exp.
$10:
	RPT	#31		;
     || NORM	ACC, XRES_EXP--	;
	
*
*;*****************************************************************************
*;     Pack the sign, mantissa and exponent into Double precision format      *
*;*****************************************************************************
*
	MOV 	P, #0 		; Zero out P

        LSR64   ACC:P, #14      ; Move the mantissa out of AH. Since only 32 
                                ; bits of the mantissa are significant this 
                                ; is not a problem
        MOV     AH, RES_EXP     ; Load exponent and remove implied one 
        LSL64   ACC:P, #4       ; Shift back mantissa and exponent

	AND	RES_SIGN, #0x8000 ; Mask the sign bit
	OR	AH, RES_SIGN	; Set the sign bit
*
RETURN_VALUE:
        MOVL    RES_LSW, P      ;
        MOVL    RES_MSW, ACC    ; Load the return value in the return area.

RETURN:
        LRETR
*
*;*****************************************************************************
*;       UNDERFLOW PROCESSING                                                 *
*;  Set ACC:P to 0x00000000:00000000                                          *
*;*****************************************************************************
*
OP_ZERO:
        ZAPA	                  ; For underflow result in ACC:P = 0
        B       RETURN_VALUE,UNC
	.endasmfunc
