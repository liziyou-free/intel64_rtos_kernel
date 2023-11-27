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
         .text; \
         .func; \
         .code64; \
          name##_exception_handler: \
          push    %rdi; \
          mov     $inum,    %rdi; \
          jmp     _isr_handler_; \
         .data; \
         .quad    name##_exception_handler; \
         .endfunc; \


#define X64_EXCEPT_HANDLE_NEC_WRAP(name, inum) \
         .global name##_exception_handler; \
         .type name##_exception_handler, %function; \
         .text; \
         .func; \
         .code64; \
          name##_exception_handler: \
          pushq    $0; \
          pushq    %rdi; \
          mov      $inum,    %rdi; \
          jmp      _isr_handler_; \
         .data; \
         .quad     name##_exception_handler; \
         .endfunc; \
         .text


#define X64_INTERRUPT_HANDLE_WRAP(inum) \
         .global exit##inum##_handler; \
         .type exit##inum##_handler, %function; \
         .text; \
         .func; \
         .code64; \
          exit##inum##_handler: \
          pushq    $0; \
          pushq    %rdi; \
          mov      $inum,    %rdi; \
          jmp      _isr_handler_; \
          .data; \
          .quad    exit##inum##_handler; \
          .endfunc; \
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
    pushq    %rbp
    pushq    %r8
    pushq    %r9
    pushq    %r10
    pushq    %r11
    pushq    %r12
    pushq    %r13
    pushq    %r14
    pushq    %r15
    /* don't push ds,gs,fs,es */

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
    popq    %rbp
    popq    %rdx
    popq    %rcx
    popq    %rbx
    popq    %rax
    popq    %rsi
    popq    %rdi
    /* Clean up error codes pushed to the stack */
    leaq    8(%rsp),    %rsp
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
    X64_INTERRUPT_HANDLE_WRAP(32)
    X64_INTERRUPT_HANDLE_WRAP(33)
    X64_INTERRUPT_HANDLE_WRAP(34)
    X64_INTERRUPT_HANDLE_WRAP(35)
    X64_INTERRUPT_HANDLE_WRAP(36)
    X64_INTERRUPT_HANDLE_WRAP(37)
    X64_INTERRUPT_HANDLE_WRAP(38)
    X64_INTERRUPT_HANDLE_WRAP(39)
    X64_INTERRUPT_HANDLE_WRAP(40)
    X64_INTERRUPT_HANDLE_WRAP(41)
    X64_INTERRUPT_HANDLE_WRAP(42)
    X64_INTERRUPT_HANDLE_WRAP(43)
    X64_INTERRUPT_HANDLE_WRAP(44)
    X64_INTERRUPT_HANDLE_WRAP(45)
    X64_INTERRUPT_HANDLE_WRAP(46)
    X64_INTERRUPT_HANDLE_WRAP(47)
    X64_INTERRUPT_HANDLE_WRAP(48)
    X64_INTERRUPT_HANDLE_WRAP(49)
    X64_INTERRUPT_HANDLE_WRAP(50)
    X64_INTERRUPT_HANDLE_WRAP(51)
    X64_INTERRUPT_HANDLE_WRAP(52)
    X64_INTERRUPT_HANDLE_WRAP(53)
    X64_INTERRUPT_HANDLE_WRAP(54)
    X64_INTERRUPT_HANDLE_WRAP(55)
    X64_INTERRUPT_HANDLE_WRAP(56)
    X64_INTERRUPT_HANDLE_WRAP(57)
    X64_INTERRUPT_HANDLE_WRAP(58)
    X64_INTERRUPT_HANDLE_WRAP(59)
    X64_INTERRUPT_HANDLE_WRAP(60)
    X64_INTERRUPT_HANDLE_WRAP(61)
    X64_INTERRUPT_HANDLE_WRAP(62)
    X64_INTERRUPT_HANDLE_WRAP(63)
    X64_INTERRUPT_HANDLE_WRAP(64)
    X64_INTERRUPT_HANDLE_WRAP(65)
    X64_INTERRUPT_HANDLE_WRAP(66)
    X64_INTERRUPT_HANDLE_WRAP(67)
    X64_INTERRUPT_HANDLE_WRAP(68)
    X64_INTERRUPT_HANDLE_WRAP(69)
    X64_INTERRUPT_HANDLE_WRAP(70)
    X64_INTERRUPT_HANDLE_WRAP(71)
    X64_INTERRUPT_HANDLE_WRAP(72)
    X64_INTERRUPT_HANDLE_WRAP(73)
    X64_INTERRUPT_HANDLE_WRAP(74)
    X64_INTERRUPT_HANDLE_WRAP(75)
    X64_INTERRUPT_HANDLE_WRAP(76)
    X64_INTERRUPT_HANDLE_WRAP(77)
    X64_INTERRUPT_HANDLE_WRAP(78)
    X64_INTERRUPT_HANDLE_WRAP(79)
    X64_INTERRUPT_HANDLE_WRAP(80)
    X64_INTERRUPT_HANDLE_WRAP(81)
    X64_INTERRUPT_HANDLE_WRAP(82)
    X64_INTERRUPT_HANDLE_WRAP(83)
    X64_INTERRUPT_HANDLE_WRAP(84)
    X64_INTERRUPT_HANDLE_WRAP(85)
    X64_INTERRUPT_HANDLE_WRAP(86)
    X64_INTERRUPT_HANDLE_WRAP(87)
    X64_INTERRUPT_HANDLE_WRAP(88)
    X64_INTERRUPT_HANDLE_WRAP(89)
    X64_INTERRUPT_HANDLE_WRAP(90)
    X64_INTERRUPT_HANDLE_WRAP(91)
    X64_INTERRUPT_HANDLE_WRAP(92)
    X64_INTERRUPT_HANDLE_WRAP(93)
    X64_INTERRUPT_HANDLE_WRAP(94)
    X64_INTERRUPT_HANDLE_WRAP(95)
    X64_INTERRUPT_HANDLE_WRAP(96)
    X64_INTERRUPT_HANDLE_WRAP(97)
    X64_INTERRUPT_HANDLE_WRAP(98)
    X64_INTERRUPT_HANDLE_WRAP(99)
    X64_INTERRUPT_HANDLE_WRAP(100)
    X64_INTERRUPT_HANDLE_WRAP(101)
    X64_INTERRUPT_HANDLE_WRAP(102)
    X64_INTERRUPT_HANDLE_WRAP(103)
    X64_INTERRUPT_HANDLE_WRAP(104)
    X64_INTERRUPT_HANDLE_WRAP(105)
    X64_INTERRUPT_HANDLE_WRAP(106)
    X64_INTERRUPT_HANDLE_WRAP(107)
    X64_INTERRUPT_HANDLE_WRAP(108)
    X64_INTERRUPT_HANDLE_WRAP(109)
    X64_INTERRUPT_HANDLE_WRAP(110)
    X64_INTERRUPT_HANDLE_WRAP(111)
    X64_INTERRUPT_HANDLE_WRAP(112)
    X64_INTERRUPT_HANDLE_WRAP(113)
    X64_INTERRUPT_HANDLE_WRAP(114)
    X64_INTERRUPT_HANDLE_WRAP(115)
    X64_INTERRUPT_HANDLE_WRAP(116)
    X64_INTERRUPT_HANDLE_WRAP(117)
    X64_INTERRUPT_HANDLE_WRAP(118)
    X64_INTERRUPT_HANDLE_WRAP(119)
    X64_INTERRUPT_HANDLE_WRAP(120)
    X64_INTERRUPT_HANDLE_WRAP(121)
    X64_INTERRUPT_HANDLE_WRAP(122)
    X64_INTERRUPT_HANDLE_WRAP(123)
    X64_INTERRUPT_HANDLE_WRAP(124)
    X64_INTERRUPT_HANDLE_WRAP(125)
    X64_INTERRUPT_HANDLE_WRAP(126)
    X64_INTERRUPT_HANDLE_WRAP(127)
    X64_INTERRUPT_HANDLE_WRAP(128)
    X64_INTERRUPT_HANDLE_WRAP(129)
    X64_INTERRUPT_HANDLE_WRAP(130)
    X64_INTERRUPT_HANDLE_WRAP(131)
    X64_INTERRUPT_HANDLE_WRAP(132)
    X64_INTERRUPT_HANDLE_WRAP(133)
    X64_INTERRUPT_HANDLE_WRAP(134)
    X64_INTERRUPT_HANDLE_WRAP(135)
    X64_INTERRUPT_HANDLE_WRAP(136)
    X64_INTERRUPT_HANDLE_WRAP(137)
    X64_INTERRUPT_HANDLE_WRAP(138)
    X64_INTERRUPT_HANDLE_WRAP(139)
    X64_INTERRUPT_HANDLE_WRAP(140)
    X64_INTERRUPT_HANDLE_WRAP(141)
    X64_INTERRUPT_HANDLE_WRAP(142)
    X64_INTERRUPT_HANDLE_WRAP(143)
    X64_INTERRUPT_HANDLE_WRAP(144)
    X64_INTERRUPT_HANDLE_WRAP(145)
    X64_INTERRUPT_HANDLE_WRAP(146)
    X64_INTERRUPT_HANDLE_WRAP(147)
    X64_INTERRUPT_HANDLE_WRAP(148)
    X64_INTERRUPT_HANDLE_WRAP(149)
    X64_INTERRUPT_HANDLE_WRAP(150)
    X64_INTERRUPT_HANDLE_WRAP(151)
    X64_INTERRUPT_HANDLE_WRAP(152)
    X64_INTERRUPT_HANDLE_WRAP(153)
    X64_INTERRUPT_HANDLE_WRAP(154)
    X64_INTERRUPT_HANDLE_WRAP(155)
    X64_INTERRUPT_HANDLE_WRAP(156)
    X64_INTERRUPT_HANDLE_WRAP(157)
    X64_INTERRUPT_HANDLE_WRAP(158)
    X64_INTERRUPT_HANDLE_WRAP(159)
    X64_INTERRUPT_HANDLE_WRAP(160)
    X64_INTERRUPT_HANDLE_WRAP(161)
    X64_INTERRUPT_HANDLE_WRAP(162)
    X64_INTERRUPT_HANDLE_WRAP(163)
    X64_INTERRUPT_HANDLE_WRAP(164)
    X64_INTERRUPT_HANDLE_WRAP(165)
    X64_INTERRUPT_HANDLE_WRAP(166)
    X64_INTERRUPT_HANDLE_WRAP(167)
    X64_INTERRUPT_HANDLE_WRAP(168)
    X64_INTERRUPT_HANDLE_WRAP(169)
    X64_INTERRUPT_HANDLE_WRAP(170)
    X64_INTERRUPT_HANDLE_WRAP(171)
    X64_INTERRUPT_HANDLE_WRAP(172)
    X64_INTERRUPT_HANDLE_WRAP(173)
    X64_INTERRUPT_HANDLE_WRAP(174)
    X64_INTERRUPT_HANDLE_WRAP(175)
    X64_INTERRUPT_HANDLE_WRAP(176)
    X64_INTERRUPT_HANDLE_WRAP(177)
    X64_INTERRUPT_HANDLE_WRAP(178)
    X64_INTERRUPT_HANDLE_WRAP(179)
    X64_INTERRUPT_HANDLE_WRAP(180)
    X64_INTERRUPT_HANDLE_WRAP(181)
    X64_INTERRUPT_HANDLE_WRAP(182)
    X64_INTERRUPT_HANDLE_WRAP(183)
    X64_INTERRUPT_HANDLE_WRAP(184)
    X64_INTERRUPT_HANDLE_WRAP(185)
    X64_INTERRUPT_HANDLE_WRAP(186)
    X64_INTERRUPT_HANDLE_WRAP(187)
    X64_INTERRUPT_HANDLE_WRAP(188)
    X64_INTERRUPT_HANDLE_WRAP(189)
    X64_INTERRUPT_HANDLE_WRAP(190)
    X64_INTERRUPT_HANDLE_WRAP(191)
    X64_INTERRUPT_HANDLE_WRAP(192)
    X64_INTERRUPT_HANDLE_WRAP(193)
    X64_INTERRUPT_HANDLE_WRAP(194)
    X64_INTERRUPT_HANDLE_WRAP(195)
    X64_INTERRUPT_HANDLE_WRAP(196)
    X64_INTERRUPT_HANDLE_WRAP(197)
    X64_INTERRUPT_HANDLE_WRAP(198)
    X64_INTERRUPT_HANDLE_WRAP(199)
    X64_INTERRUPT_HANDLE_WRAP(200)
    X64_INTERRUPT_HANDLE_WRAP(201)
    X64_INTERRUPT_HANDLE_WRAP(202)
    X64_INTERRUPT_HANDLE_WRAP(203)
    X64_INTERRUPT_HANDLE_WRAP(204)
    X64_INTERRUPT_HANDLE_WRAP(205)
    X64_INTERRUPT_HANDLE_WRAP(206)
    X64_INTERRUPT_HANDLE_WRAP(207)
    X64_INTERRUPT_HANDLE_WRAP(208)
    X64_INTERRUPT_HANDLE_WRAP(209)
    X64_INTERRUPT_HANDLE_WRAP(210)
    X64_INTERRUPT_HANDLE_WRAP(211)
    X64_INTERRUPT_HANDLE_WRAP(212)
    X64_INTERRUPT_HANDLE_WRAP(213)
    X64_INTERRUPT_HANDLE_WRAP(214)
    X64_INTERRUPT_HANDLE_WRAP(215)
    X64_INTERRUPT_HANDLE_WRAP(216)
    X64_INTERRUPT_HANDLE_WRAP(217)
    X64_INTERRUPT_HANDLE_WRAP(218)
    X64_INTERRUPT_HANDLE_WRAP(219)
    X64_INTERRUPT_HANDLE_WRAP(220)
    X64_INTERRUPT_HANDLE_WRAP(221)
    X64_INTERRUPT_HANDLE_WRAP(222)
    X64_INTERRUPT_HANDLE_WRAP(223)
    X64_INTERRUPT_HANDLE_WRAP(224)
    X64_INTERRUPT_HANDLE_WRAP(225)
    X64_INTERRUPT_HANDLE_WRAP(226)
    X64_INTERRUPT_HANDLE_WRAP(227)
    X64_INTERRUPT_HANDLE_WRAP(228)
    X64_INTERRUPT_HANDLE_WRAP(229)
    X64_INTERRUPT_HANDLE_WRAP(230)
    X64_INTERRUPT_HANDLE_WRAP(231)
    X64_INTERRUPT_HANDLE_WRAP(232)
    X64_INTERRUPT_HANDLE_WRAP(233)
    X64_INTERRUPT_HANDLE_WRAP(234)
    X64_INTERRUPT_HANDLE_WRAP(235)
    X64_INTERRUPT_HANDLE_WRAP(236)
    X64_INTERRUPT_HANDLE_WRAP(237)
    X64_INTERRUPT_HANDLE_WRAP(238)
    X64_INTERRUPT_HANDLE_WRAP(239)
    X64_INTERRUPT_HANDLE_WRAP(240)
    X64_INTERRUPT_HANDLE_WRAP(241)
    X64_INTERRUPT_HANDLE_WRAP(242)
    X64_INTERRUPT_HANDLE_WRAP(243)
    X64_INTERRUPT_HANDLE_WRAP(244)
    X64_INTERRUPT_HANDLE_WRAP(245)
    X64_INTERRUPT_HANDLE_WRAP(246)
    X64_INTERRUPT_HANDLE_WRAP(247)
    X64_INTERRUPT_HANDLE_WRAP(248)
    X64_INTERRUPT_HANDLE_WRAP(249)
    X64_INTERRUPT_HANDLE_WRAP(250)
    X64_INTERRUPT_HANDLE_WRAP(251)
    X64_INTERRUPT_HANDLE_WRAP(252)
    X64_INTERRUPT_HANDLE_WRAP(253)
    X64_INTERRUPT_HANDLE_WRAP(254)
    X64_INTERRUPT_HANDLE_WRAP(255)

    .data
    .global g_interrupt_handler_table_addr
    .global g_interrupt_handler_table_bytes
_interrupt_table_end_:
g_interrupt_handler_table_bytes:
    .word    (_interrupt_table_end_  -  _interrupt_table_head_)




