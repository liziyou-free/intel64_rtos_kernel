/*
 * x86_isr.S
 *
 *  Created on: 2023年9月11日
 *      Author: liziyou
 */



.global gp_system_stack, gp_task_stack, x86_common_isr

.global x86_exit_timer_isr_wrapper, x86_exit_timer_isr, flag, p_tcb0, p_tcb1


/* With error code */
#define  X86_EXCEPT_FUNCTION_WEC_DEL(func_name, inum) \
            .align  2; \
            .type func_name, %function; \
        func_name: \
            pushal; \
            movl   $gp_task_stack,  %eax; \
            movl   %esp,  (%eax); \
            movl   32(%esp), %eax; \
            movl   $gp_system_stack,  %esp; \
            pushl  %eax; \
            pushl  $(inum); \
            jmp  common_wec_isr; \
            .data; \
            .long  func_name; \
            .text



/* Without error code */
#define  X86_EXCEPT_FUNCTION_NEC_DEL(func_name, inum) \
            .align  2; \
            .type func_name, %function; \
        func_name: \
            pushal; \
            movl   $gp_task_stack,  %eax; \
            movl   %esp,  (%eax); \
            movl   $gp_system_stack,  %esp; \
            pushl  $0; \
            pushl  $(inum); \
            jmp  common_nec_isr; \
            .data; \
            .long  func_name; \
            .text


.section  .text
common_wec_isr:
        call  x86_common_isr
        movl   $gp_task_stack,  %eax
        movl   (%eax),  %esp
        popal
        add   $4,  %esp
        iret

common_nec_isr:
        call  x86_common_isr
        movl   $gp_task_stack,  %eax
        movl   (%eax),  %esp
        popal
        iret


  /* Except handle */

    .section  .data
    .align 4
    .type  x86_except_handle_array, %object
    .global x86_except_handle_array
x86_except_handle_array:
    .text

    X86_EXCEPT_FUNCTION_NEC_DEL(divide_error, 0)
    X86_EXCEPT_FUNCTION_NEC_DEL(debug_exceptions, 1)
    X86_EXCEPT_FUNCTION_NEC_DEL(nomaskable, 2)
    X86_EXCEPT_FUNCTION_NEC_DEL(breakpoint, 3)
    X86_EXCEPT_FUNCTION_NEC_DEL(overflow, 4)
    X86_EXCEPT_FUNCTION_NEC_DEL(bounds_check, 5)
    X86_EXCEPT_FUNCTION_NEC_DEL(invalid_opcode, 6)
    X86_EXCEPT_FUNCTION_NEC_DEL(coprocessor_not_available, 7)
    X86_EXCEPT_FUNCTION_WEC_DEL(double_fault, 8)
    X86_EXCEPT_FUNCTION_NEC_DEL(coprocessor_segment_overrun, 9)
    X86_EXCEPT_FUNCTION_WEC_DEL(invalid_tss, 10)
    X86_EXCEPT_FUNCTION_WEC_DEL(segment_not_present, 11)
    X86_EXCEPT_FUNCTION_WEC_DEL(stack_exception, 12)
    X86_EXCEPT_FUNCTION_WEC_DEL(general_protection, 13)
    X86_EXCEPT_FUNCTION_WEC_DEL(page_fault, 14)
    X86_EXCEPT_FUNCTION_NEC_DEL(coprecessor_error, 16)




/*******************************************************************************
  Extern interrupt
*******************************************************************************/
#define  X86_INTERRUPT_FUNCTION_WRAP(func_name, inum) \
            .align  2; \
            .global func_name; \
            .type func_name, %function; \
        func_name: \
            pushal; \
            movl   $gp_task_stack,  %eax; \
            movl   %esp,  (%eax); \
            movl   $gp_system_stack,  %esp; \
            pushl  $0; \
            pushl  $(inum); \
            jmp  x86_interrupt_wrap; \
            .data; \
            .long  func_name; \
            .text



/* Extern interrupt wrap */

    .section  .text
x86_interrupt_wrap:
    call  x86_common_isr

    /**
      * other opration (example: task switch)...
      */
        movl     flag,    %eax
        cmp      $0,      %eax
        je       b1
        jne      b2

    b1:
        movl     $p_tcb0,  %eax
        movl     %esp,    (%eax)
        jmp      b3
    b2:
        movl     $p_tcb1,  %eax
        movl     %esp,    (%eax)

    b3:
        movl    flag,    %eax
        test    %eax,    %eax
        sete    %al
        movzbl  %al,     %eax
        movl    %eax,    flag

    /* switch thread stack pointer */
    movl   $gp_task_stack,  %eax
    movl   (%eax),  %esp
    popal
    iret



/* Extern interrupt wrap handle */

    .data
    .align 4
    .type  x86_interrupt_handle_array, %object
    .global x86_interrupt_handle_array
x86_interrupt_handle_array:
    .text


X86_INTERRUPT_FUNCTION_WRAP(extern_interrupt_0, 32)
X86_INTERRUPT_FUNCTION_WRAP(extern_interrupt_1, 33)
X86_INTERRUPT_FUNCTION_WRAP(extern_interrupt_2, 34)
X86_INTERRUPT_FUNCTION_WRAP(extern_interrupt_3, 35)
X86_INTERRUPT_FUNCTION_WRAP(extern_interrupt_4, 36)
X86_INTERRUPT_FUNCTION_WRAP(extern_interrupt_5, 37)
X86_INTERRUPT_FUNCTION_WRAP(extern_interrupt_6, 38)
X86_INTERRUPT_FUNCTION_WRAP(extern_interrupt_7, 39)
X86_INTERRUPT_FUNCTION_WRAP(extern_interrupt_8, 40)
X86_INTERRUPT_FUNCTION_WRAP(extern_interrupt_9, 41)
X86_INTERRUPT_FUNCTION_WRAP(extern_interrupt_10, 42)
X86_INTERRUPT_FUNCTION_WRAP(extern_interrupt_11, 43)
X86_INTERRUPT_FUNCTION_WRAP(extern_interrupt_12, 44)
X86_INTERRUPT_FUNCTION_WRAP(extern_interrupt_13, 45)
X86_INTERRUPT_FUNCTION_WRAP(extern_interrupt_14, 46)
X86_INTERRUPT_FUNCTION_WRAP(extern_interrupt_15, 47)
X86_INTERRUPT_FUNCTION_WRAP(extern_interrupt_16, 48)


