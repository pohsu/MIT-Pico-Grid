;***************************************************************************
;* setjmp longjmp v15.12.3
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
;***************************************************************************
*    C syntax:       int setjmp(jmp_buf env)
*
*    Description:    Save caller's current environment for a subsequent
*                    call to longjmp.  Return 0.
*    Return:         Always returns 0.                    
****************************************************************************
        .global  _setjmp, __setjmp, _longjmp

****************************************************************************
*  NOTE : ANSI specifies that "setjmp.h" declare "setjmp" as a macro. 
*         In our implementation, the setjmp macro calls a function "_setjmp".
*         However, since the user may not include "setjmp.h", we provide
*         two entry-points to this function.
****************************************************************************
__setjmp:	.asmfunc
_setjmp:
        .if __LARGE_MODEL__
        MOV     AL,SP           ; save SP, upon entry env is pointed at by AR4
        MOV     *XAR6++,AL
 
        MOVL    ACC,*-SP[2]     ; save return address
        MOV     *XAR6++,AL      ; store 1 word at a time since alignment of
        MOV     *XAR6++,AH      ; jmp_buf is unknown
 
        MOV     *XAR6++,AR1     ; save registers compiler convention requires
        MOV     *XAR6++,AR2     ; to be saved on function entry
        MOV     *XAR6,AR3
 
        MOVB    AL,#0           ; set return value to 0

        .else
	MOV	AL,SP		; save SP, upon entry env is pointed at by AR4
	MOV	*AR4++,AL	

	MOVL	ACC,*-SP[2]	; save return address
	MOV	*AR4++,AL	; store 1 word at a time since alignment of
	MOV	*AR4++,AH	; jmp_buf is unknown

	MOV	*AR4++,AR1	; save registers compiler convention requires
	MOV	*AR4++,AR2	; to be saved on function entry
	MOV	*AR4,AR3

	MOVB	AL,#0		; set return value to 0	
        .endif

	RET			; return
	.endasmfunc

        .page
****************************************************************************
*    C syntax:    void longjmp(jmp_buf env, int returnvalue)
*
*    Description: Restore the context contained in the jump buffer.
*                 This causes an apparent "2nd return" from the
*                 setjmp invocation which built the "env" buffer.
*
*    Return:      This return appears to return "returnvalue", which must 
*                 be non-zero.
*
****************************************************************************
_longjmp:	.asmfunc
        .if __LARGE_MODEL__
        CMPB    AL,#0           ; ensure that returnvalue will be non-zero
        B       L1,NEQ          ; if (returnvalue == 0) return 1
        MOVB    AL,#1           ;
L1:
        MOV     AR1,*XAR6++     ; set SP to value stored in env
        MOV     SP,AR1
        MOV     AR1,*XAR6++     ; put new return address on stack
        MOV     *-SP[2],AR1
        MOV     AR1,*XAR6++
        MOV     *-SP[1],AR1
 
        MOV     AR1,*XAR6++     ; restore registers that compiler conventions
        MOV     AR2,*XAR6++      ; require to be restored on function return
        MOV     AR3,*XAR6

        .else
	CMPB	AL,#0		; ensure that returnvalue will be non-zero
	B	L1,NEQ		; if (returnvalue == 0) return 1
	MOVB	AL,#1		; 
L1:
	MOV	AR1,*AR4++	; set SP to value stored in env
	MOV	SP,AR1
	MOV	AR1,*AR4++	; put new return address on stack
	MOV	*-SP[2],AR1     
	MOV	AR1,*AR4++	
	MOV	*-SP[1],AR1

	MOV	AR1,*AR4++	; restore registers that compiler conventions
	MOV	AR2,*AR4++      ; require to be restored on function return
	MOV	AR3,*AR4
        .endif

	RET			; return
	.endasmfunc
