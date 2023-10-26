/*
 * startup.c
 *
 *  Created on: 2023年9月7日
 *      Author: liziyou
 */


#include "../startup/multiboot.h"
#include "../startup/x64_startup.h"
#include "../Board/x64_pc_board.h"
#include "../x64_drivers/x64_dt.h"

/*
 * A descriptor table is simply a memory array of 8byte entries that contain
 * descriptors. A descriptor table is variable in length and may contain up
 * to 8192 (2^13) descriptors.
 */
x86_gdt_struct_t g_x86_gdt[X86_GDT_NUM];


/*
 * The IDT may contain any of three kinds of descriptor:
 *  -Task gates
 *  -Interrupt gates
 *  -Trap gates
 */
uint64_t g_x86_idt[X86_IDT_NUM];


/* Except handle  */
extern void (*x86_except_handle_array[16])(void *);

/* Interrupt handle  */
extern void (*x86_interrupt_handle_array[16])(void *);



static __inline__ void set_segment_register(uint16_t cs, uint16_t ds, uint16_t ss) {

    /**
      * Modify ss,ds,es,gs,fs register using the mov instruction,
      * Modify the cs register using the lret instruction.
      */
    __asm__  __volatile__("mov    %0,    %%ss\n\t"
                          "mov    %1,    %%ds\n\t"
                          "mov    %1,    %%es\n\t"
                          "mov    %1,    %%gs\n\t"
                          "mov    %1,    %%fs\n\t"
                          "push   %2         \n\t"
                          "pushl  $set_cs    \n\t"
                          "lret              \n\t"
                          "set_cs:           \n\t"
                          "nop"
                          :
                          :"r"(ss), "r"(ds), "r"(cs));
    return;
}


void x86_kerner_memory_space_init(){

    uint64_t    descriptor;
    uint16_t    ss, cs, ds;

    g_x86_gdt[0].u64 = 0x00;

    /* Kernel code segment descriptor init */
    descriptor = x86_generate_segment_descriptor(KERNEL_CODE_ADDRESS_START,
                                                 KERNEL_CODE_ADDRESS_SIZE,
                                                 app_code_xrc_segment,
                                                 privilege_level_0);

    x86_add_item_to_gdt(g_x86_gdt, KERNEL_CODE_GDT_INDEX, descriptor);

    /* Kernel data segment descriptor init */
    descriptor = x86_generate_segment_descriptor(KERNEL_DATA_ADDRESS_START,
                                                 KERNEL_DATA_ADDRESS_SIZE,
                                                 app_data_rw_segment,
                                                 privilege_level_0);
    x86_add_item_to_gdt(g_x86_gdt, KERNEL_DATA_GDT_INDEX, descriptor);

    /* Kernel stack segment descriptor init */
    descriptor = x86_generate_segment_descriptor(KERNEL_STACK_ADDRESS_START,
                                                 KERNEL_STACK_ADDRESS_SIZE,
                                                 app_data_rwe_segment,
                                                 privilege_level_0);
    x86_add_item_to_gdt(g_x86_gdt, KERNEL_STACK_GDT_INDEX, descriptor);

    /* Kernel interrupt descriptor init */
    descriptor = x86_generate_segment_descriptor(KERNEL_ISR_ADDRESS_START,
                                                 KERNEL_ISR_ADDRESS_SIZE,
                                                 app_code_xrc_segment,
                                                 privilege_level_0);
    x86_add_item_to_gdt(g_x86_gdt, KERNEL_INT_EXCEPT_GDT_INDEX, descriptor);


    cs = x86_generate_selector(KERNEL_CODE_GDT_INDEX,
                               table_indicator_gdt,
                               privilege_level_0);

    ds = x86_generate_selector(KERNEL_DATA_GDT_INDEX,
                               table_indicator_gdt,
                               privilege_level_0);

    ss = x86_generate_selector(KERNEL_STACK_GDT_INDEX,
                               table_indicator_gdt,
                               privilege_level_0);

    x86_gdtr_flush((uint32_t *)&g_x86_gdt[0], sizeof(g_x86_gdt) - 1);

    set_segment_register(cs, ds, ss);

    return;
}


void x86_exit0_isr(void *p) {
    //printf("This is my first extern interrupt function! --@FreedomLi, come on!");
    x86_serial_send(x86_serial_receive());
}



void x86_interrupt_except_init() {

    uint64_t    descriptor = 0;
    uint16_t    selector   = 0;

    selector = x86_generate_selector(KERNEL_INT_EXCEPT_GDT_INDEX,
                                     table_indicator_gdt,
                                     privilege_level_0);

    for (uint8_t index = 0; index < 16; index++) {

        descriptor = x86_generate_int_trap_gate_descriptor((uint32_t)x86_except_handle_array[index],
                                                           selector,
                                                           trap_gate_386,
                                                           privilege_level_0);
        x86_add_item_to_idt(g_x86_idt, index, descriptor);
    }

    for (uint8_t index = 0; index < 16; index++) {

        descriptor = x86_generate_int_trap_gate_descriptor((uint32_t)x86_interrupt_handle_array[index],
                                                           selector,
                                                           interrupt_gate_386,
                                                           privilege_level_0);
        x86_add_item_to_idt(g_x86_idt, index + 32, descriptor);
    }

    x86_idtr_flush((uint32_t *)g_x86_idt, sizeof(g_x86_idt) - 1);
}






