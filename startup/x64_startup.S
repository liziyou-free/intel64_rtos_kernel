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
#include "multiboot.h"
#undef ASM_FILE
#include "../x64_cpu_drivers/x64_arch_def.h"
#include "../x64_cpu_drivers/x64_mmu.h"

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


.global main
.global stack_end
.global load_data_start
.global data_start
.global data_end
.global bss_start
.global bss_end


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
    .section  .multiboot_header, "ax"
    .code32
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
        .long    0
        .long    0
        .long    0
        .long    0
        .long    0
#endif /* __ELF__ */
        .long    0
        .long    1024
        .long    768
        .long    32



/*******************************************************************************
  32bit protect mode code
*******************************************************************************/
    X64_CODE32_FUNC_BEGIN(multiboot_entry)
multiboot_entry:
    /* Init stack pointer */
    movl   $stack_end, %esp
    /* Save multiboot infomation */
    push    %ebx
    push    %eax

	call    cmain

    /* Clear EFLAGS register, disable makeable interrupt. */
    pushl  $0
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


/* init 4-level page table */



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
    .fill    (512 - 4), X86_MMU_PAGE_ENTRY_SIZE, 0

	.align  4096
X64_ASM_DATA(x64_pde0_table)
    .fill    512,  X86_MMU_PAGE_ENTRY_SIZE,  0


	.align  4096
X64_ASM_DATA(x64_pde1_table)
    .fill    512,  X86_MMU_PAGE_ENTRY_SIZE,  0

	.align  4096
X64_ASM_DATA(x64_pde2_table)
    .fill    512,  X86_MMU_PAGE_ENTRY_SIZE,  0


	.align  4096
X64_ASM_DATA(x64_pde3_table)
    .fill    512,  X86_MMU_PAGE_ENTRY_SIZE,  0


	.align  4096
X64_ASM_DATA(x64_pte_table)
    .fill    (512 * 512),  X86_MMU_PAGE_ENTRY_SIZE,  0
    .fill    (512 * 512),  X86_MMU_PAGE_ENTRY_SIZE,  0
    .fill    (512 * 512),  X86_MMU_PAGE_ENTRY_SIZE,  0
    .fill    (512 * 512),  X86_MMU_PAGE_ENTRY_SIZE,  0



    X64_CODE32_FUNC_BEGIN(mmu_translate_table_init)
mmu_translate_table_init:
    /* map: pte -> pde */
    movl    x64_pte_table,    %esi
    movl    x64_pde0_table,   %edi

    movl    $(512 * 0x04),    %ecx
//    movl    $(X64_PDE_PT_PRESENT_BIT|X64_PDE_PT_RW_BIT),    %eax
_pde_loop:
    movl    %esi,    %eax
    add     $(X64_PDE_PT_PRESENT_BIT|X64_PDE_PT_RW_BIT),    %eax
    movl    %eax,    0(%edi)
    add     $(X86_MMU_PAGE_ENTRY_SIZE),    %edi



    movl    $(X64_PT_4KB_PRESENT_BIT|X64_PT_4KB_RW_BIT),    %ebx


X64_FUNC_END()





