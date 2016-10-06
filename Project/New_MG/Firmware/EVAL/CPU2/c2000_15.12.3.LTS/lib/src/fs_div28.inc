        .width 96
;******************************************************************************
;*                                                                            *
;*  FS_DIV v15.12.3                                                           *
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
;*  FS$$DIV - divide two floating point numbers                               *
;*                                                                            *
;******************************************************************************
*;*****************************************************************************
*;                                                                            *
*;  revision:  original                                                       *
*;                                                                            *
*;*****************************************************************************
*;                                                                            *
*;       FS$$DIV                                                HI MEMORY     *
*;                                                                            *
*;                                                               stack        *
*;                                                         +----------------+ *
*;       This routine divides two floating point      SP-->|                | *
*;       numbers.  Upon entry one operand (OP1) is         +----------------+ *
*;       in ACC and the other (OP2) is on the           -1 |  OP1 high mant | *
*;       stack as shown.  SP is moved to accomodate        +----------------+ *
*;       locals.  When division is finished the SP is   -2 |  OP1 low mant  | *
*;       returned to point at the return address.          +----------------+ *
*;                                                   -3 -4 |     XAR3       | *
*;       inputs:  OP1 - in ACC, OP2 - on stack             +----------------+ *
*;                                                   -5 -6 |     XAR2       | *
*;       implementation:  OP1 and OP2 are each unpacked    +----------------+ *
*;            into sign, exponent, and two words of  -7 -8 |     XAR1       | *
*;            mantissa.  If either exponent is zero        +----------------+ *
*;            special case processing is initiated. -9 -10 |     XAR0       | *
*;            The difference of the exponents are taken.   +----------------+ *
*;            IF the result is less than zero underflow-11 | (return addr)  | *
*;            has occurred.  If the result is zero,        +----------------+ *
*;            underflow may have occurred.  If the     -12 | (return addr)  | *
*;            result is equal to 254 overflow may          +----------------+ *
*;            have occurred.  If the result is         -13 |   MSW of OP2   | *
*;            greater than 254 overflow has occurred.      +----------------+ *
*;            Underflow processing returns a value     -14 |   LSW of OP2   | *
*;            of zero.  Overflow processing returns        +----------------+ *
*;            the largest magnitude value along with                          *
*;            the appropriate sign.  If no special             LO MEMORY      *
*;            cases are detected, a 24x24-bit                                 *
*;            divide is executed.  The result of                              *
*;            the eXclusive OR of the sign bits, the         register file    *
*;            difference of the exponents and the 24       +----------------+ *
*;            bit truncated mantissa are packed and    AR0 | OP2 low mant   | *
*;            returned.                                    +----------------+ *
*;                                                     AR1 | OP2 high mant  | *
*;                                                         +----------------+ *
*;                                                     AR2 | res low mant   | *
*;                                                         +----------------+ *
*;                                                     AR3 | res high mant  | *
*;                                                         +----------------+ *
*;                                                     AR4 | OP1 sign & exp | *
*;                                                         +----------------+ *
*;                                                     AR5 | OP2 sign & exp | *
*;                                                         +----------------+ *
*;                                                     AR6 | res sign       | *
*;                                                         +----------------+ *
*;                                                     AR7 | res exponent   | *
*;                                                         +----------------+ *
*;                                                                            *
*;       result:  returned in ACC                                             *
*;                                                                            *
*;    NOTE: The ordering of the locals are placed to take advantage           *
*;          of long word loads and stores which require the hi and lo         *
*;          words to be at certain addresses. Any future modifications        *
*;          which involve the stack must take this quirk into account         *
*;                                                                            *
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
        .global   FS$$DIV
        .sect     ".text"

FS$$DIV:	.asmfunc
        .asg      AR7,    RES_EXP
        .asg      XAR7,   XRES_EXP
        .asg      AR3,    RES_HM
        .asg      AR2,    RES_LM
        .asg      AR6,    RES_SIGN
        .asg      AR4,    OP1_SE
	.asg      XAR4,   XOP1_SE
        .asg      AR5,    OP2_SE
        .asg      AR1,    OP2_HM
        .asg      AR0,    OP2_LM
        .asg    *-SP[1],  OP1_HM
        .asg    *-SP[2],  OP1_LM

        .asg    *-SP[13],  OP2_MSW
        .asg    *-SP[14], OP2_LSW

*
*;*****************************************************************************
*;       CONTEXT SAVE                                                         *
*;       Save contents of AR0 - AR3, and allocate stack space for locals      *
*;*****************************************************************************
*
        PUSH    XAR0
        PUSH    XAR1
        PUSH    XAR2
        PUSH    XAR3
        ADDB    SP, #2            ; Allocate space for locals
