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
#include "x64_startup.h"
#include "x64_asm_common.h"
#define  COMPILER_ASM_FILE
#include "multiboot2.h"
#include "../x64_cpu_drivers/x64_segment_def.h"
#undef COMPILER_ASM_FILE
#include "../x64_cpu_drivers/x64_mmu.h"
#include "../startup/x64_arch_def.h"


/* Set stack size */
#define STACKSIZE    (1024*256)


.global main
.global stack_end
.global load_data_start
.global data_start
.global data_end
.global bss_start
.global bss_end




/*  C symbol format. HAVE_ASM_USCORE is defined by configure. */
#ifdef HAVE_ASM_USCORE
# define EXT_C(sym)                     _ ## sym
#else
# define EXT_C(sym)                     sym
#endif

/*  The size of our stack (16KB). */
#define STACK_SIZE                      0x4000

/*  The flags for the Multiboot header. */
#ifdef __ELF__
# define AOUT_KLUDGE 0
#else
# define AOUT_KLUDGE MULTIBOOT_AOUT_KLUDGE
#endif

/*******************************************************************************
  multiboot2 header
*******************************************************************************/
/*
 * An OS image must contain an additional header called Multiboot header, besid-
 * es the headers of the format used by the OS image. The Multiboot header must
 * be contained completely within the first 32768 bytes of the OS image, and must
 * be longword (32bit) aligned. In general, it should come as early as possible,
 * and may be embedded in the beginning of the text segment after the real exec-
 * utable header.
 */

    .global _start, stack_start,
    .section .multiboot_header,  "ax"

start:
_start:
    jmp     multiboot_entry

    /*  Align 64 bits boundary. */
    .align MULTIBOOT_TAG_ALIGN

    /*  Multiboot header. */
multiboot_header:
    /*  magic */
    .long   MULTIBOOT2_HEADER_MAGIC
    /*  ISA: i386 */
    .long   MULTIBOOT_ARCHITECTURE_I386
    /*  Header length. */
    .long   multiboot_header_end - multiboot_header
    /*  checksum */
    .long   -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + (multiboot_header_end - multiboot_header))
#ifndef __ELF__
address_tag_start:
    .align MULTIBOOT_TAG_ALIGN
    .short MULTIBOOT_HEADER_TAG_ADDRESS
    .align MULTIBOOT_TAG_ALIGN
    .short MULTIBOOT_HEADER_TAG_OPTIONAL
    .long address_tag_end - address_tag_start
    /*  header_addr */
    .long   multiboot_header
    /*  load_addr */
    .long   _start
    /*  load_end_addr */
    .long   _edata
    /*  bss_end_addr */
    .long   _end
address_tag_end:
entry_address_tag_start:
    .align MULTIBOOT_TAG_ALIGN
    .short MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS
    .align MULTIBOOT_TAG_ALIGN
    .short MULTIBOOT_HEADER_TAG_OPTIONAL
    .long entry_address_tag_end - entry_address_tag_start
    /*  entry_addr */
    .long multiboot_entry
entry_address_tag_end:
#endif /*  __ELF__ */
framebuffer_tag_start:
    .align MULTIBOOT_TAG_ALIGN
    .short MULTIBOOT_HEADER_TAG_FRAMEBUFFER
    .align MULTIBOOT_TAG_ALIGN
    .short MULTIBOOT_HEADER_TAG_OPTIONAL
    .align MULTIBOOT_TAG_ALIGN
    .long framebuffer_tag_end - framebuffer_tag_start
    .long 480
    .long 480
    .long 32
framebuffer_tag_end:
    .short MULTIBOOT_HEADER_TAG_END
    .short 0
    .long 8
multiboot_header_end:


/*******************************************************************************
  32bit protect mode code
*******************************************************************************/
    X64_CODE32_FUNC_BEGIN(multiboot_entry)

    /* Init stack pointer, 4byte align */
    movl    $stack_end,    %esp
    andl    $(~(4-1)),     %esp

    /* Clear EFLAGS register, disable makeable interrupt. */
    pushl  $0
    popf

    /* Save multiboot infomation */
    //push
    pushl    $0
    pushl    %ebx
    pushl    $0
    pushl    %eax


    /* BSS fill zero */
    movl  $bss_start,  %eax
    movl  $bss_end,    %ebx
