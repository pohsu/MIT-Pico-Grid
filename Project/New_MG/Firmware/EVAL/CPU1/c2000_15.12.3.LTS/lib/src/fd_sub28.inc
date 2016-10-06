
        .width 96
;******************************************************************************
;*                                                                            *
;*  FD_SUB v15.12.3 
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
;*  FD$$SUB - Subtract an IEEE 754 format double precision floating point     *
;*            number from another                                             *
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
;*
;*   o SUBTRACTION, OP1 - OP2, IS IMPLEMENTED WITH ADDITION, OP1 + (-OP2)
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
;*
;*                   +----------+----------+----------+
;*       SUBTRACTION + OP2 !INF | OP2 -INF + OP2 +INF +
;*        +----------+==========+==========+==========+
;*        + OP1 !INF +    -     |   +INF   +   -INF   +
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
;*                 stack                                                      *
;*           +----------------+                                               *
;*      SP-->|                |                                               *
;*           +----------------+                                               *
;*     -1 -2 |  -VE OP2 MSW   |                                               *
;*           +----------------+                                               *
;*     -3 -4 |  -VE OP2 LSW   |                                               *
;*           +----------------+                                               *
;*     -5 -6 |  Return addr   |                                               *
;*           +----------------+                                               *
;*                                                                            *
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
        .global   FD$$SUB
        .global   FD$$ADD
        .sect     ".text"

FD$$SUB:	.asmfunc
	.asg   *-SP[1],    OP2_SIGN
        .asg   *-SP[2],    OP2_MSW
        .asg   *-SP[4],    OP2_LSW
        .asg   *+XAR5[2],  IN2_MSW
        .asg   *+XAR5[0],  IN2_LSW


	ADDB	SP, #4

	MOVL	ACC, IN2_MSW 	  ; Load OP2 (*XAR5) to the stack
	MOVL	OP2_MSW, ACC
	MOVL	ACC, IN2_LSW
	MOVL	OP2_LSW, ACC
*
*;*****************************************************************************
*; Negate the second operand and call ADD                                     *
*;*****************************************************************************
*
	XOR	OP2_SIGN, #0x8000 ; Negate second operand
	MOVZ	AR5, SP		  ;
	SUBB	XAR5, #4	  ;
	MOVZ	AR5, AR5 	  ; Load the address of OP2 (-ve input 2)
	LCR     FD$$ADD           ; Call ADD routine

	SUBB	SP, #4
	LRETR			  ; Return
	.endasmfunc
