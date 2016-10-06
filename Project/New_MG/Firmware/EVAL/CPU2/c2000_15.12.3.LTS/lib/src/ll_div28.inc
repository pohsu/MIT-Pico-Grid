;*****************************************************************************
;* ll_div28.inc  v15.12.3
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
* This module contains the functions for 64-bit divide and modulus, signed and
* unsigned.  This stack map and these defines apply to all the routines in 
* this module.               
*
* SYMBOL DEFINITIONS:
*
* QUOT  - Intermediate Quotient
* Q     - Quotient
* DEN   - denominator 
* REM   - remainder
* xHI   - High word of that object
* xLO   - Low word of that object
* CNT   - Iteration count of divide routine
* SIGN  - Sign of result
*
******************************************************************************

;	STACK MAP
;
;	+-----------+	
;	|           |    <-- SP (during MAINDIV routine)
;	+-----------+
;	| ret addr  |  0 <-- SP (after frame allocation)
;	+-----------+	
;	|   Q_HI    | -2
;	+-----------+	
;	|   Q_LO    | -4
;	+-----------+	
;	|  REM_HI   | -6
;	+-----------+
;	|  REM_LO   | -8
;	+-----------+
;	| LDEN_HI   | -10
;	+-----------+
;	| LDEN_LO   | -12 <-- SP (on entry)
;	+-----------+	
;	| ret addr  |
;	+-----------+
;	|  DEN_HI   | -16  
;	+-----------+
;	|  DEN_LO   | -18  
;	+-----------+
;	|           |
;
; Numerator passed in ACC:P, denominator is on stack


FSIZE	.set	12		; frame size

******************************************************************************
* SET UP ALIASES FOR STACK REFERENCES AND REGISTERS USED
******************************************************************************
        .asg   *-SP[FSIZE+3],  DEN_MSB
        .asg   *-SP[FSIZE+4],  DEN_HI 
        .asg   *-SP[FSIZE+6],  DEN_LO  


	.asg	AR0,CNT		; iteration counter
	.asg	XAR0,XCNT	; iteration counter
	.asg	AR7,SIGN	; sign of result

	.asg	*-SP[2],   	Q_HI
	.asg	*-SP[4],   	Q_LO
	.asg	*-SP[5],   	REM_MSB
	.asg	*-SP[6],   	REM_HI
	.asg	*-SP[8],   	REM_LO
	.asg	*-SP[10],  	LDEN_HI
	.asg	*-SP[12],  	LDEN_LO

        .asg    XAR4,      	RES_HI  
        .asg    XAR5,      	RES_LO   

	.asg	*-SP[2 +2],   	MD_Q_HI
	.asg	*-SP[4 +2],   	MD_Q_LO
	.asg	*-SP[5 +2],   	MD_REM_MSB
	.asg	*-SP[6 +2],   	MD_REM_HI
	.asg	*-SP[8 +2],   	MD_REM_LO
	.asg	*-SP[10+2],   	MD_LDEN_HI
	.asg	*-SP[12+2],   	MD_LDEN_LO

        .asg    XAR4,   	QUOT_HI  
        .asg    XAR5,   	QUOT_LO   

        .asg    XAR4,         	LNUM_HI  
        .asg    XAR5,         	LNUM_LO   

        .asg    AR5,            TMP16  
        .asg    XAR5,           TMP32  

	.page
******************************************************************************
* 32-bit SIGNED DIVIDE, CALCULATE NUM / DEN AND RETURN IN ACCUMULATOR 
******************************************************************************
	.global	LL$$DIV
	.include "fd_util28.inc"

LL$$DIV:	.asmfunc stack_usage(14)
******************************************************************************
* ALLOCATE FRAME, THE COMPILER WILL DO ANY NECESSARY SAVING OF REGISTERS
* USED AT THE CALL SITE
******************************************************************************
	MOVB	TMP32,#0
	PUSH	TMP32
	PUSH	TMP32
	PUSH	TMP32
	PUSH	TMP32
	PUSH	TMP32
	PUSH	TMP32

