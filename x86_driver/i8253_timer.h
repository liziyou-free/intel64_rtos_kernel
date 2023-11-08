/*
 * x86_8253_timer.h
 *
 *  Created on: 2023年9月15日
 *      Author: liziyou
 */

#ifndef X86_DRIVER_I8253_TIMER_H_
#define X86_DRIVER_I8253_TIMER_H_

#include "./x86_common.h"
#include "../Board/x86_pc_board.h"

#define X86_PORT_TIM1_CH0_COUNT_REG     (X86_PORT_TIMER1 + 0)
#define X86_PORT_TIM1_CH1_COUNT_REG     (X86_PORT_TIMER1 + 1)
#define X86_PORT_TIM1_CH2_COUNT_REG     (X86_PORT_TIMER1 + 2)
#define X86_PORT_TIM1_CONTORL_REG       (X86_PORT_TIMER1 + 3)

#define TIMER_CLOCK_FREQUENCY     1193182    /* Hz */


/*
 **\brief Intel 8253 timer channel select count
 * */
typedef enum {
    time_channel0 = 0,
    time_channel1,
    time_channel2,
}timer_channel_sel_t;


/**
 * \brief Intel 8253 timer read/load data type
 */
typedef enum {
    latch_counter_value = 0,
    only_low_byte,
    only_high_byte,
    low_then_high,
}read_load_type_t;


/**
 * \brief Intel 8253 timer data format
 */
typedef enum{
    bcd_format = 0,    /* 4 decades */
    binary_format,     /* 16-bit binary */
}data_format_t;


typedef enum {
    interrupt_on_terminal_count = 0,
    programmble_one_shot,
    rate_generator,
    square_wave_rate_gennerator,
    software_triggered_strobe,
    hardware_triggered_strobe,
}timer_mode_t;



void i8253_timer_config (timer_channel_sel_t channel,
                            uint32_t  des_fre,
                            timer_mode_t mode,
                            read_load_type_t type,
                            data_format_t format);


#endif /* X86_DRIVER_I8253_TIMER_H_ */
