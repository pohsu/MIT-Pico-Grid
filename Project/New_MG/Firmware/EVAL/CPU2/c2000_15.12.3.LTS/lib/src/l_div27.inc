;*****************************************************************************
;* l_div27.inc  v15.12.3
;* 
;* Copyright (c) 1997-2016 Texas Instruments Incorporated
;* http://www.ti.com/ 
;* 
;*  Redistribution and  use in source  and binary forms, with  or without
;*  modification,  are permitted provided  that the  following conditions
;*  are met:
;* 
;*     Redistributions  of source  code must  retain the  above copyright
;*     notice, this list of conditions and the following disclaimer.
;* 
;*     Redistributions in binary form  must reproduce the above copyright
;*     notice, this  list of conditions  and the following  disclaimer in
;*     the  documentation  and/or   other  materials  provided  with  the
;*     distribution.
;* 
;*     Neither the  name of Texas Instruments Incorporated  nor the names
;*     of its  contributors may  be used to  endorse or  promote products
;*     derived  from   this  software  without   specific  prior  written
;*     permission.
;* 
;*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS
;*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT
;*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
;*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
;*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT
;*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
;*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
;*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT
;*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;* 
;*****************************************************************************


******************************************************************************
* This module contains the functions for 32-bit divide and modulus, signed and
* unsigned.  This stack map and these defines apply to all the routines in 
* this module.               
*
* SYMBOL DEFINITIONS:
*
* QUOT  - Dividend on input, quotient on output
* DEN   - denominator 
* REM   - remainder
* xHI   - High word of that object
* xLO   - Low word of that object
* COUNT - Iteration count of divide routine
* SIGN  - Sign of result
*
******************************************************************************

;	STACK MAP
;
;	+-----------+	
;	|           |    <-- SP (during MAINDIV routine)
;	+-----------+
;	| return    |    
;	+-----------+
;	| address   |  0 <-- SP (after frame allocation)
;	+-----------+	
;	|  REM_HI   | -1
;	+-----------+
;	|  REM_LO   | -2
;	+-----------+
;	|  NUM_HI   | -3
;	+-----------+
;	|  NUM_LO   | -4 <-- SP (on entry)
;	+-----------+	
;	| no return address
;	+-----------+
;	| called with FFC, return in XAR7
;	+-----------+
;	|  DEN_HI   | -5  (-7 from MAINDIV)
;	+-----------+
;	|  DEN_LO   | -6  (-8 from MAINDIV)
;	+-----------+
;	|           |
;
; Numerator passed in ACC, denominator is on stack


FSIZE	.set	4		; frame size

******************************************************************************
* SET UP ALIASES FOR STACK REFERENCES AND REGISTERS USED
******************************************************************************
	.asg	AR5,COUNT	; iteration counter
	.asg	AR6,SIGN	; sign of result
	.asg	*-SP[5],DEN_HI	; denominator high
	.asg	*-SP[6],DEN	; denominator 
	.asg	*-SP[2],REM	; remainder
	.asg	*-SP[4],QUOT	; quotient	
	.asg 	*-SP[4],NUM	; numerator (store in same place as quotient)

	.asg	*-SP[6 + 2],MD_DEN    	; MAINDIV access of denominator
	.asg	*-SP[4 + 2],MD_QUOT 	; MAINDIV access of quotient
	.asg    *-SP[2 + 2],MD_REM	; MAINDIV access of remainder

	.page
******************************************************************************
* 32-bit SIGNED DIVIDE, CALCULATE NUM / DEN AND RETURN IN ACCUMULATOR 
******************************************************************************
	.global	L$$DIV

L$$DIV:		.asmfunc
******************************************************************************
* ALLOCATE FRAME, THE COMPILER WILL DO ANY NECESSARY SAVING OF REGISTERS
* USED AT THE CALL SITE
******************************************************************************
	MPYB	P,T,#0
	PUSHL 	PL:PH
	PUSHL	PL:PH

******************************************************************************
* DETERMINE SIGN OF RESULT
******************************************************************************
	CLRC	OVM		; Clear OVM to get desired effect on ABS & NEG
	MOVL	NUM,ACC		; save off numerator
	XOR	AH,DEN_HI	; determine sign of result
	MOV	SIGN,AH		; save off sign of result

******************************************************************************
* TAKE ABSOLUTE VALUE OF OPERANDS
******************************************************************************
	MOVL	ACC,DEN		; load denominator
	ABS	ACC		; take absolute value of denominator
	MOVL	DEN,ACC		; save off |denominator|

	MOVL	ACC,NUM		; load numerator
	ABS	ACC		; take absolute value of numerator
	
******************************************************************************
* LOOK FOR EASY OUTS
******************************************************************************
	CMPL	ACC,DEN		; compare denominator to numerator
	B	EQ1,EQ		; branch if equal (return 1)
	B	EQ0,LO		; branch if denominator > numerator (return 0)

