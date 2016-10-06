        .width 96
;******************************************************************************
;*                                                                            *
;*  ULL_TOFD v15.12.3 
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
;*  ULL$$TOFD  - Convert an unsigned 64 bit integer into an IEEE 754 format   *
;*               double precision floating point number			      *
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
;*   o INPUT OP1 IS IN ACC:P
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
;*                                                                            *
;*                                                  register file             *
;*                                                +-------------------+       *
;*                                           AR5  |        | RES_EXP  |       *
;*                                                +-------------------+       *
;*                                           AR7  |        | TMP16    |       *
;*                                                +-------------------+       *
;*                                                                            *
;******************************************************************************
        .page
	.include  "fd_util28.inc"
        .global   ULL$$TOFD
        .sect     ".text"

ULL$$TOFD	.asmfunc stack_usage(2)
        .asg      XAR5,    XRES_EXP
        .asg       AR5,    RES_EXP
        .asg       AR7,    TMP16   

        .asg   *+XAR6[2],  RES_MSW
        .asg   *+XAR6[0],  RES_LSW

*
*;*****************************************************************************
*;    1 Handle the special case where input is zero                           *
*;    2 Load the initial exponent value 0x41D                                 *
*;    3 If input is negative (MSB is 1)                                       *
*;        - Right shift by one so that MSB is in bit 62                       *
*;        - Set the exponent to  0x3FF + 3F = 0x43E                           *
*;    4 If input is positive (MSB is 0)                                       *
*;        - Normalize the mantissa so that MSB is in bit 62                   *
*;        - Set the exponent to 0x41D - nn, where nn is the number of bits    *
*;          shifted out for normalization.                                    *
*;*****************************************************************************
*
	CMP64	ACC:P			; Clear V flag
	CMP64	ACC:P			;
	B	OP_ZERO, EQ		; Return zero if ACC is zero

	MOVL	XRES_EXP, #0x43D	; Load the initial exponent value
	B	NORMALIZE, GEQ		; If bit 63 is 0 normalize.
	INC	RES_EXP			; If bit 63 is 1 no need to normalize,
	LSR64	ACC:P,#1 	        ; just shift right by 1 and adjust the
	B	PACK,UNC		; exponent and start packing.

NORMALIZE:
	NORMALIZE64  RES_EXP, TMP16	; Normalize the mantissa 
	
*
*;*****************************************************************************
*;       PACK mantissa and exponent into Double precision format.             *
*;*****************************************************************************
*
PACK:
	LSR64   ACC:P  #10	; 
	MOVL	RES_LSW, P	; Store the low 32 bits of the result

	LSR64   ACC:P, #4	; Move the mantissa out of AH. Since we already
				; saved the lower 32 bits to result we are okay.
        MOV     AH, RES_EXP     ; Load exponent and remove implied one 
        LSL64   ACC:P, #4       ; Shift back mantissa and exponent
        MOVL    RES_MSW, ACC    ; Load the return value in the return area.
	LRETR
*
RETURN_VALUE:
        MOVL    RES_LSW, P     ;
        MOVL    RES_MSW, ACC   ; Load the return value in the return area.
RETURN:
        LRETR

        .page
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