*
*;*****************************************************************************
*;       CONFIGURE STATUS BITS                                                *
*;*****************************************************************************
*
        CLRC    SXM

        .if .TMS320C2800_FPU32
        MOV32   ACC, R0H
        .endif
*
*;*****************************************************************************
*;       CONVERSION OF FLOATING POINT FORMAT - UNPACK                         *
*;  Test OP1 for special case treatment of zero.                              *
*;  Split the MSW of A in the accumulator.                                    *
*;    Save the sign and exponent on the stack [xxxx xxxS EEEE EEEE].          *
*;    Add the implied one to the mantissa value.                              *
*;    Store entire mantissa with a long word store                            *
*;*****************************************************************************
*
        TEST    ACC
        B       OP1_ZERO, EQ      ; if OP1 is 0, jump to special case
        MOVZ    OP1_SE, AH        ; store AH before sign & exp are removed
        AND     AH, #07Fh         ; mask off sign & exp to get high mantissa
        ADD     AH, #080h         ; add implied 1 to mantissa
        MOVL    OP1_LM, ACC       ; store mantissa           
        MOV     AH, OP1_SE        ; Restore high part containing sign & exp
        LSR     AH, 7             ; Remove high mantissa     
        MOVZ    OP1_SE, AH        ; store sign and exponent
*
*;*****************************************************************************
*;       CONVERSION OF FLOATING POINT FORMAT - UNPACK                         *
*;  Test OP2 for special case treatment of zero.                              *
*;  Split the MSW of A in the accumulator.                                    *
*;    Save the sign and exponent on the stack [xxxx xxxS EEEE EEEE].          *
*;    Add the implied one to the mantissa value.                              *
*;    Store entire mantissa                                                   *
*;*****************************************************************************
*
        .if .TMS320C2800_FPU32
        MOV32   ACC, R1H
        .else
        MOVL    ACC, OP2_LSW      ; load ACC with OP2
        .endif
        B       OP2_ZERO, EQ      ; if OP2 is 0, divide by zero
        MOVZ    OP2_SE, AH        ; store AH before sign & exp are removed
        AND     AH, #07Fh         ; mask off sign & exp to get high mantissa
        ADD     AH, #080h         ; add implied 1 to mantissa
        MOVZ    OP2_HM, AH
        MOVZ    OP2_LM, AL        ; store mantissa
        MOV     AH, OP2_SE        ; Restore high part containing sign & exp
        LSR     AH, 7             ; Remove high mantissa     
        MOVZ    OP2_SE, AH        ; store sign and exponent
*
*;*****************************************************************************
*;       SIGN EVALUATION                                                      *
*;  Exclusive OR sign bits of OP1 and OP2 to determine sign of result.        *
*;*****************************************************************************
*
        MOV     AH, OP1_SE        ; load sign and exp of op1 to AH
        XOR     AH, OP2_SE        ; xor with op2 to get sign of result
        AND     AH, #00100h       ; mask to get sign
        MOV     RES_SIGN, AH      ; save sign of result to stack
*
*;*****************************************************************************
*;       EXPONENT SUMMATION                                                   *
*;  Find difference between operand exponents to determine the result         *
*;  exponent.  Since the subtraction process removes the bias it must be      *
*;  re-added in.
*;                                                                            *
*;  Branch to one of three blocks of processing                               *
*;    Case 1:  exp OP1 + exp OP2 results in underflow (exp < 0)               *
*;    Case 2:  exp OP1 + exp OP2 results in overflow (exp >= 0FFh)            *
*;    Case 3:  exp OP1 + exp OP2 results are in range (exp >= 0 & exp < 0FFh) *
*;      NOTE:  Cases when result exp = 0 may result in underflow unless there *
*;             is a carry in the result that increments the exponent to 1.    *
*;             Cases when result exp = 0FEh may result in overflow if there   *
*;             is a carry in the result that increments the exponent to 0FFh. *
*;*****************************************************************************
*
         AND    OP1_SE, #0FFh     ; Mask OP1 exponent
         AND    OP2_SE, #0FFh     ; Mask OP2 exponent
         ADDB   XOP1_SE, #07Fh    ; Add offset (difference eliminates offset) 
         MOV    AL, OP1_SE
         SUB    AL, OP2_SE        ; Take difference between exponents
         MOV    RES_EXP, AL       ; Save result exponent on stack
*
         B      UNDERFLOW, LT     ; branch to underflow handler if exp < 0
         SUB    AL, #0FFh         ; test for overflow
         B      OVERFLOW, GT      ; branch to overflow is exp > 127