******************************************************************************
* DETERMINE SIGN OF RESULT
******************************************************************************
	MOVZ	SIGN, DEN_MSB   ;
	XOR	SIGN, AH  	; determine sign of result
	CLRC	OVM		; Clear OVM to get desired effect on NEG64.

******************************************************************************
* TAKE ABSOLUTE VALUE OF OPERANDS
******************************************************************************
	CMP64   ACC:P		; Clear V flag
	CMP64   ACC:P		; Test contents of ACC:P
	B	$10, GEQ	;
	NEG64	ACC:P		; take absolute value of numerator
$10:
	MOVL    LNUM_HI,ACC     ; save off |numerator|
	MOVL    LNUM_LO,P       ; 
	
	MOVL	P,DEN_LO	; load denominator
	MOVL	ACC,DEN_HI	; load denominator
	B	$5, GEQ	;
	NEG64	ACC:P		;
$5:
	MOVL    LDEN_HI,ACC     ; save off |denominator|
	MOVL    LDEN_LO,P       ; 

******************************************************************************
* LOOK FOR EASY OUTS
******************************************************************************
	CMPL	ACC,LNUM_HI	;
	B	$15,NEQ		;
	MOVL	ACC,P		;
	CMPL	ACC,LNUM_LO	; compare denominator to numerator
$15:
	B	EQ1,EQ		; branch if equal (return 1)
	B	EQ0,HI		; branch if denominator > numerator (return 0)
	MOVL	ACC,LDEN_HI	;
        SB      SDIVUDenHighZero,EQ

******************************************************************************
* CALL MAIN DIVIDE ROUTINE
******************************************************************************
	MOVL	P,LNUM_LO	;
	MOVL	ACC,LNUM_HI	;
	LCR	MAINDIV		; call main divide routine
	MOVL	ACC, Q_HI	;
	MOVL	P, Q_LO		; load the result into ACC:P


******************************************************************************
* QUOTIENT IS IN ACC
******************************************************************************
CHECK_SIGN1:
	TBIT	SIGN,#15	; check MSB(sign)
	B	RET1,NTC	; return quotient as is if positive, otherwise
	NEG64	ACC:P		; negate quotient
RET1:
	SUBB	SP,#FSIZE	; deallocate frame
	LRETR			; return

******************************************************************************
* DENOMINATOR == NUMERATOR RETURN 1 (OR -1)
******************************************************************************
EQ1:
	MOV	PL,#1		;
	MOV	PH,#0		;
	MOVB	ACC,#0		; tentatively set quotient to 1
	B	CHECK_SIGN1,UNC	; check sign and return

******************************************************************************
* IF DENOMINATOR > NUMERATOR, RETURN 0
******************************************************************************
EQ0:
	ZAPA        		; set quotient to zero
	B	RET1,UNC	; return


******************************************************************************
* FOLLOWING PIECE OF CODE IS FAST OUT IF UPPER WORD OF DENOMINATOR IS ZERO
******************************************************************************
SDIVUDenHighZero:
	MOVB	ACC,#0
	MOVL	P, LNUM_HI
	RPT	#31
      ||SUBCUL  ACC, LDEN_LO
        MOVL    RES_HI, P
        MOVL    P, LNUM_LO
	RPT	#31
      ||SUBCUL  ACC, LDEN_LO
	MOVL	ACC,RES_HI 
        B       CHECK_SIGN1, UNC
	.endasmfunc


	.page
******************************************************************************
* 32-bit SIGNED MODULUS, CALCULATE NUM % DEN AND RETURN IN ACCUMULATOR
******************************************************************************
	.global	LL$$MOD

LL$$MOD:	.asmfunc stack_usage(14)
******************************************************************************
* ALLOCATE FRAME
******************************************************************************
	MOVB	TMP32,#0
	PUSH	TMP32
	PUSH	TMP32
	PUSH	TMP32
	PUSH	TMP32
	PUSH	TMP32
	PUSH	TMP32

******************************************************************************
* DETERMINE SIGN OF RESULT
******************************************************************************
	MOVZ	SIGN,AH         ; sign of result is sign of numerator
	CLRC	OVM		; Clear OVM to get desired effect on NEG64.

