# 0 "./startup/x64_isr.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "./startup/x64_isr.S"
# 43 "./startup/x64_isr.S"
# 1 "./startup/x64_arch_def.h" 1
# 44 "./startup/x64_isr.S" 2
# 90 "./startup/x64_isr.S"
    .global x64_common_isr
    .code64
_isr_handler_:
    pushq %rsi
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15







    leaq -(8 + 512)(%rsp), %rsp
    fxsaveq (%rsp)


    call x64_common_isr

    fxrstorq (%rsp)
    leaq (8 + 512)(%rsp), %rsp
    popq %r15
 popq %r14
 popq %r13
 popq %r12
 popq %r11
 popq %r10
 popq %r9
 popq %r8
 popq %rdx
 popq %rcx
 popq %rbx
 popq %rax
    popq %rsi
    popq %rdi

    iretq


    .data
    .align 8
    .type g_exception_handler_table_addr, %object
_exception_table_head_:
g_exception_handler_table_addr:
 .global divide_error_exception_handler; .type divide_error_exception_handler, %function; .code64; divide_error_exception_handler: pushq $0; pushq %rdi; mov $0, %rdi; jmp _isr_handler_; .data; .quad divide_error_exception_handler; .text
 .global debug_exception_handler; .type debug_exception_handler, %function; .code64; debug_exception_handler: pushq $0; pushq %rdi; mov $1, %rdi; jmp _isr_handler_; .data; .quad debug_exception_handler; .text
 .global nmi_exception_handler; .type nmi_exception_handler, %function; .code64; nmi_exception_handler: pushq $0; pushq %rdi; mov $2, %rdi; jmp _isr_handler_; .data; .quad nmi_exception_handler; .text
 .global breakpoint_exception_handler; .type breakpoint_exception_handler, %function; .code64; breakpoint_exception_handler: pushq $0; pushq %rdi; mov $3, %rdi; jmp _isr_handler_; .data; .quad breakpoint_exception_handler; .text
 .global overflow_exception_handler; .type overflow_exception_handler, %function; .code64; overflow_exception_handler: pushq $0; pushq %rdi; mov $4, %rdi; jmp _isr_handler_; .data; .quad overflow_exception_handler; .text
 .global bound_range_exception_handler; .type bound_range_exception_handler, %function; .code64; bound_range_exception_handler: pushq $0; pushq %rdi; mov $5, %rdi; jmp _isr_handler_; .data; .quad bound_range_exception_handler; .text
 .global invalid_opcode_exception_handler; .type invalid_opcode_exception_handler, %function; .code64; invalid_opcode_exception_handler: pushq $0; pushq %rdi; mov $6, %rdi; jmp _isr_handler_; .data; .quad invalid_opcode_exception_handler; .text
 .global device_not_available_exception_handler; .type device_not_available_exception_handler, %function; .code64; device_not_available_exception_handler: pushq $0; pushq %rdi; mov $7, %rdi; jmp _isr_handler_; .data; .quad device_not_available_exception_handler; .text
 .global double_fault_exception_handler; .type double_fault_exception_handler, %function; .code64; double_fault_exception_handler: push %rdi; mov $8, %rdi; jmp _isr_handler_; .data; .quad double_fault_exception_handler; .text
 .global coprocessor_segment_overrun_exception_handler; .type coprocessor_segment_overrun_exception_handler, %function; .code64; coprocessor_segment_overrun_exception_handler: pushq $0; pushq %rdi; mov $9, %rdi; jmp _isr_handler_; .data; .quad coprocessor_segment_overrun_exception_handler; .text
 .global invalid_tss_exception_handler; .type invalid_tss_exception_handler, %function; .code64; invalid_tss_exception_handler: push %rdi; mov $10, %rdi; jmp _isr_handler_; .data; .quad invalid_tss_exception_handler; .text
 .global segment_not_present_exception_handler; .type segment_not_present_exception_handler, %function; .code64; segment_not_present_exception_handler: push %rdi; mov $11, %rdi; jmp _isr_handler_; .data; .quad segment_not_present_exception_handler; .text
 .global stack_fault_exception_handler; .type stack_fault_exception_handler, %function; .code64; stack_fault_exception_handler: push %rdi; mov $12, %rdi; jmp _isr_handler_; .data; .quad stack_fault_exception_handler; .text
 .global general_protection_exception_handler; .type general_protection_exception_handler, %function; .code64; general_protection_exception_handler: push %rdi; mov $13, %rdi; jmp _isr_handler_; .data; .quad general_protection_exception_handler; .text
 .global page_fault_exception_handler; .type page_fault_exception_handler, %function; .code64; page_fault_exception_handler: push %rdi; mov $14, %rdi; jmp _isr_handler_; .data; .quad page_fault_exception_handler; .text
 .global reserved_exception_handler; .type reserved_exception_handler, %function; .code64; reserved_exception_handler: pushq $0; pushq %rdi; mov $15, %rdi; jmp _isr_handler_; .data; .quad reserved_exception_handler; .text
 .global fpu_fp_error_exception_handler; .type fpu_fp_error_exception_handler, %function; .code64; fpu_fp_error_exception_handler: pushq $0; pushq %rdi; mov $16, %rdi; jmp _isr_handler_; .data; .quad fpu_fp_error_exception_handler; .text
 .global aligment_check_exception_handler; .type aligment_check_exception_handler, %function; .code64; aligment_check_exception_handler: push %rdi; mov $17, %rdi; jmp _isr_handler_; .data; .quad aligment_check_exception_handler; .text
 .global machine_cahck_exception_handler; .type machine_cahck_exception_handler, %function; .code64; machine_cahck_exception_handler: pushq $0; pushq %rdi; mov $18, %rdi; jmp _isr_handler_; .data; .quad machine_cahck_exception_handler; .text
 .global simd_fp_exception_handler; .type simd_fp_exception_handler, %function; .code64; simd_fp_exception_handler: pushq $0; pushq %rdi; mov $19, %rdi; jmp _isr_handler_; .data; .quad simd_fp_exception_handler; .text
 .global virt_exception_exception_handler; .type virt_exception_exception_handler, %function; .code64; virt_exception_exception_handler: push %rdi; mov $20, %rdi; jmp _isr_handler_; .data; .quad virt_exception_exception_handler; .text


    .data
    .global g_exception_handler_table_addr
    .global g_exception_handler_table_bytes