clean_bss_section:
    cmp     %eax,    %ebx
    je      copy_data_section
    movb    $0,      (%eax)
    add     $1,      %eax
    jne     clean_bss_section

    /* Init data section */
copy_data_section:
    movl    $data_start,         %edi
    movl    $data_end,           %ecx
    sub     $data_start,         %ecx
    movl    $load_data_start,    %esi
    cld      /* incremented! */
    rep      movsb

    /* Init MMU translate table */
    call    mmu_translate_table_init

    /* PAE  must be set before entering IA-32e mode */
    movl    %cr4,    %eax
    or      $(X64_CR4_PAE_BIT | X64_CR4_PGE_BIT),    %eax
    movl    %eax,    %cr4

    /* Write MMU translate-table-base-address to CR3 */
    movl    $x64_pml4e_table,    %eax
    movl    %eax,    %cr3

    /* Enable IA32_EFER_MSR.LME */
    movl    $IA32_EFER_MSR,    %ecx
    rdmsr
    or      $IA32_EFER_MSR_LME,    %eax
    wrmsr

    /* Enabe CR0.PG */
    movl    %cr0,    %eax
    or      $(X64_CR0_PE_BIT | X64_CR0_PG_BIT),    %eax
    movl    %eax,    %cr0

    lgdt    g_x64_gdtr_loader
    ljmp    $X64_GET_SEGMENT_SELECTOR_VALUE(X64_GDT_CODE64_INDEX, \
                                            PRIVILEGE_LEVEL_0, \
                                            TABLE_INDICATOR_GDT), \
                                            $x64_start
X64_FUNC_END()


/*
 * init 4-level page table
 */
    X64_CODE32_FUNC_BEGIN(mmu_translate_table_init)
mmu_translate_table_init:
    /* map: pte -> pde */
    movl    $x64_pte_table,    %esi
    movl    $x64_pde0_table,   %edi
    movl    $0,                %edx

    movl    $(X64_MMU_4LEVEL_PAGE_ENTRIES * 0x04),    %ecx
_pde_loop:
    movl    %esi,    %eax
    add     $(X64_PDE_PT_PRESENT_BIT|X64_PDE_PT_RW_BIT),    %eax
    movl    %eax,    0(%edi)
    add     $(X86_MMU_PAGE_ENTRY_SIZE),    %edi

    movl    $X64_MMU_4LEVEL_PAGE_ENTRIES,    %ebx
_pdt_loop:
    movl    $(X64_PT_4KB_PRESENT_BIT|X64_PT_4KB_RW_BIT),    %eax
    addl    %edx,       %eax
    movl    %eax,       0(%esi)
    addl    $0x1000,    %edx    /* 4096bytes */
    addl    $8,         %esi
    dec     %ebx
    jnz    _pdt_loop
    dec     %ecx
    jnz    _pde_loop
    ret
    X64_FUNC_END()



/*******************************************************************************
  64bit mode code
*******************************************************************************/
    .global x64_tss_init
    .global x64_idt_init
    .global x64_arch_init
    .global divided_zero, main
    X64_CODE64_FUNC_BEGIN(x64_start)
    cli

    lgdt    g_x64_gdtr_loader

    // Set Segement Registers for proper iret, etc. operation
    mov    $X64_GET_SEGMENT_SELECTOR_VALUE(
                            X64_GDT_DATA_INDEX, \
                            PRIVILEGE_LEVEL_0, \
                            TABLE_INDICATOR_GDT), \
                            %ax
    mov     %ax,    %ss
    mov     %ax,    %ds
    mov     %ax,    %es
    mov     %ax,    %fs
    mov     %ax,    %gs

    pop     %rdi
    pop     %rsi
    call    cmain
    call    x64_arch_init
    call    x64_idt_init
    call    x64_tss_init
    sti
    call    main
    int     $36
    call    divided_zero

_loop_:
    nop
    jmp    _loop_

    X64_FUNC_END()





/*******************************************************************************
  GDT and IDT
*******************************************************************************/

/* X86 Global Descriptor Table */
    .align     8
    .global    g_gdt_tss_pos, g_x64_tss_obj_test
    X64_ASM_DATA(g_x64_gdt)