******************************************************************************
* CALL MAIN DIVIDE ROUTINE
******************************************************************************
	CALL	MAINDIV		; call main divide routine
	MOV	ACC,P	        ; load result into ACC


******************************************************************************
* QUOTIENT IS IN ACC
******************************************************************************
CHECK_SIGN1:
	TBIT	SIGN,#15	; check MSB(sign)
	B	RET1,NTC	; return quotient as is if positive, otherwise
	NEG	ACC		; negate quotient
RET1:
	SUBB	SP,#FSIZE	; deallocate frame
	FFCRET	*XAR7		; return

******************************************************************************
* DENOMINATOR == NUMERATOR RETURN 1 (OR -1)
******************************************************************************
EQ1:
	MOVB	ACC,#1		; tentatively set quotient to 1
	B	CHECK_SIGN1	; check sign and return

******************************************************************************
* IF DENOMINATOR > NUMERATOR, RETURN 0
******************************************************************************
EQ0:
	MOVB 	ACC,#0		; set quotient to zero
	B	RET1		; return
	.endasmfunc



	.page
******************************************************************************
* 32-bit SIGNED MODULUS, CALCULATE NUM % DEN AND RETURN IN ACCUMULATOR
******************************************************************************
	.global	L$$MOD

L$$MOD:		.asmfunc
******************************************************************************
* ALLOCATE FRAME
******************************************************************************
	MPYB	P,T,#0
	PUSHL 	PL:PH
	PUSHL	PL:PH

******************************************************************************
* DETERMINE SIGN OF RESULT
******************************************************************************
	CLRC	OVM		; Clear OVM to get desired effect on ABS & NEG
	MOV	SIGN,AH		; sign of result is sign of numerator

******************************************************************************
* TAKE ABSOLUTE VALUE OF OPERANDS
******************************************************************************
	ABS	ACC		; take absolute value of numerator
	MOVL	NUM,ACC		; save off |numerator|

	MOVL	ACC,DEN		; load denominator
	ABS	ACC		; take absolute value of denominator
	MOVL	DEN,ACC		; save off |denominator|

	MOVL	ACC,NUM		; reload numerator

******************************************************************************
* LOOK FOR EASY OUTS
******************************************************************************
	CMPL	ACC,DEN		; compare denominator to numerator
	B	EQ2,EQ		; branch if equal (return 0)
	B	CHECK_SIGN2,LO  ; if (denominator > numerator) return numerator

******************************************************************************
* CALL MAIN DIVIDE ROUTINE, MAINDIV RETURNS QUOTIENT IN ACC
******************************************************************************
	CALL	MAINDIV		; call main divide routine
	MOVL	ACC,REM		; reload remainder

******************************************************************************
* NEGATE SIGN OF REMAINDER IF NECESSARY
******************************************************************************
CHECK_SIGN2:
	TBIT	SIGN,#15	; check MSB(sign)
	B	RET2, NTC	; if(positive) branch to return
	NEG	ACC		; otherwise, negate remainder

RET2:
	SUBB	SP,#FSIZE	; deallocate frame
	FFCRET	*XAR7		; return

******************************************************************************
* NUM AND DEN ARE EQUAL, RETURN 0
******************************************************************************
EQ2:
	MOVB	ACC,#0		; load return register with 0
	SUBB	SP,#FSIZE	; deallocate frame
	FFCRET	*XAR7		; return
	.endasmfunc



	.page
******************************************************************************
* 32-bit UNSIGNED DIVIDE, CALCULATE NUM / DEN AND RETURN IN ACCUMULATOR
******************************************************************************
	.global	UL$$DIV

UL$$DIV:	.asmfunc
******************************************************************************
* ALLOCATE FRAME
******************************************************************************
	MPYB	P,T,#0
	PUSHL 	PL:PH
	PUSHL	PL:PH

******************************************************************************
* LOOK FOR EASY OUTS
******************************************************************************
	CMPL	ACC,DEN		; compare denominator to numerator
	B	EQ3,EQ		; branch if equal (return 1)
	B	DGT3,LO  	; branch if denominator > numerator (return 0) 
        CMP     DEN_HI,#0       ; check if upper word of denominator is zero.
        SB      DIVUDenHighZero,EQ

******************************************************************************
* CALL MAIN DIVIDE ROUTINE
******************************************************************************
	CALL	MAINDIV		; call main divide routine
	MOV	ACC,P	        ; load result into ACC

******************************************************************************
* QUOTIENT IS IN ACC
******************************************************************************
RET3:
	SUBB	SP,#FSIZE	; deallocate frame
	FFCRET	*XAR7		; return

******************************************************************************
* NUM AND DWN ARE EQUAL, RETURN 1
******************************************************************************
EQ3:
	MOVB	ACC,#1		; quotient = 1
	B	RET3		; return

