         .width 96
;******************************************************************************
;*                                                                            *
;*  FS$$TOU - convert a floating point value to a 16-bit unsigned integer     *
;*  FS_TOU v15.12.3                                                           *
;*                                                                            *
;* Copyright (c) 2000-2016 Texas Instruments Incorporated                     *
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
;*                                                                            *
;******************************************************************************
*;*****************************************************************************
*;                                                                            *
*;  revision:  original                                                       *
*;                                                                            *
*;*****************************************************************************
*;                                                                            *
*;       FS$$TOU                                             register file    *
*;                                                         +----------------+ *
*;       This routine converts a floating point        AR4 | OP sign & exp  | *
*;       value to a 16-bit unsigned integer.  Upon         +----------------+ *
*;       entry the floating point number is in         AR5 |16-bit magnitude| *
*;       ACC.  When the conversion is complete,            +----------------+ *
*;       the integer value will be in AL                                      *
*;                                                                            *
*;       inputs:  A (floating point value) in ACC                             *
*;                                                                            *
*;       implementation:  A is unpacked into sign,                            *
*;            exponent, and mantissa.                                         *
*;            If the exponent exceeds a value of 08Eh                         *
*;            then an overflow has occurred and a                             *
*;            saturated value will be returned.  For                          *
*;            all exponents less than 07Fh the value                          *
*;            of zero is returned.  Within the exponent                       *
*;            range of 07Fh through 08Eh, the                                 *
*;            denormalized result is truncacted to                            *
*;            fifteen bits and the properly signed                            *
*;            result is placed in AL.  Negative floating                      *
*;            point values are returned as zero.                              *
*;                                                                            *
*;       result:  returned in AL                                              *
*;                                                                            *
*;*****************************************************************************
         .page
*;*****************************************************************************
*;                                                                            *
*;  Floating Point Format - Single Precision                                  *
*;                                                                            *
*;                                                                            *
*;       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
*;       |31 |30 |29 |28 |27 |26 |25 |24 |23 |22 |21 |20 |19 |18 |17 |16 |    *
*;       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
*;       |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |    *
*;       | S |E7 |E6 |E5 |E4 |E3 |E2 |E1 |E0 |M22|M21|M20|M19|M18|M17|M16|    *
*;       |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |    *
*;       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
*;                                                                            *
*;       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
*;       |15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |    *
*;       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
*;       |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |    *
*;       |M15|M14|M13|M12|M11|M10|M9 |M8 |M7 |M6 |M5 |M4 |M3 |M2 |M1 |M0 |    *
*;       |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |    *
*;       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    *
*;                                                                            *
*;                                                                            *
*;       Single precision floating point format is a 32 bit format            *
*;       consisting of a 1 bit sign field, an 8 bit exponent field, and a     *
*;       23 bit mantissa field.  The fields are defined as follows.           *
*;                                                                            *
*;            Sign <S>          : 0 = positive values; 1 = negative values    *
*;                                                                            *
*;            Exponent <E7-E0>  : offset binary format                        *
*;                                00 = special cases (i.e. zero)              *
*;                                01 = exponent value + 127 = -126            *
*;                                FE = exponent value + 127 = +127            *
*;                                FF = special cases (not implemented)        *
*;                                                                            *
*;            Mantissa <M22-M0> : fractional magnitude format with implied 1  *
*;                                1.M22M21...M1M0                             *
*;                                                                            *
*;            Range             : -1.9999998 e+127 to -1.0000000 e-126        *
*;                                +1.0000000 e-126 to +1.9999998 e+127        *
*;                                (where e represents 2 to the power of)      *
*;                                -3.4028236 e+38  to -1.1754944 e-38         *
*;                                +1.1754944 e-38  to +3.4028236 e+38         *
*;                                (where e represents 10 to the power of)     *
*;*****************************************************************************
         .page
         .global   FS$$TOU

FS$$TOU		.asmfunc
	 .asg	AR4, OP_SE
	 .asg	AR5, RES_MAG

*
*;*****************************************************************************
*;       CONVERSION OF FLOATING POINT FORMAT - UNPACK                         *
*;  Test OP for special case treatment of zero.                               *
*;    Save the sign and exponent on the stack [xxxx xxxS EEEE EEEE].          *
*;    Add the implied one to the mantissa value.                              *
*;    Store most significant 16 bits of mantissa                              *
*;*****************************************************************************
*
        TEST    ACC
        B       NEGATIVE, LT    ; if OP is negative, return 0
        MOV     PH, AH          ; save AH
        LSR     AH, 7           ; remove high mantissa from AH
        MOV     OP_SE, AH       ; save sign and exponent
        MOV     AH, PH          ; restore original AH
        LSL     ACC, 9          ; zero sign & exp to get mantissa
        CLRC    SXM
        SFR     ACC, 1
        ADD     AH, #08000h     ; add implied 1 to mantissa
        MOV     RES_MAG, AH     ; save 16 most significant magnitude bits
*
*;*****************************************************************************
*;       EXPONENT EVALUATION                                                  *
*;  Test the exponent to determine into which of the three cases it belongs.  *
*;    Case 1:  exponents < 07Fh; 07Fh is the exponent for integer value 1.    *
*;             Result returned is 0 since the absolute value is less than 1.  *
*;    Case 2:  exponents > 08Eh; 08Eh is the exponent for integer values      *
*;             in the absolute range from 32768 to 65535.                     *
*;             Result returned is 7FFFh (the largest 16-bit value).           *
*;    Case 3:  exponents in the range of 07Fh to 08Eh inclusive will result   *
*;             in 16-bit unsigned integer values from 0 to 65535              *
*;*****************************************************************************
*	
        MOV     AH, OP_SE      ; load exponent
        SUB     AH, #07Fh      ; if exponent < 07Fh then underflow occurs
        B       UNDERFLOW, LT
        SUB     AH, #0Fh       ; if exponent > 08Eh then overflow occurs
        B       OVERFLOW, GT
*
*;*****************************************************************************
*;       NORMAL REPRESENTABLE 16-BIT RESULTS                                  *
*;  Load shift value needed to denormalize to T register                      *
*;  Shift via the T-register to denormalize (result in lower accumulator).    *
*;*****************************************************************************
*
        NEG     AH
        MOV     T, AH             ; place shift count into T register
        MOV     ACC, RES_MAG      ; load denormalized mantissa
        SFR     ACC, T            ; T is in [0..15]
*
*;*****************************************************************************
*;       RETURN                                                               *
*;*****************************************************************************
*
RETURN
ZERO
        RET
*
*;*****************************************************************************
*;       UNDERFLOW PROCESSING                                                 *
*;  Load accumulator with return value.                                       *
*;  This is not an error condition since results are always truncated.        *
*;*****************************************************************************
*
UNDERFLOW
NEGATIVE
         MOV    AL, #0
         RET
*
*;*****************************************************************************
*;       OVERFLOW PROCESSING                                                  *
*;  Load accumulator with return value.                                       *
*;*****************************************************************************
*
OVERFLOW
        MOV    AL, #0FFFFh
        RET
	.endasmfunc