******************************************************************************
* TAKE ABSOLUTE VALUE OF OPERANDS
******************************************************************************
	CMP64   ACC:P		; Clear V flag
	CMP64   ACC:P		; Test contents of ACC:P
	B	$25, GEQ	;
	NEG64	ACC:P		; take absolute value of numerator
$25:
	MOVL    LNUM_HI,ACC     ; save off |numerator|
	MOVL    LNUM_LO,P       ; 
	
	MOVL	P,DEN_LO	; load denominator
	MOVL	ACC,DEN_HI	; load denominator
	B	$20, GEQ	;
	NEG64	ACC:P		;
$20:
	MOVL    LDEN_HI,ACC     ; save off |denominator|
	MOVL    LDEN_LO,P       ; 

******************************************************************************
* LOOK FOR EASY OUTS
******************************************************************************
	CMPL	ACC,LNUM_HI	;
	B	$30,NEQ		;
	MOVL	ACC,P		;
	CMPL	ACC,LNUM_LO	; compare denominator to numerator
$30
	B	EQ2,EQ		; branch if equal (return 0)
	B	RET_NUM1,HI     ; if (denominator > numerator) return numerator
        MOVL	ACC, LDEN_HI     ; check if upper word of denominator is zero
        SB      SMODUDenHighZero,EQ

******************************************************************************
* CALL MAIN DIVIDE ROUTINE, MAINDIV RETURNS QUOTIENT IN ACC
******************************************************************************
	MOVL	P,LNUM_LO	;
	MOVL	ACC,LNUM_HI	;
	LCR	MAINDIV		; call main divide routine
	MOVL	ACC,REM_HI	; reload remainder
	MOVL	P,REM_LO	; reload remainder

******************************************************************************
* NEGATE SIGN OF REMAINDER IF NECESSARY
******************************************************************************
CHECK_SIGN2:
	TBIT	SIGN,#15	; check MSB(sign)
	B	RET2, NTC	; if(positive) branch to return
	NEG64	ACC:P		; otherwise, negate remainder

RET2:
	SUBB	SP,#FSIZE	; deallocate frame
	LRETR			; return

RET_NUM1:
	MOVL	ACC, LNUM_HI	;
	MOVL	P, LNUM_LO	;
	B	CHECK_SIGN2,UNC	;
******************************************************************************
* NUM AND DEN ARE EQUAL, RETURN 0
******************************************************************************
EQ2:
	ZAPA       		; load return register with 0
	SUBB	SP,#FSIZE	; deallocate frame
	LRETR     		; return


******************************************************************************
* FOLLOWING PIECE OF CODE IS FAST OUT IF UPPER WORD OF DENOMINATOR IS ZERO.
* THE RESULT (REMAINDER) IS IN ACC:P UPON RETURN.
******************************************************************************
SMODUDenHighZero:
	MOVB	ACC,#0
	MOVL	P, LNUM_HI
	RPT	#31
      ||SUBCUL  ACC, LDEN_LO
        MOVL    P, LNUM_LO
	RPT	#31
      ||SUBCUL  ACC, LDEN_LO
        MOVL    P, ACC
        MOVB    ACC, #0
        B       CHECK_SIGN2, UNC
	.endasmfunc

	.page
******************************************************************************
* 32-bit UNSIGNED DIVIDE, CALCULATE NUM / DEN AND RETURN IN ACCUMULATOR
******************************************************************************
	.global	ULL$$DIV

ULL$$DIV:	.asmfunc stack_usage(14)
******************************************************************************
* ALLOCATE FRAME
******************************************************************************
	MOVB	TMP32,#0
	PUSH	TMP32
	PUSH	TMP32
	PUSH	TMP32
	PUSH	TMP32
	PUSH	TMP32
	PUSH	TMP32

******************************************************************************
* LOOK FOR EASY OUTS
******************************************************************************
	MOVL	LNUM_HI, ACC    ; Save off NUM_HI
	MOVL	LNUM_LO, P      ; Save off NUM_LO
	CMPL	ACC,DEN_HI	;
	B	$35,NEQ		;
	MOVL	ACC,P		;
	CMPL	ACC,DEN_LO	; compare denominator to numerator
