         .width 96
;******************************************************************************
;*                                                                            *
;*  FS_TOLL v15.12.3 
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
;*  FS$$TOLL  - Convert an IEEE 754 format single precision floating point    *
;*              number into an unsigned 64 bit long long integer	      *
;*                                                                            *
;******************************************************************************
;******************************************************************************
;*                                                                            *
;*  revision:  original                                                       *
;*                                                                            *
;******************************************************************************
*;                                                                            *
*;       FS$$TOLL                                            register file   *
*;                                                         +----------------+ *
*;       This routine converts a floating point        AR4 | OP sign & exp  | *
*;       value to a 64-bit signed integer.  Upon           +----------------+ *
*;       entry the floating point number is in                                *
*;       ACC. When the conversion is complete, the integer                    *
*;       value will be in ACC:P                                               *
*;                                                                            *
*;       inputs:  A floating point value in ACC                               *
*;                                                                            *
*;       implementation:  A is unpacked into sign,                            *
*;            exponent, and mantissa.                                         *
*;            If the exponent exceeds a value of 0BEh                         *
*;            then an overflow has occurred and a                             *
*;            saturated value will be returned.  For                          *
*;            all exponents less than 07Fh the value                          *
*;            of zero is returned.  Within the exponent                       *
*;            range of 07Fh through 0BEh, the                                 *
*;            mantissa is normalized and returned.                            *
*;                                                                            *
*;       result:  returned in ACC:P                                           *
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
         .global   FS$$TOLL

FS$$TOLL	.asmfunc
	 .asg	AR4, OP_SE

*
*;*****************************************************************************
*;       CONVERSION OF FLOATING POINT FORMAT - UNPACK                         *
*;    Save the sign and exponent on the stack [xxxx xxxS EEEE EEEE].          *
*;    Add the implied one to the mantissa value.                              *
*;*****************************************************************************
*
        TEST    ACC
        B       ZERO, EQ        ; if OP is zero, return
        MOVZ    OP_SE, AH       ; save original high mantissa
        AND     AH, #07Fh       ; zero sign & exp to get mantissa
        ADD     AH, #080h       ; add implied 1 to mantissa
        LSL     ACC, 7          ; shift mantissa to ready for denormalization
        MOV     PH, AH          ; save current high result
        MOV     AH, OP_SE       ; restore original high mantissa
        LSR     AH, 7
*
*;*****************************************************************************
*;       EXPONENT EVALUATION                                                  *
*;  Test the exponent to determine into which of the three cases it belongs.  *
*;    Case 1:  exponents < 07Fh; 07Fh is the exponent for integer value 1.    *
*;             Result returned is 0 since the absolute value is less than 1.  *
*;    Case 2:  exponents > 09Eh; 09Dh is the exponent for integer values      *
*;             in the absolute range from 1073741824 to 2147483648.           *
*;             Result returned is 7FFFFFFFh or 80000000h                      *
*;    Case 3:  exponents in the range of 07Fh to 09Dh inclusive will result   *
*;             in 16-bit signed integer values from -2MB to +2MB              *
*;*****************************************************************************
*
        AND     AH, #0FFh
        SUB     AH, #07Fh      ; if exponent < 07Fh then underflow occurs
        B       UNDERFLOW, LT
        SUB     AH, #3Eh       ; if exponent > 0BDh then overflow occurs
        B       OVERFLOW, GT
        B       TEST_SIGN1, EQ ; test if denormalizing shift count is zero
*
*;*****************************************************************************
*;       NORMAL REPRESENTABLE 64-BIT RESULTS                                  *
*;  Load shift value needed to denormalize to T register                      *
*;  Shift via the T-register to denormalize                                   *
*;*****************************************************************************
*
	NEG	AH		  ;
	MOV	T, AH		  ;
	MOV	AH,PH		  ;
	MOV	P,#0		  ;
	LSR64   ACC:P, T	  ;
	B	TEST_SIGN, UNC    ;

*
*;*****************************************************************************
*; Convert number to signed value                                             *
*;*****************************************************************************
*
TEST_SIGN1
	MOV	AH,PH
	MOV	P, #0
TEST_SIGN
        TBIT    OP_SE, #15      ; test sign bit
        B       POSITIVE, NTC   ; leave number alone if positive
        NEG64   ACC:P           ; convert to two's compliment value
POSITIVE
        LRETR
*
*;*****************************************************************************
*;       OVERFLOW PROCESSING                                                  *
*;  Set ACC to 0x80000000:00000000 if sign is negative; otherwise             *
*;          to 0x7fffffff:ffffffff                                            *
*;*****************************************************************************
*
OVERFLOW:
	MOV	OVC, OP_SE	; Load OVC with bits 15-10 of OP_SE, which
				; is sign bit and 5 MSB of exponent 
	SAT64	ACC:P		; If OVC > 0 ACC:P = 0x7fffffff:ffffffff
				; else if OVC < 0, ACC:P = 0x80000000:00000000
				; When OVC = 0, ACC is not modified. For this 
				; case to happen, 5 MSBs of exp should be zero
				; which means the exponent is less than 0x3FF 
				; and should have been underflowed.           
        LRETR
*
*;*****************************************************************************
*;       UNDERFLOW PROCESSING                                                 *
*;  Load accumulator with return value.                                       *
*;  This is not an error condition since results are always truncated.        *
*;*****************************************************************************
*
ZERO
UNDERFLOW
         ZAPA
         LRETR			 ; return
	.endasmfunc

