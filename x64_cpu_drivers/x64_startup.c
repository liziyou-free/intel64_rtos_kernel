/*
 * startup.c
 *
 *  Created on: 2023年9月7日
 *      Author: liziyou
 */

#include <stdint.h>
#include "../x64_cpu_drivers/x64_segment_def.h"

/*
 * A descriptor table is simply a memory array of 8byte entries that contain
 * descriptors. A descriptor table is variable in length and may contain up
 * to 8192 (2^13) descriptors.
 */
//x86_gdt_struct_t g_x86_gdt[64]__attribute__((aligned(16)));


/**
 *\note The base addresses of the IDT should be aligned on an 8-byte boundary
 * to maximize performance of cache line fills.
 */
uint64_t g_x86_idt[256 * 2]__attribute__((aligned(8)));



void irq_handle(void) {
  static int temp = 5;
  int a = 0;
  a = a + 5;
  temp = a + temp;
}


void x64_idt_init(void) {

  x64_idt_int_trap_gate_t  obj;
  uint16_t idt_elements = sizeof(g_x86_idt) / 16; /* a descriptor:16bytes */

  for (int j = 0; j < idt_elements; j++) {
      if (j < 32) {
          /* exception */
          x64_create_gate_descriptor(&obj, irq_handle, 1, TRAP_GATE_386, PRIVILEGE_LEVEL_0);
      } else {
          /* interrupt */
          x64_create_gate_descriptor(&obj, irq_handle, 1, INTERRUPT_GATE_386, PRIVILEGE_LEVEL_0);
      }
      x64_add_descriptor_to_idt(&obj, &g_x86_idt[0], j);
  }
  /* Update to IDTR register */
  x64_flush_idtr_register(g_x86_idt, sizeof(g_x86_idt) - 1);

  return;
}