$35
	B	EQ3,EQ		; branch if equal (return 1)
	B	DGT3,LO  	; branch if denominator > numerator (return 0) 

	MOVL	ACC,DEN_HI	;
        SB      DIVUDenHighZero,EQ

******************************************************************************
* CALL MAIN DIVIDE ROUTINE
******************************************************************************
	MOVL	ACC, DEN_HI	; Move the denomiator to local variable 
	MOVL	LDEN_HI, ACC	; that MAINDIV accesses
	MOVL	ACC, DEN_LO	;
	MOVL	LDEN_LO, ACC	;

	MOVL	ACC,LNUM_HI	; Load numerator to ACC:P to pass to MAINDIV
	LCR	MAINDIV		; call main divide routine
	MOVL	ACC,Q_HI        ; load result into ACC:P
	MOVL	P,Q_LO          ; 

******************************************************************************
* QUOTIENT IS IN ACC
******************************************************************************
RET3:
	SUBB	SP,#FSIZE	; deallocate frame
	LRETR    		; return

******************************************************************************
* NUM AND DWN ARE EQUAL, RETURN 1
******************************************************************************
EQ3:
	MOV	PL,#1		;
	MOV	PH,#0		;
	MOVB	ACC,#0		; quotient = 1
	B	RET3,UNC	; return

******************************************************************************
* IF DEN > NUM, RETURN 0
******************************************************************************
DGT3:
	ZAPA        		; set quotient to zero
	B	RET3,UNC	; return

******************************************************************************
* FOLLOWING PIECE OF CODE IS FAST OUT IF UPPER WORD OF DENOMINATOR IS ZERO
* THE RESULT (QUOTIENT) IS IN ACC:P UPON RETURN.
******************************************************************************
DIVUDenHighZero:
	MOVB	ACC,#0
	MOVL	P, LNUM_HI
	RPT	#31
      ||SUBCUL  ACC, DEN_LO
        MOVL    RES_HI, P
        MOVL    P, LNUM_LO
	RPT	#31
      ||SUBCUL  ACC, DEN_LO
	MOVL	ACC, RES_HI
        B       RET3, UNC
	.endasmfunc



	.page
******************************************************************************
* 32-bit UNSIGNED MODULUS, CALCULATE NUM % DEN AND RETURN IN ACCUMULATOR
******************************************************************************
	.global	ULL$$MOD

ULL$$MOD:	.asmfunc stack_usage(14)
******************************************************************************
* ALLOCATE FRAME
******************************************************************************
	MOVB	TMP32,#0
	PUSH	TMP32
	PUSH	TMP32
	PUSH	TMP32
	PUSH	TMP32
	PUSH	TMP32
	PUSH	TMP32

******************************************************************************
* LOOK FOR EASY OUTS
******************************************************************************
	MOVL	LNUM_HI, ACC    ; save off NUM_HI
	MOVL	LNUM_LO, P      ; save off NUM_LO
	CMPL	ACC,DEN_HI	;
	B	$40,NEQ		;
	MOVL	ACC,P		;
	CMPL	ACC,DEN_LO	; compare denominator to numerator
$40
	B	EQ4,EQ		; branch if equal (return 0)
	B	RET_NUM2,LO     ; if (denominator > numerator) return numerator
        MOVL	ACC, DEN_HI     ; check if upper word of denominator is zero
        SB      MODUDenHighZero,EQ

******************************************************************************
* CALL MAIN DIVIDE ROUTINE, MOVE REMAINDER INTO ACC
******************************************************************************
	MOVL	ACC, DEN_HI	; Move the denomiator to local variable 
	MOVL	LDEN_HI, ACC	; that MAINDIV accesses
	MOVL	ACC, DEN_LO	;
	MOVL	LDEN_LO, ACC	;

	MOVL	ACC,LNUM_HI	; Load numerator to ACC:P to pass to MAINDIV
	LCR	MAINDIV		; call main divide routine
	MOVL	ACC,REM_HI	; load remainder into ACC:P
	MOVL	P,REM_LO	; 

******************************************************************************
* RETURN SEQUENCE
******************************************************************************
RET4:
	SUBB	SP,#FSIZE	; deallocate frame
	LRETR	  		; return

