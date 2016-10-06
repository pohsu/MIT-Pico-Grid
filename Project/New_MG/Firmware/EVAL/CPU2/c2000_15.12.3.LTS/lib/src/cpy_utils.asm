;******************************************************************************
;* CPY_UTILS v15.12.3                                                         *
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
;******************************************************************************
        .include "c2000asm.inc"

;********************************************************************
; CPY_UTILS.ASM
;
; Copy utility functions to perform 4 different types of copy,
; data -> data, data -> program, program -> data, and
; program -> program.
;
; source address always arrives in the accumulator 'ACC'
; destination address arrives on the stack '*-SP[2]'
;
; We will use XAR6 and XAR7 as temporary registers in 
; these routines.
;
;********************************************************************

	.if	.TMS320C2800
;********************************************************************
; In C28x, the memory is unified and hence the data and program can be
; accessed using data or program access. So the 4 different types of 
; copy ends up being the same. Since we don't support huge model to reach
; all the 32-bit data address space in the compiler here we assume that
; both data and program address range are limited to 0x3fffff. Hence 
; we use RPT PREAD. The user can modify this implementation to suit
; their needs.
;********************************************************************
	.sect	".text"
	.global	_ddcopy
	.global	_dpcopy
	.global	_pdcopy
	.global	_ppcopy
_ddcopy:
_dpcopy:
_pdcopy:
_ppcopy:
        MOVL    XAR7,ACC         ; place source addr into XAR7   
        MOVL    XAR4,*-SP[4]     ; load destination addr into XAR4
        MOVL    ACC,*-SP[6]      ; load copy size in bytes into ACC

        CMPB    AL, #0
        B       $10, EQ          ; If AL is zero jump to check AH

        SUBB    AL, #1           ; First copy LO16(copy_size) bytes.
        RPT     AL
     || PREAD	*XAR4++,*XAR7

$10
        CMPB    AH, #0           ; 
        B       $30, EQ          ; If AH is zero then we are done.
                                 ; else copy remaining data in 64K chunks
        MOV     PL, *-SP[6]      ; 
        MOV     PH, #0           ; Since XAR7 is not updated in PREAD,         
        ADDUL   P, XAR7          ; keep track of the udpated source addr in P
$20:
        MOVL    XAR7, P          ; Get the source address from P
                                 ; A simulator bug prevents using 0xffff as
        PREAD	*XAR4++,*XAR7    ; repeat count. So copy one byte and do   
                                 ; repeat pread on 64K-1 bytes.
        ADDB    XAR7, #1         ; 
        MOV     AL, #0xfffe      ; 
        RPT     AL
     || PREAD	*XAR4++,*XAR7

        ADD     PH, #1           ; Update the source address in P.
        LSR     AH, #1           ; Copy 64K chunks of data repeatedly until
        BF      $20, NEQ         ; all the bytes are copied.
$30:
        LRETR
        .else

	.sect	".text"
	.global	_ddcopy
;********************************************************************
; Data Memory -> Data Memory
;********************************************************************
_ddcopy:
        MOVL    XAR6,ACC         ; place source addr into XAR6
        MOV     XAR7,*-SP[4]     ; load destination addr into XAR7
        MOVL    ACC,*-SP[6]      ; load copy size in bytes into ACC
        B       $20, EQ
$10:
        MOV	AR4,*XAR6++      ; copy copy_size bytes in this loop
        SUBB    ACC,#1
        MOV	*XAR7++,AR4
        B       $10, NEQ

$20:
        RET

	.sect	".text"
	.global	_dpcopy
;********************************************************************
; Data Memory -> Program Memory
;
; Transfer the dest address into XAR7 so that we can use a PWRITE
; instruction to store the data into program memory.
;********************************************************************
_dpcopy:
        MOVL    XAR6,ACC         ; place source addr into XAR7   
        MOV     XAR7,*-SP[4]     ; load destination addr into XAR4
        MOVL    ACC,*-SP[6]      ; load copy size in bytes into ACC
        B       $40, EQ          
$30:
        PWRITE  *XAR7,*XAR6++    ; copy copy_size bytes in this loop
        ADDB    XAR7, #1
        SUBB    ACC, #1 
        B       $30, NEQ
$40:
        RET   

	.sect	".text"
	.global	_pdcopy
;********************************************************************
; Program Memory -> Data Memory
;
; Put source address into XAR7 so we can use a PREAD instruction to
; get the data from program memory.
;********************************************************************
_pdcopy:
        MOVL    XAR7,ACC         ; place source addr into XAR7   
        MOV     XAR6,*-SP[4]     ; load destination addr into XAR4
        MOVL    ACC,*-SP[6]      ; load copy size in bytes into ACC
        B       $60, EQ          
$50:
        PREAD   *XAR6++,*XAR7    ; copy copy_size bytes in this loop
        ADDB    XAR7, #1
        SUBB    ACC, #1 
        B       $50, NEQ
$60:
        RET   

	.sect	".text"
	.global	_ppcopy
;********************************************************************
; Program Memory -> Program Memory
;
; First read the data from program memory into the accumulator.
; Then store it into the destination location (also in program 
; memory).  We use XAR7 to hold first the source address and then
; the dest address so that we can make use of the PREAD and 
; PWRITE instructions for reading/writing data from/to program
; memory.
;********************************************************************
        .asg *-SP[2], SADDR
        .asg XAR6,    DADDR
_ppcopy:
        PUSH    ACC              ; push source address into stack
        MOV     DADDR,*-SP[6]    ; load destination addr into DADDR
        MOVL    ACC,*-SP[8]      ; load copy size in bytes into ACC
        B       $80, EQ
$70:
        MOV 	XAR7,SADDR       ; Load Source Address into XAR7
	PREAD	AR4,*XAR7        ; Read from Program Memory into AR4
        MOV     AR5, *XAR7++     ; Increment XAR7; AR5 is discarded
        MOV     SADDR,XAR7       ; Store incremented source address 

        MOV   	XAR7,DADDR       ; Load destination address into XAR7
	PWRITE	*XAR7,AR4        ; Write AR4 contents to Program Memory
        MOV     AR5, *XAR7++     ; Increment XAR7; AR5 is discarded
        MOV     DADDR,XAR7       ; Store incremented destination address 

        SUBB    ACC, #1          ; decrement count and return when zero
        B       $70, NEQ         ; 
$80:
        RET
        .endif

