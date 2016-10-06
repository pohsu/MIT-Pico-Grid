        .width 96
;******************************************************************************
;*                                                                            *
;*  FS_MPY v15.12.3                                                           *
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
;*  FS$$MPY - multiply two floating point numbers                             *
;*                                                                            *
;******************************************************************************
*;*****************************************************************************
*;                                                                            *
*;  revision:  original                                                       *
*;                                                                            *
*;*****************************************************************************
*;                                                                            *
*;       FS$$MPY                                                HI MEMORY     *
*;                                                                            *
*;                                                               stack        *
*;                                                         +----------------+ *
*;       This routine multiplies two floating point   SP-->|                | *
*;       numbers.  Upon entry one operand (OP1) is         +----------------+ *
*;       in ACC and the other (OP2) is on the           -1 |  OP1 high mant | *
*;       stack as shown.  SP is moved to accomodate        +----------------+ *
*;       locals.  When multiplication is finished the   -2 |  OP1 low mant  | *
*;       SP is returned to point at the return address.    +----------------+ *
*;                                                   -3 -4 |     XAR3       | *
*;       inputs:  OP1 - in ACC, OP2 - on stack             +----------------+ *
*;                                                   -5 -6 |     XAR2       | *
*;       implementation:  OP1 and OP2 are each unpacked    +----------------+ *
*;            into sign, exponent, and two words of  -7 -8 |     XAR1       | *
*;            mantissa.  If either exponent is zero        +----------------+ *
*;            special case processing is initiated. -9 -10 |     XAR0       | *
*;            The exponents are summed.  If the            +----------------+ *
*;            result is less than zero underflow       -11 | (return addr)  | *
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
*;            multiply is executed.  The result of                            *
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
        .global   FS$$MPY
        .sect     ".text"

FS$$MPY:	.asmfunc
        .asg      AR7,    RES_EXP
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
        B       OP_ZERO, EQ       ; if OP1 is 0, jump to special case
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
        MOVL    ACC, OP2_LSW      ; load ACC with OP2
        B       OP_ZERO, EQ       ; if OP2 is 0, jump to special case
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
*;  Sum the exponents of OP1 and OP2 to determine the result exponent.  Since *
*;  the exponents are biased (excess 127) the summation must be decremented   *
*;  by the bias value to avoid double biasing the result.                     *
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
         SUBB   XOP1_SE, #07Fh    ; Subtract offset (avoid double bias)       
         MOV    AL, OP1_SE
         ADD    AL, OP2_SE        ; Add the exponents                  
         MOV    RES_EXP, AL       ; Save result exponent on stack
*
         B      UNDERFLOW, LT     ; branch to underflow handler if exp < 0
         SUB    AL, #0FFh         ; test for overflow
         B      OVERFLOW, GT      ; branch to overflow is exp > 127
*
*;*****************************************************************************
*;       MULTIPLICATION                                                       *
*;  Multiplication is implemented by parts.  Mantissa for OP1 is three bytes  *
*;  identified as Q, R, and S (Q represents OP1 high mantissa and R and S     *
*;  represent the two bytes of OP1 low mantissa).  Mantissa for OP2 is also 3 *
*;  bytes identified as X, Y, and Z (X represents OP2 high mant and Y and Z   *
*;  represent the two bytes of OP2 low mantissa).  Then                       *
*;                                                                            *
*;                      0  Q  R  S      (mantissa of OP1)                     *
*;                   x  0  X  Y  Z      (mantissa of OP2)                     *
*;                     ===========                                            *
*;                           RS*YZ      <-- save only upper 16 bits of result *
*;                        RS*0X                                               *
*;                        0Q*YZ                                               *
*;                     0Q*0X            <-- upper 16 bits are always zero     *
*;                     ===========                                            *
*;                        result        <-- result is always in the internal  *
*;                                          32 bits (which ends up in the     *
*;                                          accumulator) of the possible 64   *
*;                                          bit product                       *
*;                                                                            *
*;*****************************************************************************
*
        MOV     T, OP1_LM       ; load low mant of OP1 to T register
        MPYU    ACC, T, OP2_LM  ; ACC = RS * YZ
        MPYU    P, T, OP2_HM    ; P   = RS * 0X
        SFR     ACC, 16
        ADDL    ACC, P
        MOVL    P, ACC          ; P   = (RS * YZ) + (RS * 0X)
*
        MOV     T, OP1_HM       ; load high mant of OP1 to T register
        MPYU    ACC, T, OP2_LM  ; ACC = 0Q * YZ
        ADDL    ACC, P          ; ACC = (RS * YZ) + (RS * 0X) + (0Q * YZ)
        MPYU    P, T, OP2_HM    ; P   = 0Q * 0X
        
        ADD     ACC, PL << 16   ; ACC = final result
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
        ADDB    ACC, #040h       ; Add rounding bit
        B       NORMALIZED, GEQ  ; check if MSB is in 31
        SFR     ACC, 1           ; shift result so result is in bits 30:7
        ADD     RES_EXP, #1      ; increment exponent

NORMALIZED
        MOVL    P, ACC           ; save ACC
        MOV     AH, RES_EXP      ; load exponent of result
        B       UNDERFLOW, LEQ   ; check for underflow
        SUB     AH, #0FFh        ; adjust to check for overflow
        B       OVERFLOW, GEQ    ; check for overflow
        MOVL    ACC, P           ; restore ACC
        SFR     ACC, 7           ; shift to get 23 ms bits of mantissa result
        AND     AH, #07Fh        ; remove implied one
        MOVZ    RES_HM, AH
        MOVZ    RES_LM, AL       ; store the mantissa result
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
OP_ZERO
        SUBB    SP, #2
        POP     XAR3
	POP	XAR2
	POP	XAR1
	POP	XAR0
        LRETR

        .page
*;*****************************************************************************
*;       OVERFLOW PROCESSING                                                  *
*;  Load accumulator with return value.                                       *
*;*****************************************************************************
*
OVERFLOW
        MOV     AH, RES_SIGN
        LSL     AH, 7             ; Load sign of result
        ADD     AH, #07F7Fh       ; Result exponent  = 0FEh
                                  ; Result high mant = 07Fh
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
	.endasmfunc
