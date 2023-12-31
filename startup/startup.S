/*
 *  QEMU: I386 MultiBoot startup.s
 *
 *  https://www.gnu.org/software/grub/manual/multiboot/multiboot.html
*/

/******************************************************************************/

#include "x86_startup.h"
#define  ASM_FILE
#include "multiboot.h"
#undef ASM_FILE

//#undef  __ELF__


/* X86 calling conventions: cdecl */


#define  MULTIBOOT_HEADER_FLAFS  (MULTIBOOT_PAGE_ALIGN | \
                                  MULTIBOOT_VIDEO_MODE | \
                                  MULTIBOOT_MEMORY_INFO)

/*
 * The field ‘checksum’ is a 32bit unsigned value which, when added to the other
 * magic fields (i.e. ‘magic’ and ‘flags’), must have a 32-bit unsigned sum of
 * zero.
 */
#define MULTIBOOT_CHECK_SUM   -(MULTIBOOT_HEADER_FLAFS + MULTIBOOT_HEADER_MAGIC)


/* Set stack size */
#define STACKSIZE    0x10000


.global cmain
.global main
.global stack_end
.global x86_apic_init
.global x86_pic_i8259a_init
.global x86_isa_serial_init
.global x86_serial_send
.global x86_timer_int
.global load_data_start
.global data_start
.global data_end
.global bss_start
.global bss_end
.global task_init
.global p_tcb0


/*
 * An OS image must contain an additional header called Multiboot header, besid-
 * es the headers of the format used by the OS image. The Multiboot header must
 * be contained completely within the first 8192 bytes of the OS image, and must
 * be longword (32bit) aligned. In general, it should come as early as possible,
 * and may be embedded in the beginning of the text segment after the real exec-
 * utable header.
 */
    .section  .multiboot_header, "ax"
    .global _start, stack_start,
_start:
    /* Force the cpu to clear the pipeline */
    jmp  multiboot_entry
    .align 4
multiboot_header:
    .long  MULTIBOOT_HEADER_MAGIC
    .long  MULTIBOOT_HEADER_FLAFS
    .long  MULTIBOOT_CHECK_SUM
#ifndef __ELF__
        /* header_addr */
        .long   multiboot_header
        /* load_addr */
        .long   _start
        /* load_end_addr */
        .long   _edata
        /* bss_end_addr */
        .long   _end
        /* entry_addr */
        .long   multiboot_entry
#else /* ! __ELF__ */
        .long   0
        .long   0
        .long   0
        .long   0
        .long   0
#endif /* __ELF__ */
        .long 0
        .long 1024
        .long 768
        .long 32

    .section .text
    .code32
    .type multiboot_entry, %function
    .global x86_kerner_memory_space_init, x86_interrupt_except_init, stack_end
multiboot_entry:
    /* Init stack pointer */
    movl   $stack_end, %esp
    /* Save multiboot infomation */
    push  %ebx
    push  %eax

    /* Clear EFLAGS register, disable makeable intrruupt. */
    push  $0
    popf


    /* BSS fill zero */
    movl  $bss_start,  %eax
    movl  $bss_end,    %ebx
clean_bss_section:
    cmp   %eax,  %ebx
    je    copy_data_section
    movb  $0,  (%eax)
    add   $1,  %eax
    jne   clean_bss_section


    /* Init data section */
copy_data_section:
    movl  $data_start,       %edi
    movl  $data_end,         %ecx
    sub   $data_start,       %ecx
    movl  $load_data_start,  %esi
    cld   /* incremented! */
    rep   movsb


    /* X86 IDT init */
//    call  x86_interrupt_except_init

    /* X86 GDT init */
//    call  x86_kerner_memory_space_init

    /* X86 PIC i8259a init */
/*    mov   %esp, %ebp
    push  $1
    call  x86_pic_i8259a_init

    call  x86_apic_init
*/
    /* Enable global interrupt */
//    sti

    /* Serial Init */
//   call  x86_isa_serial_init

        /* Serial test */
//    push  $'a'
//    call  x86_serial_send

    /* Timer Init (tick 1000 / s) */
 //   call  x86_timer_int

    /* Task test */
/*    call  task_init
    movl  p_tcb0,  %esp
    movl  $0,  %eax
    movl  $0,  %ebx
    popa
    iret

    mov   %ebp, %esp
next:
    call  cmain
    call  main
    int   $0
loop:
    hlt
    jmp  loop
*/

    .data
    .align  4
    .global  stack_s,stack_e
stack_s:
    .rept  STACKSIZE
    .byte  0
    .endr
stack_e:


/***************************************************************************
  X86 Global Descriptor Table Loader
***************************************************************************/
    .data
    .align  4
    .global  _g_gdt, g_x86_gdt
_g_gdt:
    .word  0    /* Limit = Total-bytes - 1 */
    .long  0    /* GDT base address */


/***************************************************************************
  X86 Interrupt Descriptor Table Loader
***************************************************************************/
    .data
    .align  4
    .global  _g_idt, g_x86_idt
_g_idt:
    .word  0    /* Limit = Total-bytes - 1 */
    .long  0    /* IDT base address */


