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

/**
 *\brief Local APIC register index
 */
#define LAPIC_BASE_ADDR                       0xFEE00000
#define LAPIC_ID_REGSITER                     (LAPIC_BASE_ADDR + 0X20)
#define LAPIC_VERSION_REGSITER                (LAPIC_BASE_ADDR + 0X20)
#define LAPIC_TPR_REGSITER                    (LAPIC_BASE_ADDR + 0X80)
#define LAPIC_APR_REGSITER                    (LAPIC_BASE_ADDR + 0X90)
#define LAPIC_PPR_REGSITER                    (LAPIC_BASE_ADDR + 0XA0)
#define LAPIC_EOI_REGSITER                    (LAPIC_BASE_ADDR + 0XB0)
#define LAPIC_RRD_REGSITER                    (LAPIC_BASE_ADDR + 0XC0)
#define LAPIC_LOGICAL_DEST_REGSITER           (LAPIC_BASE_ADDR + 0XD0)
#define LAPIC_DEST_FORMAT_REGSITER            (LAPIC_BASE_ADDR + 0XD0)
#define LAPIC_ERR_STATUS_REGSITER             (LAPIC_BASE_ADDR + 0X280)
#define LAPIC_CMCI_REGSITER                   (LAPIC_BASE_ADDR + 0X2F0)
#define LAPIC_ICR_LOW_REGSITER                (LAPIC_BASE_ADDR + 0X300)
#define LAPIC_ICR_HIGH_REGSITER               (LAPIC_BASE_ADDR + 0X310)
#define LAPIC_TIMER_REGSITER                  (LAPIC_BASE_ADDR + 0X320)
#define LAPIC_THERMAL_SENSOR_REGSITER         (LAPIC_BASE_ADDR + 0X330)
#define LAPIC_MONITOR_COUNTER_REGSITER        (LAPIC_BASE_ADDR + 0X340)
#define LAPIC_LINT_0_REGSITER                 (LAPIC_BASE_ADDR + 0X350)
#define LAPIC_LINT_1_REGSITER                 (LAPIC_BASE_ADDR + 0X360)
#define LAPIC_LVT_ERR_REGSITER                (LAPIC_BASE_ADDR + 0X370)
#define LAPIC_INITIAL_COUNTER_REGSITER        (LAPIC_BASE_ADDR + 0X380)
#define LAPIC_CURRENT_COUNTER_REGSITER        (LAPIC_BASE_ADDR + 0X390)
#define LAPIC_DIVIDE_CONFIG_REGSITER          (LAPIC_BASE_ADDR + 0X3E0)

void intel_apic_init(void);

#endif /* X64_APIC_H_ */






