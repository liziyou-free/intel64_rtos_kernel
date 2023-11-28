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
#include "../x64_driver/i8253_timer.h"
#include "../x64_driver/x64_serial.h"

void x86_timer_init (void);
void lv_demo_stress(void);
void x64_lvgl_init (void);
void lvgl_tick_and_handle (void);

int main(int argc, char**arg) {

    volatile long long count = 0;
    read(0,arg,5);
    x86_timer_init();
    x86_isa_serial_init();
    x86_serial_send_str("Application Start!");
    x64_lvgl_init();
    lv_demo_stress();
      for (;;) {
          count ++;
          if (count > 1000000) {
              count = 0;
              lvgl_tick_and_handle();
              x86_serial_send_str("FreedomLi \r\n");
              continue;
          }
      }
    return 0;
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
