/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd.
* ALL rights reserved.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*
* The License of this software follows LGPL v2.1, See the LICENSE for more details:
* https://opensource.org/licenses/LGPL-2.1
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/


/*
	   Handler's Stack
	    +-----------+
	    |           |
	    +-----------+
	    |    SS     |    + 40
	    +-----------+
	    |    RSP    |    + 32
	    +-----------+
	    |   RFLAGS  |    + 24
	    +-----------+
	    |    CS     |    + 16
	    +-----------+
	    |    RIP    |    + 8
	    +-----------+
	    |  ERR CODE |    + 0 ==>>  Stack pointer after transfer
	    +-----------+        to handler
	    |           |
	    +-----------+

*/

#include "x64_arch_def.h"


#define X64_EXCEPT_HANDLE_WEC_WRAP(name, inum) \
		 .global name##_exception_handler; \
		 .type name##_exception_handler, %function; \
		 .code64; \
		  name##_exception_handler: \
		  push    %rdi; \
		  mov     $inum,    %rdi; \
		  jmp     _isr_handler_; \
          .data; \
          .quad    name##_exception_handler; \
          .text


#define X64_EXCEPT_HANDLE_NEC_WRAP(name, inum) \
		 .global name##_exception_handler; \
		 .type name##_exception_handler, %function; \
		 .code64; \
		  name##_exception_handler: \
		  pushq    $0; \
		  pushq    %rdi; \
		  mov      $inum,    %rdi; \
		  jmp      _isr_handler_; \
          .data; \
          .quad    name##_exception_handler; \
          .text

#define X64_INTERRUPT_HANDLE_WRAP(inum) \
		 .global exit##inum##_handler; \
		 .type exit##inum##_handler, %function; \
		 .code64; \
		  exit##inum##_handler: \
		  pushq    $0; \
		  pushq    %rdi; \
		  mov      $inum,    %rdi; \
		  jmp      _isr_handler_; \
          .data; \
          .quad    exit##inum##_handler; \
          .text



/**
 * \brief x64 exception handler
 */
    .global x64_common_isr
    .code64
_isr_handler_:
    pushq    %rsi
    pushq    %rax
    pushq    %rbx
    pushq    %rcx
    pushq    %rdx
    pushq    %r8
    pushq    %r9
    pushq    %r10
    pushq    %r11
    pushq    %r12
    pushq    %r13
    pushq    %r14
    pushq    %r15

    /**
     * \note The destination operand contains the first byte of the memory image,
     * and it must be aligned on a 16-byte boundary. A misaligned destination
     * operand will result in a general-protection exception being generated
     * (or in some cases, an alignment check exception).
     */
    leaq       -(8 + 512)(%rsp), %rsp
    fxsaveq    (%rsp)
    //finit ?

    call    x64_common_isr

    fxrstorq    (%rsp)
    leaq    (8 + 512)(%rsp), %rsp
    popq    %r15
	popq    %r14
	popq    %r13
	popq    %r12
	popq    %r11
	popq    %r10
	popq    %r9
	popq    %r8
	popq    %rdx
	popq    %rcx
	popq    %rbx
	popq    %rax
    popq    %rsi
    popq    %rdi

    iretq


    .data
    .align 8
    .type g_exception_handler_table_addr, %object
_exception_table_head_:
g_exception_handler_table_addr:
	X64_EXCEPT_HANDLE_NEC_WRAP(divide_error, X64_DIVIDE_ERROR)
	X64_EXCEPT_HANDLE_NEC_WRAP(debug, X64_DEBUG)
	X64_EXCEPT_HANDLE_NEC_WRAP(nmi, X64_NON_MASKABLE_INTERRUPT)
	X64_EXCEPT_HANDLE_NEC_WRAP(breakpoint, X64_BREAKPOINT)
	X64_EXCEPT_HANDLE_NEC_WRAP(overflow, X64_OVERFLOW)
	X64_EXCEPT_HANDLE_NEC_WRAP(bound_range, X64_BOUND_RANGE)
	X64_EXCEPT_HANDLE_NEC_WRAP(invalid_opcode, X64_INVALID_OPCODE)
	X64_EXCEPT_HANDLE_NEC_WRAP(device_not_available, X64_DEVICE_NOT_AVAILABLE)
	X64_EXCEPT_HANDLE_WEC_WRAP(double_fault, X64_DOUBLE_FAULT)
	X64_EXCEPT_HANDLE_NEC_WRAP(coprocessor_segment_overrun, X64_COPROCESSOR_SEGMENT_OVERRUN)
	X64_EXCEPT_HANDLE_WEC_WRAP(invalid_tss, X64_INVALID_TSS)
	X64_EXCEPT_HANDLE_WEC_WRAP(segment_not_present, X64_SEGMENT_NOT_PRESENT)
	X64_EXCEPT_HANDLE_WEC_WRAP(stack_fault, X64_STACK_FAULT)
	X64_EXCEPT_HANDLE_WEC_WRAP(general_protection, X64_GENERAL_PROTECTION)
	X64_EXCEPT_HANDLE_WEC_WRAP(page_fault, X64_PAGE_FAULT)
	X64_EXCEPT_HANDLE_NEC_WRAP(reserved, X64_RESERVED)
	X64_EXCEPT_HANDLE_NEC_WRAP(fpu_fp_error, X64_X87_FPU_FP_ERROR)
	X64_EXCEPT_HANDLE_WEC_WRAP(aligment_check, X64_ALIGNMENT_CHECK)
	X64_EXCEPT_HANDLE_NEC_WRAP(machine_cahck, X64_MACHINE_CHECK)
	X64_EXCEPT_HANDLE_NEC_WRAP(simd_fp, X64_SIMD_FP)
	X64_EXCEPT_HANDLE_WEC_WRAP(virt_exception, X64_VIRT_EXCEPTION)
	//X64_EXCEPT_HANDLE_NEC_WRAP(divide_error, X64_SECURITY_EXCEPTION)

    .data
    .global g_exception_handler_table_addr
    .global g_exception_handler_table_bytes
_exception_table_end_:
g_exception_handler_table_bytes:
    .word    (_exception_table_end_  -  _exception_table_head_)


    .data
    .align 8
    .type g_interrupt_handler_table, %object
_interrupt_table_head_:
g_interrupt_handler_table_addr:
	X64_INTERRUPT_HANDLE_WRAP(0)
	X64_INTERRUPT_HANDLE_WRAP(1)
	X64_INTERRUPT_HANDLE_WRAP(2)
	X64_INTERRUPT_HANDLE_WRAP(3)
	X64_INTERRUPT_HANDLE_WRAP(4)
	X64_INTERRUPT_HANDLE_WRAP(5)
	X64_INTERRUPT_HANDLE_WRAP(6)
	X64_INTERRUPT_HANDLE_WRAP(7)
	X64_INTERRUPT_HANDLE_WRAP(8)
	X64_INTERRUPT_HANDLE_WRAP(9)
	X64_INTERRUPT_HANDLE_WRAP(10)
	X64_INTERRUPT_HANDLE_WRAP(11)
	X64_INTERRUPT_HANDLE_WRAP(12)
	X64_INTERRUPT_HANDLE_WRAP(13)
	X64_INTERRUPT_HANDLE_WRAP(14)
	X64_INTERRUPT_HANDLE_WRAP(15)
	X64_INTERRUPT_HANDLE_WRAP(16)

	.data
    .global g_interrupt_handler_table_addr
    .global g_interrupt_handler_table_bytes
_interrupt_table_end_:
g_interrupt_handler_table_bytes:
    .word    (_interrupt_table_end_  -  _interrupt_table_head_)




