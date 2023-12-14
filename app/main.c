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

#include "../x64_cpu_drivers/x64_apic.h"
#include "../x64_cpu_drivers/x64_mmu.h"
#include "../x64_driver/i8253_timer.h"
#include "../x64_driver/x64_serial.h"
#include "../Board/x64_pc_board.h"


void x86_timer_init (void);
void lv_demo_stress(void);
void lv_demo_benchmark(void);
void lv_demo_music(void);
void x64_lvgl_init (void);
void lvgl_tick_and_handle (void);

extern uint8_t test_data[];

int main (void)
{
    phyaddr_info phyaddrinfo = {0, 0};
    volatile long long count = 0;

    char *p = (char*)malloc(1024*1024);

    table_unit_t *addr;
    x64_lookup_phyaddr(&phyaddrinfo, 0xfee00000ull);
    addr = x64_mmu_alloc_table(2);
    mmu_print_table_poor_bitmap();
    x64_mmu_alloc_table(5);
    mmu_print_table_poor_bitmap();
    x64_mmu_free_table(addr);
    x64_mmu_free_table(++addr);
    mmu_print_table_poor_bitmap();

    uint64_t flag = X64_MMU_PRESENT_BIT | X64_MMU_RW_BIT;
    x64_mmu_mmap_setup(0x140100000, (uint64_t)test_data, 0x301000, flag, x64_mmu_alloc_table);
    mmu_print_table_poor_bitmap();

    uint8_t *dst, *src;
    dst = (uint8_t *)0x140100000;
    src = (uint8_t *)test_data;
    for (uint64_t c = 0; c < 0x301000; c++) {
        if (*dst != *src) {
            mmu_print_table_poor_bitmap();
            for (;;);
        }
        dst++;
        src++;
    }

    x86_timer_init();
    serial_init(X64_PORT_COM1, 115200, 1);
    x86_serial_send_str(X64_PORT_COM1, "Application Start!\r\n");
    x64_lvgl_init();
    lv_demo_benchmark();
      for (;;) {
          count ++;
          if (count > 10000) {
              count = 0;
              lvgl_tick_and_handle();
              continue;
          }
      }
    return 0;
}


void printf_init(void){
    serial_init(X64_PORT_COM1, 115200, 1);
}













/******************************************************************************/
//
//void task0 (void *p_arg) {
//    static int j = 0;
//    while(1){
//        j++;
//        if (j == 500) {
//            x86_serial_send_str((char*)p_arg);
//            break;
//        }
//    }
//    return;
//}
//
//
//void task1 (void *p_arg) {
//
//    static int j = 0;
//    while(1){
////        x86_serial_send(*(char*)p_arg);
//        j++;
//        if (j < 300) {
//            x86_serial_send_str((char*)p_arg);
//        } else {
//             break;
//        }
//    }
//    return;
//}
//
//
//void task_exit_hook(void *arg, void *arg1) {
//
//    x86_serial_send_str("Task Exit! %s\r\n");
//    x86_serial_send_str((char *)arg);
//    x86_serial_send_str((char *)arg1);
//    for(;;);
//
//}
//
//
///*
// *                     Performance Considerations
// * Because the hardware mechanism saves almost all of the CPU state it can be
// * slower than is necessary.For example,when the CPU loads new segment registers
// * it does all of the access and permission checks that are involved. As most
// * modern operating systems don't use segmentation,loading the segment registers
// * during context switches may be not be required, so for performance reasons t-
// * hese operating systems tend not to use the hardware context switching mechan-
// * ism. Due to it not being used as much CPU manufacturers don't optimize CPUs
// * for this method anymore (AFAIK). In addition the new 64 bit CPU's do not sup-
// * port hardware context switches when in 64 bit/long mode.
// */
//
//uint32_t *task_init_list_stack(uint32_t *stack_top,
//                               void(*p_func)(void*),
//                               void* p_arg,
//                               void(*p_exit)(void*, void*),
//                               void *p_exit_arg,
//                               void *p_exit_arg1)
//{
//
//    uint32_t    code_segment;
//
//    *stack_top = (uint32_t)p_exit_arg1;
//    stack_top--;
//
//    *stack_top = (uint32_t)p_exit_arg;
//    stack_top--;
//
//    /* First parameter of the task */
//    *stack_top = (uint32_t *)p_arg;
//    stack_top--;
//
//    *stack_top = (uint32_t)p_exit;
//    stack_top--;
//
//    /* EFLAG REGISTER (default value : 0x200 ==> 'eable interrupt') */
//    *stack_top = 0x200;
//    stack_top--;
//
//    /* Use the same code segment as the kernel. */
//    __asm__ __volatile__ ("movl  %%cs, %0":"=r"(code_segment));
//    *stack_top = code_segment;
//    stack_top--;
//
//    /* First instruction */
//    *stack_top = (uint32_t)p_func;
//    stack_top--;
//
//    /*all general registers */
//    *stack_top = 0xea;
//    stack_top--;
//
//    *stack_top = 0xec;
//    stack_top--;
//
//    *stack_top = 0xed;
//    stack_top--;
//
//    *stack_top = 0xeb;
//    stack_top--;
//
//    /* ESP */
//    *stack_top = 0xa5a5a5a5;
//    stack_top--;
//
//    /* EBP */
//    *stack_top = 0x00;
//    stack_top--;
//
//    /* ESI */
//    *stack_top = 0xea;
//    stack_top--;
//
//    /* EDI */
//    *stack_top = 0x55555555;
//
//    return stack_top;
//}
//
//
//
//
//
