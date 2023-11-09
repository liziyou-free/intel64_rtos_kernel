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
#define  ASM_FILE
#include "multiboot2.h"
#undef ASM_FILE
#include "../x64_cpu_drivers/x64_arch_def.h"
#include "../x64_cpu_drivers/x64_mmu.h"

/* Set stack size */
#define STACKSIZE    0x10000


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
  multiboot header
*******************************************************************************/
/*
 * An OS image must contain an additional header called Multiboot header, besid-
 * es the headers of the format used by the OS image. The Multiboot header must
 * be contained completely within the first 8192 bytes of the OS image, and must
 * be longword (32bit) aligned. In general, it should come as early as possible,
 * and may be embedded in the beginning of the text segment after the real exec-
 * utable header.
 */

    .global _start, stack_start,
    .section .multiboot_header,  "ax"

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
        .long 1024
        .long 768
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
start:
_start:
    jmp     multiboot_entry
    /* Init stack pointer */
    movl   $stack_end, %esp

    /* Clear EFLAGS register, disable makeable interrupt. */
    pushl  $0
    popf

        /* Save multiboot infomation */
    push    %ebx
    push    %eax
	call    cmain

    call    mmu_translate_table_init

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
X64_FUNC_END()



/* init 4-level page table */
    X64_CODE32_FUNC_BEGIN(mmu_translate_table_init)
mmu_translate_table_init:
    /* map: pte -> pde */
    movl    x64_pte_table,    %esi
    movl    x64_pde0_table,   %edi

    movl    $(X64_MMU_4LEVEL_PAGE_ENTRIES * 0x04),    %ecx
_pde_loop:
    movl    %esi,    %eax
    add     $(X64_PDE_PT_PRESENT_BIT|X64_PDE_PT_RW_BIT),    %eax
    movl    %eax,    0(%edi)
    add     $(X86_MMU_PAGE_ENTRY_SIZE),    %edi

    movl    $X64_MMU_4LEVEL_PAGE_ENTRIES,    %ebx
_pdt_loop:
    movl    $(X64_PT_4KB_PRESENT_BIT|X64_PT_4KB_RW_BIT),    %eax
    addl    $0x1000,    %edx    /* 4096bytes */
    addl    %edx,       %eax
    movl    %eax,       %esi
    dec     %ebx
    jne    _pdt_loop
    dec     %ebx
    jne    _pde_loop
X64_FUNC_END()





/*******************************************************************************
  ia-32e mode code (long mode)
*******************************************************************************/




/*******************************************************************************
  GDT IDT
*******************************************************************************/

/* X86 Global Descriptor Table Loader */
    .align     4
X64_ASM_DATA(g_gdt)
    .word    0  /* Limit = Total-bytes - 1 */
    .long    0  /* GDT base address */



/* X86 Interrupt Descriptor Table Loader */
    .align     4
X64_ASM_DATA(g_idt)
    .word    0    /* Limit = Total-bytes - 1 */
    .long    0    /* IDT base address */




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







