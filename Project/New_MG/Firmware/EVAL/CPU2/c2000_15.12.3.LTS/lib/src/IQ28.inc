;******************************************************************************
;* IQ CONSTRUCTOR CODE FOR C28X v15.12.3                                      *
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
;******************************************************************************
	.sect	".text"
	.global	__IQ
        .asg   *+XAR4[3],  OP1_SIGN
        .asg   *+XAR4[2],  OP1_MSW
        .asg   *+XAR4[0],  OP1_LSW
	.asg   AR5, 	   INP_N
	.asg   AR4, 	   EXP  

__IQ:	.asmfunc stack_usage(2)
	MOVZ	  INP_N, AL		; Save N
	MOVL	  ACC, OP1_MSW		; Load the input into ACC:P
	MOVL	  P,   OP1_LSW		;
        CMP64     ACC:P                 ; Clear V flag
        CMP64     ACC:P                 ; check for 0
	B         EXIT,EQ

	MOVL      XAR7,ACC              ; save upper half of A in XAR7

	ASR	  AH,15
	MOVZ      AR6,AH                ; save sign in AR6

	MOVL	  ACC,XAR7              ; find the exp
        LSR       AH,4                  
        AND       AH,AH,#0x7ff              
	MOV	  AL,AH
	MOVB      AH,#0
	SETC      SXM
        ADD       ACC,#-1023            ; ACC contains the exp
	CLRC      SXM
	B         NEG_EXP,LT            ; branch if exp < 0

	MOVZ      EXP,AL                ; save exp in AR5

        MOVB      AL,#30                ; determine if exp > (30 - N)
	SUB       AL,INP_N              ; AL contains (30 - N)
	MOV       AH,EXP
	SUB       AH,AL                 ; AH contains (exp - (30 - N))
	B         SATURATE,GT           ; saturate if int val >= I bits
	B         FIND_I,UNC

NEG_EXP:
	NEG       AL
	MOVZ      EXP,AL                ; save exp in AR5
	SUBB	  AL,#1                 ; exp - 1 = number of leading zeros
	SUB       AL,INP_N
	B         ZERO_RESULT,GT        ; return 0 number of leading 0's > Q

        MOVL      ACC,XAR7
	ANDB      AH,#0xf               ; zero sign and exp
	ADD       AH,#0x10              ; add implied 1
	LSL64     ACC:P,#0xb            ; shift frac << 11
	MOVL      XAR7,ACC              ; save frac in XAR6:XAR7
	MOV       AH,EXP                ; shift = (exp - 1)
	SUBB      AH,#1
	MOV       T,AH
	MOVL      ACC,XAR7
	LSR64     ACC:P,T               ; shift frac >> (exp - 1)
	MOVL      P,ACC                 ; move frac to P
	MOVB      ACC,#0                ; set I to 0
	MOVL      XAR7,ACC              ; save result back to XAR6:XAR7
	B         FIND_RESULT,UNC

FIND_I:
        MOVL      ACC,XAR7              ; move int from XAR7 to ACC
        ANDB      AH,#0xf               ; clear sign and exp
	ADD 	  AH,#0x10              ; add implied 1 
	LSL64     ACC:P,#0xb            ; shift << 11
	MOVL	  XAR7,ACC		

        MOVB      AL,#31                ; find shift amount
        SUB       AL,EXP                ; shift = 31 - exp
        MOV       T,AL                   
        MOVL      ACC,XAR7           
        LSR64     ACC:P,T               ; shift >> (31 - exp);
        MOVL      XAR7,ACC              

FIND_RESULT:
	MOVL      ACC,XAR7              ; move I to ACC

	MOV	  T,INP_N
	LSL64	  ACC:P,T               ; shift I:Q by N

        TBIT      PH,#15                ; check if must round result
	B         CHECK_SIGN,NTC

	TBIT      AR6,#0
	B         ROUND,TC              ; round negative results

	MOV       PL,#65535
	MOV       PH,#32767
	CMPL      ACC,P
	B         CHECK_SIGN,EQ         ; Don't round if result is saturated

ROUND:
	ADDB      ACC,#1                ; round ACC

CHECK_SIGN:	
        TBIT	  AR6,#0                ; check sign bit
	B	  EXIT,NTC              ; negate result if necessary

	NEG       ACC                   ; negate result
	B 	  EXIT,UNC

ZERO_RESULT:
	MOVB	  ACC,#0
	B         EXIT,UNC

SATURATE:
        MOV       AL,AR6
	B         UNDERFLOW,NEQ

        MOV       AH,#32767
        MOV       AL,#65535             ; return 0x7fffffff
        B         EXIT,UNC              

UNDERFLOW:
        MOV       AH,#32768
        MOV       AL,#0                 ; return 0x80000000
        B         EXIT,UNC               

EXIT:    
        LRETR
	.endasmfunc
