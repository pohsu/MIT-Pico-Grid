        .width 96
;******************************************************************************
;*                                                                            *
;*  ULL_TOFS v15.12.3 
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
;*  ULL$$TOFS  - Convert an unsigned 64 bit integer into an IEEE 754 format   *
;*               single precision floating point number			      *
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
;*   o RESULT IS RETURNED IN ACC
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
;*                                                                            *
;*                                                  register file             *
;*                                                +-------------------+       *
;*                                           AR5  |        | EXPONENT |       *
;*                                                +-------------------+       *
;*                                           AR6  |        | TMP16    |       *
;*                                                +-------------------+       *
;*                                                                            *
;******************************************************************************
        .page
	.include  "fd_util28.inc"
        .global   ULL$$TOFS
        .sect     ".text"

ULL$$TOFS:	.asmfunc stack_usage(2)
        .asg      XAR5,    XEXPONENT
        .asg       AR5,    EXPONENT
        .asg       AR6,    TMP16   
*
*;*****************************************************************************
*;    1 Handle the special case where input is zero                           *
*;    2 Load the initial exponent value 0x7F + 3F = 0xBE                      *
*;    3 If input MSB is 1                                                     *
*;        - Right shift by one so that MSB is in bit 62                       *
*;        - Set the exponent to  0x7F + 3F = 0xBE                             *
*;    4 If input is positive (MSB is 0)                                       *
*;        - Normalize the mantissa so that MSB is in bit 62                   *
*;        - Set the exponent to 0xBD - nn, where nn is the number of bits     *
*;          shifted out for normalization.                                    *
*;*****************************************************************************
*
	CMP64	ACC:P			; Clear V flag
	CMP64	ACC:P			;
	B	RETURN, EQ		; Return zero if ACC is zero

	MOVB	XEXPONENT, #0xBD	; Load the initial exponent to 0xBD
	B	NORMALIZE, GT		; If MSB is 0 normalize the value.

	INC	EXPONENT		; MSB is 1 so inc the exp to 0xBE
	LSR64	ACC:P, #1		; Right shift so that MSB is in bit 62
	B	PACK,UNC		; No need to normalize, go to pack.

NORMALIZE:
	NORMALIZE64  EXPONENT, TMP16	; Normalize the mantissa 
	
*
*;*****************************************************************************
*;       CONVERSION OF FLOATING POINT FORMAT - PACK                           *
*;  Mask  mantissa   [00MM MMMM MMMM MMMM MMMM MMMM MMMM MMMM]                *
*;  Shift mantissa   [0000 0000 0MMM MMMM MMMM MMMM MMMM MMMM]                *
*;  Pack exponent.   [0EEE EEEE EMMM MMMM MMMM MMMM MMMM MMMM]                *
*;  Add sign         [SEEE EEEE EMMM MMMM MMMM MMMM MMMM MMMM]                *
*;*****************************************************************************
*
PACK:
        AND     AH, #03FFFh         ; remove implied one and sign bit
	LSR64	ACC:P, #7	    ; shift mantissa to make room for exponent
        MOV     TMP16, AH           ; save AH
        MOV     AH, EXPONENT
        LSL     AH, 7               ; adjust exponent
        OR      AH, TMP16           ; restore AH with exponent
*
RETURN:
        LRETR
	.endasmfunc
