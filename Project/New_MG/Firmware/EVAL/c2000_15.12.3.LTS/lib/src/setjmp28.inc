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
__setjmp:	.asmfunc stack_usage(4)
_setjmp:
	PUSH	RPC             ; save return address
	MOVL	ACC,*--SP
	MOVL	*XAR4++,ACC     ; store return address

	MOV	AL,SP		; save SP, upon entry env is pointed at by AR4
	MOV	*XAR4++,AL	
        MOV     *XAR4++,AL      ; align the buffer to 32-bit boundary.

        MOVL    *XAR4++,XAR1    ; to be saved on function entry
        MOVL    *XAR4++,XAR2    ; to be saved on function entry
        MOVL    *XAR4,XAR3      ; to be saved on function entry

	MOVB	AL,#0		; set return value to 0	

	LRETR			; return
	.endasmfunc

        .page
****************************************************************************
*    C++ syntax:    void longjmp(jmp_buf env, int returnvalue)
*
*    Description: Restore the context contained in the jump buffer.
*                 This causes an apparent "2nd return" from the
*                 setjmp invocation which built the "env" buffer.
*
*    Return:      This return appears to return "returnvalue", which must 
*                 be non-zero.
*
****************************************************************************
_longjmp:	.asmfunc stack_usage(4)
	CMPB	AL,#0		; ensure that returnvalue will be non-zero
	B	L1,NEQ		; if (returnvalue == 0) return 1
	MOVB	AL,#1		; 
L1:
        MOVL    XAR1, *XAR4++
        PUSH    XAR1            ; put new return address on stack
	POP	RPC             ; pop new return address

	MOVZ	AR1,*XAR4++	; set SP to value stored in env
	MOV	SP,AR1
	MOVZ	AR1,*XAR4++	; Ignore alignment hole.          

	MOVL    XAR1,*XAR4++    ; restore register that compiler conventions
        MOVL    XAR2,*XAR4++    ; require to be restored on function return
        MOVL    XAR3,*XAR4    

	LRETR			; return
	.endasmfunc
