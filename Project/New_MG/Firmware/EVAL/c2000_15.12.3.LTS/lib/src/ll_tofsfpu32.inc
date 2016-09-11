        .width 96
;******************************************************************************
;*                                                                            *
;*  LL_TOFS v15.12.3 
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
;*  LL$$TOFS  - Convert a signed 64 bit integer into an IEEE 754 format       *
;*              single precision floating point number			      *
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
;*   o RESULT IS RETURNED IN R0H
;*
;*       NOTE : POSSIBLE LOSS OF PRECISION SINCE 32-BIT FLOAT HAS ONLY         
;*              24 BITS OF PRECISION                                           
;******************************************************************************
;*                                                                            *
;*  Floating Point Format - Single Precision                                  *
;*                                                                            *
;*                                                                            *
;*       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
;*       |31 |30 |29 |28 |27 |26 |25 |24 |23 |22 |21 |20 |19 |18 |17 |16 |    *
;*       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
;*       |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |    *
;*       | S |E7 |E6 |E5 |E4 |E3 |E2 |E1 |E0 |M22|M21|M20|M19|M18|M17|M16|    *
;*       |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |    *
;*       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
;*                                                                            *
;*       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
;*       |15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |    *
;*       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
;*       |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |    *
;*       |M15|M14|M13|M12|M11|M10|M9 |M8 |M7 |M6 |M5 |M4 |M3 |M2 |M1 |M0 |    *
;*       |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |    *
;*       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
;*                                                                            *
;*                                                                            *
;*       Single precision floating point format is a 32 bit format            *
;*       consisting of a 1 bit sign field, an 8 bit exponent field, and a     *
;*       23 bit mantissa field.  The fields are defined as follows.           *
;*                                                                            *
;*            Sign <S>          : 0 = positive values; 1 = negative values    *
;*                                                                            *
;*            Exponent <E7-E0>  : offset binary format                        *
;*                                00 = special cases (i.e. zero)              *
;*                                01 = exponent value + 127 = -126            *
;*                                FE = exponent value + 127 = +127            *
;*                                FF = special cases (not implemented)        *
;*                                                                            *
;*            Mantissa <M22-M0> : fractional magnitude format with implied 1  *
;*                                1.M22M21...M1M0                             *
;*                                                                            *
;*            Range             : -1.9999998 e+127 to -1.0000000 e-126        *
;*                                +1.0000000 e-126 to +1.9999998 e+127        *
;*                                (where e represents 2 to the power of)      *
;*                                -3.4028236 e+38  to -1.1754944 e-38         *
;*                                +1.1754944 e-38  to +3.4028236 e+38         *
;*                                (where e represents 10 to the power of)     *
;******************************************************************************

        .page
        .global   LL$$TOFS
        .sect     ".text"

LL$$TOFS:	.asmfunc stack_usage(2)
        .asg       R2H, SIGN
*
*;*****************************************************************************
*;    1 Handle the special case where input is zero                           *
*;    2 Save the sign of the input                                            *
*;    3 Get the absolute value of input                                       *
*;    4 Convert absolute value (unsigned long long) to single precision float *
*;    5 Change the sign of the number if input was negative to start with     *
;*                                                                            *
;* Modifies R1H and R2H which are both SOC registers                          *
*;*****************************************************************************
*
        MOV32      R0H, ACC             ; move upper half to R0H - move is
                                        ; done here for the input = 0 case

        MOVF32     SIGN, #0.0           ; initialize sign to 0.0 (positive)
	CMP64	   ACC:P		; Clear V flag
	CMP64	   ACC:P		;
	B	   RETURN, EQ		; Return zero if ACC:P is zero
	B	   CONVERT, GT		; Convert immediately if positive number
        MOVF32     SIGN, #1.0           ; record sign as 1.0 (negative)
        NEG64      ACC:P                ; get absolute value
        MOV32      R0H, ACC             ; move upper half to R0H again
  
        ; From now on treat as though input is unsigned long long
CONVERT:
        MOV32      R1H, P               ; move lower half to R1H
        NOP                             ; The MOV32's to R0H and R1H are
        NOP                             ; CPI8 operations needing 4 cycle 
        NOP                             ; delay before result can be accessed.
        UI32TOF32  R0H, R0H             ; convert upper half to float
        UI32TOF32  R1H, R1H             ; convert lower half to float

        MPYF32     R0H, #0x4f80, R0H    ; value of upper half = upper half
                                        ; bits, multiplied by 2 ^ 32.
                                        ; 2 ^ 32 = 0x4f800000. MPYF32
                                        ; needs only the upper 16 bits
                                        ; and automatically zeroes the rest.
        NOP
        ADDF32     R0H, R1H, R0H

        ; Check the sign and negate result if necessary
        CMPF32     SIGN, #1.0
        NEGF32     R0H, R0H, EQ         ; negate if input was negative

RETURN:
        ; Result is in R0H already
        LRETR
	.endasmfunc
