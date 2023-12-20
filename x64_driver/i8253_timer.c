/*
 * x86_8253_timer.c
 *
 *  Created on: 2023年9月15日
 *      Author: liziyou
 */

#include "./i8253_timer.h"
#include "../Board/x64_pc_board.h"
#include "../x64_cpu_drivers/x64_apic.h"


void timer_interrupt_handler (void *p_param);


void i8253_timer_config (timer_channel_sel_t channel,
                            uint32_t  des_fre,
                            timer_mode_t mode,
                            read_load_type_t type,
                            data_format_t format)
{
    uint8_t  cotrol_reg = 0;
    uint32_t temp = 0;

    cotrol_reg |=  channel << 6;
    cotrol_reg |=  type << 4;
    cotrol_reg |=  mode << 1;
    cotrol_reg |=  format << 0;
    /* Set control register */
    outb(cotrol_reg, X86_PORT_TIM1_CONTORL_REG);

    /* Set divid register */
    temp = ((TIMER_CLOCK_FREQUENCY / des_fre) % 256);
    outb(temp, X86_PORT_TIM1_CH0_COUNT_REG);
    temp = ((TIMER_CLOCK_FREQUENCY / des_fre) / 256);
    outb(temp, X86_PORT_TIM1_CH0_COUNT_REG);
}


void x86_timer_init (void)
{
    #define __SYSTEM_TICK__    1000 //1000

    i8253_timer_config(time_channel0,
                       __SYSTEM_TICK__,
                       rate_generator,
                       low_then_high,
                       binary_format);

    /* register interrupt handler */
    x64_irq_handler_register(IRQ2, timer_interrupt_handler, NULL);

    /* 使能中断线 */
    ioapic_unmask_irq(IRQ2);
    return;
}


void timer_interrupt_handler (void *p_param)
{
    static uint16_t global_tick = 0;
    /* LVGL tick */
    void lv_tick_inc(uint32_t tick_period);
    lv_tick_inc(1);

    global_tick++;
    if(global_tick == 1000) {
//        x86_serial_send(0X3F8, '#');
        global_tick = 0;
    }
}



