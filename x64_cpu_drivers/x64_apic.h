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

#ifndef X64_APIC_H_
#define X64_APIC_H_

#include <stdint.h>


/**
 *\brief Local APIC register offset
 */
//#define LAPIC_BASE_ADDR                       0xFEE00000

#define LAPIC_ID_REG_OFFSET                     0X20
#define LAPIC_VERSION_REG_OFFSET                0X20
#define LAPIC_TPR_REG_OFFSET                    0X80
#define LAPIC_APR_REG_OFFSET                    0X90
#define LAPIC_PPR_REG_OFFSET                    0XA0
#define LAPIC_EOI_REG_OFFSET                    0XB0
#define LAPIC_RRD_REG_OFFSET                    0XC0

#define LAPIC_LOGICAL_DEST_REG_OFFSET           0XD0
#define LAPIC_LDR_ID_SHIFT                      24

#define LAPIC_DEST_FORMAT_REG_OFFSET            0XE0
#define _DFR_MODEL_SHIFT_                       28
#define LAPIC_DFR_FLAT_MODEL                    (0x0f << _DFR_MODEL_SHIFT_)
#define LAPIC_DFR_CLUSTER_MODEL                 (0x00 << _DFR_MODEL_SHIFT_)

#define LAPIC_SPURIOUS_INTERRUPT_REG_OFFSET     0xF0
#define LAPIC_SPURIOUS_VECTOR_SHIFT             0
#define LAPIC_SOFTWARE_ENABLE_SHIFT             8
#define LAPIC_FOCUS_CHECK_SHIFT                 9
#define LAPIC_EOI_SUPPRESSION_SHIFT             12

#define LAPIC_ERR_STATUS_REG_OFFSET             0X280

#define LAPIC_CMCI_REG_OFFSET                   0X2F0

#define LAPIC_ICR_LOW_REG_OFFSET                0X300
#define LAPIC_ICR_HIGH_REG_OFFSET               0X310
#define LAPIC_TIMER_REG_OFFSET                  0X320
#define LAPIC_THERMAL_SENSOR_REG_OFFSET         0X330
#define LAPIC_MONITOR_COUNTER_REG_OFFSET        0X340
#define LAPIC_LINT_0_REG_OFFSET                 0X350
#define LAPIC_LINT_1_REG_OFFSET                 0X360
#define LAPIC_LVT_ERR_REG_OFFSET                0X370
#define LAPIC_INITIAL_COUNTER_REG_OFFSET        0X380
#define LAPIC_CURRENT_COUNTER_REG_OFFSET        0X390
#define LAPIC_DIVIDE_CONFIG_REG_OFFSET          0X3E0



void cpu_local_apic_open (void);

void intel_local_apic_init(uint16_t core_id);

#endif /* X64_APIC_H_ */