******************************************************************************
* IF DEN > NUM, RETURN 0
******************************************************************************
DGT3:
	MOVB 	ACC,#0		; set quotient to zero
	B	RET3		; return

******************************************************************************
* FOLLOWING PIECE OF CODE IS FAST OUT IF UPPER WORD OF DENOMINATOR IS ZERO
******************************************************************************
DIVUDenHighZero:
        MOV     P,ACC
        MOVU    ACC,AH
        RPT     #15
      ||SUBCU   ACC,DEN
        MOV     PH,AL
        MOV     AL,PL
        RPT     #15
      ||SUBCU   ACC,DEN
        MOV     AH,PH
        B       RET3
	.endasmfunc



	.page
******************************************************************************
* 32-bit UNSIGNED MODULUS, CALCULATE NUM % DEN AND RETURN IN ACCUMULATOR
******************************************************************************
	.global	UL$$MOD

UL$$MOD:	.asmfunc
******************************************************************************
* ALLOCATE FRAME
******************************************************************************
	MPYB	P,T,#0
	PUSHL 	PL:PH
	PUSHL	PL:PH

******************************************************************************
* LOOK FOR EASY OUTS
******************************************************************************
	CMPL	ACC,DEN		; compare denominator to numerator
	B	EQ4,EQ		; branch if equal (return 0)
	B	RET4,LO         ; if (denominator > numerator) return numerator
        CMP     DEN_HI,#0       ; check if upper word of denominator is zero
        SB      MODUDenHighZero,EQ

******************************************************************************
* CALL MAIN DIVIDE ROUTINE, MOVE REMAINDER INTO ACC
******************************************************************************
	CALL	MAINDIV		; call main divide routine
	MOVL	ACC,REM		; load remainder into return ACC

******************************************************************************
* RETURN SEQUENCE
******************************************************************************
RET4:
	SUBB	SP,#FSIZE	; deallocate frame
	FFCRET	*XAR7		; return

******************************************************************************
* NUM AND DEN ARE EQUAL, RETURN 0
******************************************************************************
EQ4:
	MOVB	ACC,#0		; load return ACC with 0
	B	RET4		; return

******************************************************************************
* FOLLOWING PIECE OF CODE IS FAST OUT IF UPPER WORD OF DENOMINATOR IS ZERO.
******************************************************************************
MODUDenHighZero:
        MOV     P,ACC
        MOVU    ACC,AH
        RPT     #15
      ||SUBCU   ACC,DEN
        MOV     AL,PL
        RPT     #15
      ||SUBCU   ACC,DEN
	MOV	AL,AH
        MOV     AH,#0
        B       RET4
	.endasmfunc


	.page
******************************************************************************
* MAIN DIVIDE ROUTINE
* ASSUMES NUM AND DEN ARE UNSIGNED AND NUM != 0.  IF DEN == 0, THEN THIS 
* ROUTINE WILL TAKE A LONG TIME ...
******************************************************************************
*
* This routine is based on the following psuedo-code
*
*    count = 31;
*
*    while (q <<= 1 shifts 0 into carry)
*      count--;
* 
*    /* just shifted 1 into carry */
*    r = 1; *
*    for(;;)
*    {
*      if (r >= d)
*      {
*	 r -= d;
*	 q |= 1;
*      }
*
*      if (count-- == 0) return;
*
*      r <<= 1;
*      if (MSB(q) == 1) r |= 1;
*      q <<= 1;
*    }


******************************************************************************
* QUOT IS IN ACCUMULATOR. P REGISTER AND REMAINDER HAVE BEEN INITIALIZED TO ZERO
******************************************************************************
MAINDIV:	.asmfunc
        MOVB    AR5,#31         ; Initialize loop counter
        TEST    ACC             ; Test ACC value
        SB      LOOP,LT         ; If negative, then skip
 
        RPT     #29             ; Normalize Numerator
     || NORM    ACC,AR5--
 
        LSL     ACC,1           ; If normalized, bit-31= 0
	SUBB	AR5,#1
LOOP:
        LSL     ACC,1           ; Shift bit into Carry (C)
        MOVL    MD_QUOT,ACC     ; Store shifted intermediate quotient
        MOVL    ACC,MD_REM      ; Load Remainder
        ROL     ACC             ; Shift Carry into bit-0
        MOVL    MD_REM,ACC      ; Store Remainder
        SUBL    ACC,MD_DEN      ; Subtract denominator
        SB      SKIP,LO	        ; Skip if lower (Carry=0)
        MOVL    MD_REM,ACC      ; else, store new Remainder
SKIP:
        MOV     ACC,P           ; Load Quotient (Carry preserved)
        ROL     ACC             ; Shift carry into bit-0
        MOV     P,ACC           ; Store Quotient
        MOVL    ACC,MD_QUOT     ; Reload intermediate quotient
        BANZ    LOOP,AR5--      ; Loop until count = 0
 
        RET                    ; Return to setup routine
	.endasmfunc 
