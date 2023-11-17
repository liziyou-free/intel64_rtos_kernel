# 0 "./startup/x64_boot.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "./startup/x64_boot.S"
# 19 "./startup/x64_boot.S"
# 1 "./startup/x64_startup.h" 1
# 20 "./startup/x64_boot.S" 2
# 1 "./startup/x64_asm_common.h" 1
# 21 "./startup/x64_boot.S" 2

# 1 "./startup/multiboot2.h" 1
# 23 "./startup/x64_boot.S" 2
# 1 "./startup/../x64_cpu_drivers/x64_segment_def.h" 1
# 24 "./startup/x64_boot.S" 2

# 1 "./startup/../x64_cpu_drivers/x64_mmu.h" 1
# 26 "./startup/x64_boot.S" 2
# 1 "./startup/../x64_cpu_drivers/x64_arch_def.h" 1
# 27 "./startup/x64_boot.S" 2






.global main
.global stack_end
.global load_data_start
.global data_start
.global data_end
.global bss_start
.global bss_end
# 73 "./startup/x64_boot.S"
    .global _start, stack_start,
    .section .multiboot_header, "ax"

start:
_start:
    jmp multiboot_entry


    .align 8


multiboot_header:

    .long 0xe85250d6

    .long 0

    .long multiboot_header_end - multiboot_header

    .long -(0xe85250d6 + 0 + (multiboot_header_end - multiboot_header))
# 119 "./startup/x64_boot.S"
framebuffer_tag_start:
    .align 8
    .short 5
    .align 8
    .short 1
    .align 8
    .long framebuffer_tag_end - framebuffer_tag_start
    .long 480
    .long 480
    .long 32
framebuffer_tag_end:
    .short 0
    .short 0
    .long 8
multiboot_header_end:





    .global multiboot_entry ; .section .text.multiboot_entry; .type multiboot_entry, %function; .func; .code32; multiboot_entry:


    movl $stack_end, %esp
    andl $(~(4-1)), %esp


    pushl $0
    popf



    pushl $0
    pushl %ebx
    pushl $0
    pushl %eax



    movl $bss_start, %eax
    movl $bss_end, %ebx
clean_bss_section:
    cmp %eax, %ebx
    je copy_data_section
    movb $0, (%eax)
    add $1, %eax
    jne clean_bss_section


copy_data_section:
    movl $data_start, %edi
    movl $data_end, %ecx
    sub $data_start, %ecx
    movl $load_data_start, %esi
    cld
    rep movsb


    call mmu_translate_table_init


    movl %cr4, %eax
    or $((1U << 5) | (1U << 7)), %eax
    movl %eax, %cr4


    movl $x64_pml4e_table, %eax
    movl %eax, %cr3


    movl $0xC0000080U, %ecx
    rdmsr
    or $(1u << 8), %eax
    wrmsr


    movl %cr0, %eax
    or $((1u << 0) | (1u << 31)), %eax
    movl %eax, %cr0

    lgdt g_x64_gdtr_loader
    ljmp $((0x03 << 3) | (0 << 2) | 0), $x64_start



.endfunc;.text





    .global mmu_translate_table_init ; .section .text.mmu_translate_table_init; .type mmu_translate_table_init, %function; .func; .code32; mmu_translate_table_init:
mmu_translate_table_init:

    movl $x64_pte_table, %esi
    movl $x64_pde0_table, %edi
    movl $0, %edx

    movl $(512 * 0x04), %ecx
_pde_loop:
    movl %esi, %eax
    add $((1 << 0)|(1 << 1)), %eax
    movl %eax, 0(%edi)
    add $(8), %edi

    movl $512, %ebx
_pdt_loop:
    movl $((1 << 0)|(1 << 1)), %eax
    addl %edx, %eax
    movl %eax, 0(%esi)
    addl $0x1000, %edx
    addl $8, %esi
    dec %ebx
    jnz _pdt_loop
    dec %ecx
    jnz _pde_loop
    ret
    .endfunc;.text






    .global x64_tss_init
    .global x64_idt_init
    .global divided_zero
    .global x64_start ; .section .text.x64_start; .type x64_start, %function; .func; .code64; x64_start:
    cli

    mov $x64_pml4e_table, %rax
    mov %rax, %cr3

    movl $0xC0000080U, %ecx
    rdmsr

    lgdt g_x64_gdtr_loader


    mov $((0x02 << 3) | (0 << 2) | 0), %ax



    mov %ax, %ss
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    pop %rdi
    pop %rsi
    call cmain
    call x64_idt_init
    call x64_tss_init
    int $36
    call divided_zero