_gdt_start:

    .quad    0    /* Must is 0! */
    .quad    X64_CREATE_GDT_ITEM_UNIT_4KB(X64_FLAT_SEGMENT_BASE, X64_FLAT_SEGMENT_LIMIT, APP_CODE32_XRC_SEGMENT, PRIVILEGE_LEVEL_0)
    .quad    X64_CREATE_GDT_ITEM_UNIT_4KB(X64_FLAT_SEGMENT_BASE, X64_FLAT_SEGMENT_LIMIT, APP_DATA_RW_SEGMENT, PRIVILEGE_LEVEL_0)
    .quad    X64_CREATE_GDT_ITEM_UNIT_4KB(X64_FLAT_SEGMENT_BASE, X64_FLAT_SEGMENT_LIMIT, APP_CODE64_X_SEGMENT, PRIVILEGE_LEVEL_0)
g_gdt_tss_pos:
    .quad    0, 0    /* Reserve position, initialize TSS in C code */
_gdt_end:


/* X86 Global Descriptor Table Loader */
    .align     4
    X64_ASM_DATA(g_x64_gdtr_loader)
    .word    _gdt_end - _gdt_start -1  /* Limit = Total-bytes - 1 */
    .quad    _gdt_start                /* GDT base address */



/*******************************************************************************
  build 4-level paging (MAX MAP:4GB)
*******************************************************************************/
	.align  4096
X64_ASM_DATA(x64_pml4e_table)
    .quad    (x64_pdpte_table + X64_PML4E_PRESENT_BIT + X64_PML4E_RW_BIT)
	/* Reserve space, but do not use it for the time being */
	.fill    (512-1),  X86_MMU_PAGE_ENTRY_SIZE,  0

	.align  4096
X64_ASM_DATA(x64_pdpte_table)
    .quad    (x64_pde0_table + X64_PDPTE_PDE_PRESENT_BIT + X64_PDPTE_PDE_RW_BIT)
    .quad    (x64_pde1_table + X64_PDPTE_PDE_PRESENT_BIT + X64_PDPTE_PDE_RW_BIT)
    .quad    (x64_pde2_table + X64_PDPTE_PDE_PRESENT_BIT + X64_PDPTE_PDE_RW_BIT)
    .quad    (x64_pde3_table + X64_PDPTE_PDE_PRESENT_BIT + X64_PDPTE_PDE_RW_BIT)
    /* Reserve space, but do not use it for the time being */
    .fill    (X64_MMU_4LEVEL_PAGE_ENTRIES - 4), X86_MMU_PAGE_ENTRY_SIZE, 0

	.align  4096
X64_ASM_DATA(x64_pde0_table)
    .fill    X64_MMU_4LEVEL_PAGE_ENTRIES,  X86_MMU_PAGE_ENTRY_SIZE,  0


	.align  4096
X64_ASM_DATA(x64_pde1_table)
    .fill    X64_MMU_4LEVEL_PAGE_ENTRIES,  X86_MMU_PAGE_ENTRY_SIZE,  0

	.align  4096
X64_ASM_DATA(x64_pde2_table)
    .fill    X64_MMU_4LEVEL_PAGE_ENTRIES,  X86_MMU_PAGE_ENTRY_SIZE,  0


	.align  4096
X64_ASM_DATA(x64_pde3_table)
    .fill    X64_MMU_4LEVEL_PAGE_ENTRIES,  X86_MMU_PAGE_ENTRY_SIZE,  0


	.align  4096
X64_ASM_DATA(x64_pte_table)
    .fill    (X64_MMU_4LEVEL_PAGE_ENTRIES * X64_MMU_4LEVEL_PAGE_ENTRIES),  X86_MMU_PAGE_ENTRY_SIZE,  0
    .fill    (X64_MMU_4LEVEL_PAGE_ENTRIES * X64_MMU_4LEVEL_PAGE_ENTRIES),  X86_MMU_PAGE_ENTRY_SIZE,  0
    .fill    (X64_MMU_4LEVEL_PAGE_ENTRIES * X64_MMU_4LEVEL_PAGE_ENTRIES),  X86_MMU_PAGE_ENTRY_SIZE,  0
    .fill    (X64_MMU_4LEVEL_PAGE_ENTRIES * X64_MMU_4LEVEL_PAGE_ENTRIES),  X86_MMU_PAGE_ENTRY_SIZE,  0



/*******************************************************************************
  stack
*******************************************************************************/
    .section  .kernel.stack
    .align  64
    .global  g_kernel_stack
g_kernel_stack:
    .fill    STACKSIZE,    1,    0