RET_NUM2:
	MOVL	ACC,LNUM_HI	;
	B	RET4, UNC	;
******************************************************************************
* NUM AND DEN ARE EQUAL, RETURN 0
******************************************************************************
EQ4:
	ZAPA       	 	; load return ACC:P with 0
	B	RET4,UNC	; return

******************************************************************************
* FOLLOWING PIECE OF CODE IS FAST OUT IF UPPER WORD OF DENOMINATOR IS ZERO.
* THE RESULT (REMAINDER) IS IN ACC:P UPON RETURN.
******************************************************************************
MODUDenHighZero:
	MOVB	ACC,#0
	MOVL	P, LNUM_HI
	RPT	#31
      ||SUBCUL  ACC, DEN_LO
        MOVL    P, LNUM_LO
	RPT	#31
      ||SUBCUL  ACC, DEN_LO
        MOVL	P,ACC
	MOVB	ACC,#0
        B       RET4, UNC
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
*    count = 63;
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
* INPUT : NUMERATOR IN ACC:P. DENOMINATOR IN MD_LDEN_HI:MD_LDEN_LO
*         MD_REM_HI:MD_REM_LO and MD_Q_HI:MD_Q_LO are initilaized to zero.
*
* OUTPUT: REMAINDER IN MD_REM_HI:MD_REM_LO AND QUOTIENT IS IN MD_Q_HI:MD_Q_LO
*
* LOCALS: QUOT_HI:QUOT_LO - intermediate quotient.
*         CNT	- loop count (ARn)
*         TMP16 - loc16 temp for the normalize macro.
******************************************************************************
MAINDIV:	.asmfunc stack_usage(2)
        MOVB    XCNT,#63     	; Initialize loop counter
	CMP64   ACC:P		; Clear V flag
	CMP64   ACC:P		; Test contents of ACC:P

        SB      LOOP,LT         ; If negative, then skip normalization
 
 	NORMALIZE64 CNT, TMP16	;

        LSL64   ACC:P,#1        ; If normalized, bit-63= 0
	SUBB	XCNT,#1
LOOP:
        LSL64   ACC:P,#1        ; Shift bit into Carry (C)

        MOVL    QUOT_HI,ACC     ; Store shifted intermediate quotient
        MOVL    QUOT_LO,P       ; Store shifted intermediate quotient

        MOVL    ACC,MD_REM_LO   ; Load Remainder low
        ROL     ACC             ; Shift Carry into bit-0
        MOVL    MD_REM_LO,ACC   ; Store Remainder
	MOVL    P,ACC		;
        MOVL    ACC,MD_REM_HI   ; Load Remainder high
        ROL     ACC             ; Shift Carry from remainder low into bit-32
        MOVL    MD_REM_HI,ACC   ; Store Remainder

	CMPL	ACC,MD_LDEN_HI  ;
	B	MD_NEQ, NEQ	;
	MOVL	ACC,P		;
	CMPL	ACC,MD_LDEN_LO	;
MD_NEQ:
	B	SKIP, LO	;

	MOVL	ACC, MD_REM_HI  ;
	SUBUL	P, MD_LDEN_LO	;
	SUBBL	ACC, MD_LDEN_HI ;

        MOVL    MD_REM_HI,ACC   ; else, store new Remainder
        MOVL    MD_REM_LO,P     ; else, store new Remainder
	SETC	C		;
	B	SKIP1,UNC	;
SKIP:
	MOVL	ACC, MD_REM_HI  ;
	CLRC	C		;
SKIP1:
        MOVL    ACC,MD_Q_LO     ; Load Quotient low (Carry preserved)
        ROL     ACC             ; Shift carry into bit-0
	MOVL	MD_Q_LO,ACC
        MOVL    ACC,MD_Q_HI     ; Load Quotient high (Carry preserved)
        ROL     ACC             ; Shift carry into bit-32
	MOVL	MD_Q_HI,ACC

        MOVL    ACC,QUOT_HI     ; Reload intermediate quotient
        MOVL    P,QUOT_LO       ; Reload intermediate quotient
        BANZ    LOOP,CNT--      ; Loop until count = 0
 
        LRETR                   ; Return to setup routine
	.endasmfunc
 