_loop_:
    nop
    jmp _loop_

    .endfunc;.text
# 291 "./startup/x64_boot.S"
    .align 8
    .global g_gdt_tss_pos
    .global g_x64_gdt; .section .data.g_x64_gdt; .type g_x64_gdt, %object; g_x64_gdt:
_gdt_start:
    .quad 0
    .quad ( ((0xFFFFF & 0x0000ffff) << 0) | ((0x00000 & 0x0000ffff) << 16) | (((0x00000 >> 16) & 0x00ff) << 32) | (0x1e << 40) | (0 << 45) | (1 << 47) | (((0xFFFFF >> 16) & 0x0f) << 48) | (1 << 54) | (0 << 55) | ((0x00000 >> 24) << 56) )
    .quad ( ((0xFFFFF & 0x0000ffff) << 0) | ((0x00000 & 0x0000ffff) << 16) | (((0x00000 >> 16) & 0x00ff) << 32) | (0x12 << 40) | (0 << 45) | (1 << 47) | (((0xFFFFF >> 16) & 0x0f) << 48) | (1 << 54) | (0 << 55) | ((0x00000 >> 24) << 56) )
    .quad ((0x2018 << 40 | 0 << 45 | 1 << 47 | 1 << 53) | (0 << 54 | 1 << 55))
g_gdt_tss_pos:
    .quad 0, 0
_gdt_end:



    .align 4
    .global g_x64_gdtr_loader; .section .data.g_x64_gdtr_loader; .type g_x64_gdtr_loader, %object; g_x64_gdtr_loader:
    .word _gdt_end - _gdt_start -1
    .quad _gdt_start



    .align 4
.global g_idtr; .section .data.g_idtr; .type g_idtr, %object; g_idtr:
    .word 0
    .long 0







 .align 4096
.global x64_pml4e_table; .section .data.x64_pml4e_table; .type x64_pml4e_table, %object; x64_pml4e_table:
    .quad (x64_pdpte_table + (1 << 0) + (1 << 1))

 .fill (512-1), 8, 0

 .align 4096
.global x64_pdpte_table; .section .data.x64_pdpte_table; .type x64_pdpte_table, %object; x64_pdpte_table:
    .quad (x64_pde0_table + (1 << 0) + (1 << 1))
    .quad (x64_pde1_table + (1 << 0) + (1 << 1))
    .quad (x64_pde2_table + (1 << 0) + (1 << 1))
    .quad (x64_pde3_table + (1 << 0) + (1 << 1))

    .fill (512 - 4), 8, 0

 .align 4096
.global x64_pde0_table; .section .data.x64_pde0_table; .type x64_pde0_table, %object; x64_pde0_table:
    .fill 512, 8, 0


 .align 4096
.global x64_pde1_table; .section .data.x64_pde1_table; .type x64_pde1_table, %object; x64_pde1_table:
    .fill 512, 8, 0

 .align 4096
.global x64_pde2_table; .section .data.x64_pde2_table; .type x64_pde2_table, %object; x64_pde2_table:
    .fill 512, 8, 0


 .align 4096
.global x64_pde3_table; .section .data.x64_pde3_table; .type x64_pde3_table, %object; x64_pde3_table:
    .fill 512, 8, 0


 .align 4096
.global x64_pte_table; .section .data.x64_pte_table; .type x64_pte_table, %object; x64_pte_table:
    .fill (512 * 512), 8, 0
    .fill (512 * 512), 8, 0
    .fill (512 * 512), 8, 0
    .fill (512 * 512), 8, 0






    .section .kernel.stack
    .align 64
    .global g_kernel_stack
g_kernel_stack:
    .fill 0x10000, 1, 0
