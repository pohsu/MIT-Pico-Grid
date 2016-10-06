        .width 96
;******************************************************************************
;*                                                                            *
;*  FS_CMP v15.12.3                                                           *
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
;*  FS$$CMP - compare two floating point numbers                              *
;*                                                                            *
;******************************************************************************
*;*****************************************************************************
*;                                                                            *
*;  revision:  original                                                       *
*;                                                                            *
*;*****************************************************************************
*;                                                                            *
*;       FS$$CMP                                                HI MEMORY     *
*;                                                                            *
*;       This routine compares two floating point                stack        *
*;       numbers.  Upon entry, one number is on ACC        +----------------+ *
*;       (OP1) and the other is in the stack as       SP-->|                | *
*;       shown (OP2).  The routine returns a               +----------------+ *
*;       positive 1 if OP1 is greater than OP2, a       -1 | (return addr)  | *
*;       negative 1 if OP1 is less than OP2, and           +----------------+ *
*;       returns 0 if OP1 is equal to OP2.              -2 | (return addr)  | *
*;       It is up to the calling program to interpret      +----------------+ *
*;       the results.                                   -3 |   MSW of OP2   | *
*;                                                         +----------------+ *
*;       inputs:  OP1 and OP2 (floating point numbers)  -4 |   LSW of OP2   | *
*;                                                         +----------------+ *
*;       implementation:  Uses the MAX and MIN                                *
*;            instructions to determine which operand          LO MEMORY      *
*;            is larger.                                                      *
*;                                                                            *
*;       result:  returned in ACC                                             *
*;                OP1 >  OP2 :  return 1                                      *
*;                OP1 <  OP2 :  return -1                                     *
*;                OP1 == OP2 :  return 0                                      *
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
        .global   FS$$CMP
        .sect     ".text"

FS$$CMP:	.asmfunc
        .asg    *-SP[3], OP2_MSW
        .asg    *-SP[4], OP2_LSW

*;*****************************************************************************
*;       COMPARISON OF OP1 and OP2                                            *
*;  There are three cases (directly reflecting the value in the accumulator). *
*;    Case 1:  OP1 is greater than OP2.                                       *
*;    Case 2:  OP1 is equal to OP2                                            *
*;    Case 3:  OP1 is less than OP2                                           *
*;*****************************************************************************
*
        CMPL   ACC, OP2_LSW         ; compare the two operands
        B      OPS_EQ, EQ           ; branch if OP1 == OP2
        B      OP1_GT_OP2, GT       ; branch if OP1 > OP2
        MOV    AR4, #-1             ; set AR4 to -1 if OP1 < OP2
        B      CHECK_SIGNS
OP1_GT_OP2
        MOV    AR4, #1              ; set AR4 to 1 if OP1 > OP2
        B      CHECK_SIGNS
OPS_EQ
        MOV    AR4, #0              ; set AR4 to 0 if OP1 == OP2
CHECK_SIGNS
        MOV    AR5, #2              ; initialize AR5 to 2                      
        TBIT   AH, #15              ; test to see if OP1 is negative        
        B      OP1_NONNEG, NTC      ; branch if OP1 is non-negative          
        DEC    AR5                  ; decrememt AR5 if OP1 is negative       
OP1_NONNEG
        TBIT   OP2_MSW, #15         ; test to see if OP2 is negative
        B      OP2_NONNEG, NTC      ; branch if OP2 is non-negative
        DEC    AR5                  ; decrement AR5 if OP2 is negative
OP2_NONNEG
        SETC   SXM                  ; sign extend the next move
        MOV    ACC, AR4             ; move the result from AR4 to ACC
        CMP    AR5, #0              ; test to see if both operands are
                                    ; negative (AR5 == 0)
        B      DONE, GT             ; branch if either operand is non-negative
        NEG    ACC                  ; if OP1 and OP2 are both negative,
                                    ; then reverse the result
DONE
        RET
	.endasmfunc
