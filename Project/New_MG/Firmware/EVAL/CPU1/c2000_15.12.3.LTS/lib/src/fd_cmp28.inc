        .width 96
;******************************************************************************
;*                                                                            *
;*  FD_CMP v15.12.3 
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
;*  FD$$CMP - Compare two IEEE 754 format double precision floating point     *
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
;*   o RESULT IS RETURNED IN AL    
;*   o INPUT OP1 AND OP2 ARE PRESERVED                                 
;*
;*   o SIGNALING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o +0 == -0  (This is not implemented yet)
;*
;*   o IF COMPUTING THE RESULT INVOLVES INFINITIES, THE FOLLOWING TABLE
;*     SUMMARIZES THE EFFECTIVE RESULT
;*                   +----------+----------+----------+
;*                   + OP2 !INF | OP2 -INF + OP2 +INF +
;*        +----------+==========+==========+==========+
;*        + OP1 !INF +    -     |   +INF   +   -INF   +
;*        +----------+----------+----------+----------+
;*        + OP1 -INF +   -INF   |    -0    +   -INF   +
;*        +----------+----------+----------+----------+
;*        + OP1 +INF +   +INF   |   +INF   +    +0    +
;*        +----------+----------+----------+----------+
;*
;*   o THE RESULT OF THE COMPARE IS COMPUTED USING INTEGER SUBTRACT IF THE
;*     SIGN OF THE INPUTS IS THE SAME.  THE TABLE SUMMARIZES THE
;*     IMPLEMENTATION.
;*                 +-----------+-----------+
;*                 +   OP2 +   |   OP2 -   +
;*        +--------+===========+===========+
;*        + OP1 +  + OP1 - OP2 |    OP1    +
;*        +--------+-----------+-----------+
;*        + OP1 -  +    OP1    | OP2 - OP1 +
;*        +--------+-----------+-----------+
;*   
;*   o RETURN VALUE IN AL 
;*     OP1 > OP2  : return 1
;*     OP1 < OP2  : return -1
;*     OP1 == OP2 : return 0
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
;*                 stack                            register file             *
;*           +----------------+                   +-------------------+       *
;*      SP-->|                |             XAR4  |   PTR TO OP1      |       *
;*           +----------------+                   +-------------------+       *
;*        -1 | (return addr)  |             XAR5  |   PTR TP OP2      |       *
;*           +----------------+                   +-------------------+       *
;*        -2 | (return addr)  |              AR7  |     RESULT        |       *
;*           +----------------+                   +-------------------+       *
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
	.include  "fd_util28.inc"
        .global   FD$$CMP
        .sect     ".text"

FD$$CMP:	.asmfunc
	.asg	   AR7,	   RESULT 

        .asg   *+XAR4[3],  OP1_SIGN
        .asg   *+XAR4[2],  OP1_MSW
        .asg   *+XAR4[0],  OP1_LSW
        .asg   *+XAR5[3],  OP2_SIGN
        .asg   *+XAR5[2],  OP2_MSW
        .asg   *+XAR5[0],  OP2_LSW
*
*;*****************************************************************************
*;      COMPARE THE OPERANDS AND SET THE RESULT                               *
*;  The 64 bit compare is done in two parts                                   *
*;    1. Compare most significant 32 bits of the operands (OP1_MS32, OP2_MS32)*
*;     - If OP1_MS32 > OP2_MS32, RESULT is 1                                  *
*;     - If OP1_MS32 < OP2_MS32, RESULT is -1                                 *
*;    2. If they are same do unsigned compare on the least significant 32 bits*
*;     - If OP1_LS32 > OP2_MS32, RESULT is 1                                  *
*;     - If OP1_LS32 < OP2_MS32, RESULT is -1                                 *
*;     - If the low 32 bits are also same, RESULT is 0                        *
*;                                                                            *
*; NOTE: The COND MOVB instruction cannot be used to load -1 to RESULT so the *
*;       following code loads 0, 1 or 2 for LT, EQ or GT respectively. The    *
*;       result will be decremented later by 1.                               *
*;*****************************************************************************
*
        MOVL    ACC, OP1_MSW    ; LOAD OP1 (MSW)

	MOV 	RESULT, #1     	; Set result to 0; change this if ops are diff 
	CMPL	ACC, OP2_MSW	;
	MOVB	RESULT, #2, GT  ; Set result to 1 if OP1 > OP2
	MOVB	RESULT, #0, LT  ; Set result to -1 if OP1 < OP2
	B	CHECK_INF, NEQ  ;
	MOVL	ACC, OP1_LSW	;
	CMPL	ACC, OP2_LSW	;
	MOVB	RESULT, #2, HI  ; Set result to 1 if OP1 > OP2
	MOVB	RESULT, #0, LO  ; Set result to -1 if OP1 < OP2
	B  	OP_EQUAL, EQ 	; Operands are same; return zero
*
*;*****************************************************************************
*;      HANDLE THE SPECIAL CASE OF INFINITY.                                  *
*;   If both the operands are infinity the signs determine the result.        *
*;*****************************************************************************
*
CHECK_INF:
	MOVL	ACC, OP1_MSW	; Restore OP1 MSW
	LSL	AH, 1		; Shift out the sign
	CMP	AH, #0xFFE0	;
	B	RETURN, LO 	; If OP1 is not infinite, the compare we did 
				; initially is valid. So return that value.

        ; OP1 is infinite, now check OP2
	MOVL	ACC, OP2_MSW	; Load OP2 MSW
	LSL	AH, 1		; Shift out sign
	CMP	AH, #0xFFE0	; 
	B	RETURN, LO	; If OP2 is not infinite, return the cmp result

	; Both ops are infinite check the sign 
	MOVL	ACC, OP1_MSW	;
	LSR	AH, 15		; Shift the sign bit to bit 0
	MOV	AL, OP2_SIGN	;
	LSR	AL, 15		; Shift the sign bit to bit 0
	SUB	AL, AH		; OP2 sign - OP1 sign is the result 
	LRETR			; Return
*
*;*****************************************************************************
*;      SET THE RESULT IN AL AND RETURN                                       *
*;  IF both inputs are negative negate the RESULT before returning.           *
*;*****************************************************************************
*
RETURN:
	DEC	RESULT		; Transform [0 1 2] tp [-1 0 1]
	SETC	SXM		; Set SXM mode correctly for the MOV
	MOV 	ACC, OP1_SIGN   ;
	MOV 	AL, RESULT	; Load the result into AL                
	AND	AH, OP2_SIGN	; Test if both the operands are negative
	B	NO_NEG, GEQ	; 
	NEG	AL 		; If both ops are negative, negate the result 
NO_NEG:
	LRETR

OP_EQUAL:
	MOVB	AL, #0		; Return 0 in AL
	LRETR
	.endasmfunc