*
*;*****************************************************************************
*;       DIVISION                                                             *
*;  Division is implemented by parts.  The mantissas for both OP1 and OP2 are *
*;  left shifted in the 32 bit field to reduce the effect of secondary and    *
*;  tertiary contributions to the final result.  The left shifted results     *
*;  are identified as OP1'HI, OP1'LO, OP2'HI, and OP2'LO where OP1'HI and     *
*;  OP2'HI have the xx most significant bits of the mantissas and OP1'LO      *
*;  and OP2'LO contain the remaining bits of each mantissa.  Let QHI and      *
*;  QLO represent the two portions of the resultant mantissa.  Then           *
*;                                                                            *
*;                 OP1'HI + OP1'LO     OP1'HI + OP1'LO          1             *
*;   QHI + QLO  =  ---------------  =  ---------------  * ----------------- . *
*;                 OP2'HI + OP2'LO         OP2'HI        (1 + OP2'LO/OP2'HI)  *
*;                                                                            *
*;   Now let   X = OP2'LO/OP2'HI.                                             *
*;                                                                            *
*;   Then by Taylor's Series Expansion,                                       *
*;                                                                            *
*;               1                2    3                                      *
*;             -----  =  1 - X + X  - X  +  ...                               *
*;             (1+X)                                                          *
*;                                                                            *
*;   Since OP2'HI contains the first xx significant bits of the OP2 mantissa, *
*;                                                                            *
*;                               -yy                                          *
*;          X = OP2'LO/OP2'HI < 2   .                                         *
*;                                                                            *
*;   Therefore the X**2 term and all subsequent terms are less than the       *
*;   least significant bit of the 24-bit result and can be dropped.           *
*;   The result then becomes                                                  *
*;                                                                            *
*;                 OP1'HI + OP1'LO          1                                 *
*;   QHI + QLO  =  ---------------  * ----------------                        *
*;                     OP2'HI        (1 + OP2'LO/OP2'HI)                      *
*;                                                                            *
*;                 OP1'HI + OP1'LO          OP2'LO                            *
*;              =  ---------------  * ( 1 - ------ )                          *
*;                     OP2'HI               OP2'HI                            *
*;                                                                            *
*;                                       OP2'LO                               *
*;               = (Q'HI + Q'LO) * ( 1 - ------ )                             *
*;                                       OP2'HI                               *
*;                                                                            *
*;    where Q'HI and Q'LO represent the first approximation of the result.    *
*;    Also since Q'LO and OP2'LO/OP2'HI are less significant the 24th bit of  *
*;    the result, this product term can be dropped so that                    *
*;                                                                            *
*;     QHI + QLO  =  Q'HI + Q'LO - (Q'HI * OP2'LO)/OP2'HI .                   *
*;                                                                            *
*;*****************************************************************************
*
        MOV     AH, OP2_HM
        MOV     AL, OP2_LM        ; Load divisor mantissa
        LSL     ACC, 7            ; Shift divisor in preparation for division
        MOVZ    OP2_HM, AH
        MOVZ    OP2_LM, AL        ; Save off divisor

        MOVL    ACC, OP1_LM       ; Load dividend mantissa
        LSL     ACC, 6            ; Shift dividend in preparation for division

        RPT     #14               ; QHI = OP1'HI/OP2'HI
     || SUBCU   ACC, OP2_HM
        MOVZ    RES_HM, AL        ; Save QHI

        SUBU    ACC, RES_HM       ; Clear QHI from ACC
        RPT     #10               ; Q'LO = OP1'LO / OP2'HI
     || SUBCU   ACC, OP2_HM
        MOV     RES_LM, ACC << 5  ; Save Q'LO

        MOV     T, RES_HM         ; T = Q'HI
        MPYU    ACC, T, OP2_LM    ; Store Q'HI * OP2'LO in acc A
        SFR     ACC, 1

        RPT     #11               ; Calculate Q'HI * OP2'LO / OP2'HI
     || SUBCU   ACC, OP2_HM       ;   (correction factor)
        LSL     ACC, 4            ; Left shift to bring it to proper range
        MOV     AH, #0            ; Mask off correction factor
        LSL     ACC, 1

        NEG     ACC               ; Subtract correction factor
        ADDCU   ACC, RES_LM       ; Add Q'LO
        ADD     AH, RES_HM        ; Add Q'HI