_exception_table_end_:
g_exception_handler_table_bytes:
    .word (_exception_table_end_ - _exception_table_head_)


    .data
    .align 8
    .type g_interrupt_handler_table, %object
_interrupt_table_head_:
g_interrupt_handler_table_addr:
 .global exit0_handler; .type exit0_handler, %function; .code64; exit0_handler: pushq $0; pushq %rdi; mov $0, %rdi; jmp _isr_handler_; .data; .quad exit0_handler; .text
 .global exit1_handler; .type exit1_handler, %function; .code64; exit1_handler: pushq $0; pushq %rdi; mov $1, %rdi; jmp _isr_handler_; .data; .quad exit1_handler; .text
 .global exit2_handler; .type exit2_handler, %function; .code64; exit2_handler: pushq $0; pushq %rdi; mov $2, %rdi; jmp _isr_handler_; .data; .quad exit2_handler; .text
 .global exit3_handler; .type exit3_handler, %function; .code64; exit3_handler: pushq $0; pushq %rdi; mov $3, %rdi; jmp _isr_handler_; .data; .quad exit3_handler; .text
 .global exit4_handler; .type exit4_handler, %function; .code64; exit4_handler: pushq $0; pushq %rdi; mov $4, %rdi; jmp _isr_handler_; .data; .quad exit4_handler; .text
 .global exit5_handler; .type exit5_handler, %function; .code64; exit5_handler: pushq $0; pushq %rdi; mov $5, %rdi; jmp _isr_handler_; .data; .quad exit5_handler; .text
 .global exit6_handler; .type exit6_handler, %function; .code64; exit6_handler: pushq $0; pushq %rdi; mov $6, %rdi; jmp _isr_handler_; .data; .quad exit6_handler; .text
 .global exit7_handler; .type exit7_handler, %function; .code64; exit7_handler: pushq $0; pushq %rdi; mov $7, %rdi; jmp _isr_handler_; .data; .quad exit7_handler; .text
 .global exit8_handler; .type exit8_handler, %function; .code64; exit8_handler: pushq $0; pushq %rdi; mov $8, %rdi; jmp _isr_handler_; .data; .quad exit8_handler; .text
 .global exit9_handler; .type exit9_handler, %function; .code64; exit9_handler: pushq $0; pushq %rdi; mov $9, %rdi; jmp _isr_handler_; .data; .quad exit9_handler; .text
 .global exit10_handler; .type exit10_handler, %function; .code64; exit10_handler: pushq $0; pushq %rdi; mov $10, %rdi; jmp _isr_handler_; .data; .quad exit10_handler; .text
 .global exit11_handler; .type exit11_handler, %function; .code64; exit11_handler: pushq $0; pushq %rdi; mov $11, %rdi; jmp _isr_handler_; .data; .quad exit11_handler; .text
 .global exit12_handler; .type exit12_handler, %function; .code64; exit12_handler: pushq $0; pushq %rdi; mov $12, %rdi; jmp _isr_handler_; .data; .quad exit12_handler; .text
 .global exit13_handler; .type exit13_handler, %function; .code64; exit13_handler: pushq $0; pushq %rdi; mov $13, %rdi; jmp _isr_handler_; .data; .quad exit13_handler; .text
 .global exit14_handler; .type exit14_handler, %function; .code64; exit14_handler: pushq $0; pushq %rdi; mov $14, %rdi; jmp _isr_handler_; .data; .quad exit14_handler; .text
 .global exit15_handler; .type exit15_handler, %function; .code64; exit15_handler: pushq $0; pushq %rdi; mov $15, %rdi; jmp _isr_handler_; .data; .quad exit15_handler; .text
 .global exit16_handler; .type exit16_handler, %function; .code64; exit16_handler: pushq $0; pushq %rdi; mov $16, %rdi; jmp _isr_handler_; .data; .quad exit16_handler; .text

 .data
    .global g_interrupt_handler_table_addr
    .global g_interrupt_handler_table_bytes
_interrupt_table_end_:
g_interrupt_handler_table_bytes:
    .word (_interrupt_table_end_ - _interrupt_table_head_)