*	
*;*****************************************************************************
*;       POST-NORMALIZATION ADJUSTMENT AND STORAGE                            *
*;  Set up to adjust the normalized result.                                   *
*;    The MSB may be in bit 31.  Test this case and increment the exponent    *
*;    and right shift mantissa 1 bit so result is in bits 30 through 7.       *
*;  Right shift mantissa by 7 bits.                                           *
*;  Store low mantissa on stack.                                              *
*;  Mask implied 1 and store high mantissa on stack.                          *
*;  Test result for underflow and overflow.                                   *
*;*****************************************************************************
*
        SFR     ACC, 1          ; MSB may be in bit 31; shift so we don't
                                ; lose any mantissa during normalization
        ADD     RES_EXP, #1     ; adjust exponent

	RPT	#31
     || NORM    ACC, XRES_EXP-- ; normalize the result, and adjust exponent

        ADDB    ACC, #020h      ; add rounding bit
        SFR     ACC, 1          ; addition may result in carry, so shift
                                ; before normalizing to make sure mantissa is
                                ; preserved
        ADD     RES_EXP, #1     ; adjust exponent

	RPT	#31
     || NORM    ACC, XRES_EXP-- ; normalize after rounding, and adjust exponent

        MOVL    P, ACC          ; save ACC
        MOV     AL, RES_EXP     ; test exponent
        B       UNDERFLOW, LEQ  ; process underflow if it occurs
        SUB     AL, #0FFh       ; adjust to check for overflow
        B       OVERFLOW, GEQ   ; process overflow if it occurs
        MOVL    ACC, P          ; restore ACC
        SFR     ACC, 7          ; shift right to place mantissa for splitting
        AND     AH, #07Fh       ; remove implied one
        MOVZ    RES_HM, AH
        MOVZ    RES_LM, AL      ; save result mantissa
*
*;*****************************************************************************
*;       CONVERSION OF FLOATING POINT FORMAT - PACK                           *
*;  Load sign.                                                                *
*;  Pack exponent.                                                            *
*;  Pack mantissa.                                                            *
*;*****************************************************************************
*
        MOV     ACC, RES_SIGN << 16  ; 0000 000S 0000 0000 0000 0000 0000 0000
        ADD     AH, RES_EXP          ; 0000 000S EEEE EEEE 0000 0000 0000 0000
        LSL     AH, 7                ; SEEE EEEE E000 0000 0000 0000 0000 0000
        ADDCU   ACC, RES_LM          ; SEEE EEEE E000 0000 MMMM MMMM MMMM MMMM
        ADD     AH, RES_HM           ; SEEE EEEE EMMM MMMM MMMM MMMM MMMM MMMM
*
*;*****************************************************************************
*;       CONTEXT RESTORE                                                      *
*;  Pop local floating point variables.                                       *
*;  Pop AR0 - AR3                                                             *
*;*****************************************************************************
*
* entry: A - final result
*
RETURN_VALUE
OP1_ZERO
        .if .TMS320C2800_FPU32
        MOV32    R0H, ACC
        .endif

        SUBB    SP, #2
        POP    XAR3
        POP    XAR2
        POP    XAR1
        POP    XAR0
        LRETR

        .page
*;*****************************************************************************
*;       OVERFLOW PROCESSING                                                  *
*;  Load accumulator with return value.                                       *
*;*****************************************************************************
*
OVERFLOW
        MOV     AH, #0            ; Expand value in AL to occupy all of ACC
        SAT     ACC               ; Result exponent  = 0FEh
        SUB     AH, #081h         ; Result high mant = 07Fh
        MOVL    P, ACC            ; Move result to P
        MOV     AH, RES_SIGN      ; Calculate sign of result
        B       RETURN_VALUE, NEQ
        MOV     AH, #8000h
        XOR     AH, PH
        MOV     AL, PL
        B       RETURN_VALUE,UNC
*
*;*****************************************************************************
*;       UNDERFLOW PROCESSING                                                 *
*;  Load accumulator with return value.                                       *
*;*****************************************************************************
*
UNDERFLOW
        MOV     ACC, #0 
        B       RETURN_VALUE,UNC
*
*;*****************************************************************************
*;       DIVIDE BY ZERO                                                       *
*;  Load accumulator with return value.                                       *
*;*****************************************************************************
*
OP2_ZERO
        MOVL    P, ACC          ; Save ACC
        MOV     AH, OP1_SE      ; Load sign and exponent of OP1
        AND     AH, #100h       ; Mask to get sign of OP1
        LSL     AH, 7
        MOVZ    OP1_SE, AH      ; Store sign
        MOVL    ACC, P          ; Restore ACC

        SUB     AH,  #081h      ; Result high mant = 7Fh
        XOR     AH,  #8000h
        OR      AH,  OP1_SE     ; Pack sign
        B       RETURN_VALUE,UNC
	.endasmfunc
