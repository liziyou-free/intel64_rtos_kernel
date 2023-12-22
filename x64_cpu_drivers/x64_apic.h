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
#include "../startup/x64_common.h"


#define X64_INTERRUPT_VECTOR_BASE    32


/*******************************************************************************
 * Local  APIC
*******************************************************************************/

/**
 *\brief Local APIC register offset
 */
//#define Local APIC_BASE_ADDR                       0xFEE00000

#define APIC_ID_REG_OFFSET                     0X20
#define APIC_VERSION_REG_OFFSET                0X20
#define APIC_TPR_REG_OFFSET                    0X80
#define APIC_APR_REG_OFFSET                    0X90
#define APIC_PPR_REG_OFFSET                    0XA0
#define APIC_EOI_REG_OFFSET                    0XB0
#define APIC_RRD_REG_OFFSET                    0XC0

#define APIC_LOGICAL_DEST_REG_OFFSET           0XD0
# define APIC_LDR_ID_SHIFT                     24

#define APIC_DEST_FORMAT_REG_OFFSET            0XE0
# define _DFR_MODEL_SHIFT_                     28
# define APIC_DFR_FLAT_MODEL                   (0x0f << _DFR_MODEL_SHIFT_)
# define APIC_DFR_CLUSTER_MODEL                (0x00 << _DFR_MODEL_SHIFT_)

#define APIC_SPURIOUS_INTERRUPT_REG_OFFSET     0xF0
# define APIC_SPURIOUS_VECTOR_SHIFT            0
# define APIC_SOFTWARE_ENABLE_SHIFT            8
# define APIC_FOCUS_CHECK_SHIFT                9
# define APIC_EOI_SUPPRESSION_SHIFT            12

#define APIC_ERR_STATUS_REG_OFFSET             0X280
#define APIC_ICR_LOW_REG_OFFSET                0X300
#define APIC_ICR_HIGH_REG_OFFSET               0X310


#define APIC_INITIAL_COUNTER_REG_OFFSET        0X380
#define APIC_CURRENT_COUNTER_REG_OFFSET        0X390
#define APIC_DIVIDE_CONFIG_REG_OFFSET          0X3E0

/* Local APIC interrupt vector register */
#define APIC_LVT_CMCI_REG_OFFSET               0X2F0
#define APIC_LVT_TIMER_REG_OFFSET              0X320
#define APIC_LVT_THERMAL_SENSOR_REG_OFFSET     0X330
#define APIC_LVT_MONITOR_COUNTER_REG_OFFSET    0X340
#define APIC_LVT_LINT_0_REG_OFFSET             0X350
#define APIC_LVT_LINT_1_REG_OFFSET             0X360
#define APIC_LVT_ERR_REG_OFFSET                0X370

/* APIC LVT field */
#define LVT_VECTOR_SHIFT                       0
#define LVT_DELIVERY_MODE_SHIFT                8
#define LVT_DELIVERY_STATUS_SHIFT              12
#define LVT_PIN_POLARITY_SHIFT                 13
#define LVT_REMOTE_IRR_SHIFT                   14
#define LVT_TRIGGER_MODE_SHIFT                 15
#define LVT_MASK_SHIFT                         16


/*******************************************************************************
 * I/O  APIC
*******************************************************************************/

/**
 *\brief I/O APIC register offset
 */
#define IOAPIC_BASE_ADDR                       0xFEC00000
#define IOAPIC_REG_INDEX                       0x00
#define IOAPIC_REG_DATA                        0x10
#  define IOAPIC_REG_ID                        0x00       /* Register index: ID */
#  define IOAPIC_REG_VER                       0x01       /* Register index: version */
#  define IOAPIC_REG_TABLE                     0x10       /* Redirection table base */
#  define IOAPIC_PIN_DISABLE                  (1 << 16)   /* Disable */



/**
 *\brief ioapic_trigger_mode_t
 */
#define TRIGGER_RISING_EDGE            0
#define TRIGGER_FALLING_EDGE           1
#define TRIGGER_LEVEL_ACTIVE_HIGH      4
#define TRIGGER_LEVEL_ACTIVE_LOW       5


/**
 *\brief delivery_mode_t
 *\note How the interrupt will be sent to the CPU(s).Most of the cases you want
 * Fixed mode, or Lowest Priority if you don't want to suspend a high priority
 * task on some important Processor/Core/Thread.
 */
#define DELIVERY_MODE_FIXED            0
#define DELIVERY_MODE_LOWEST_PRIO      1
#define DELIVERY_MODE_SMI              2
#define DELIVERY_MODE_NMI              4
#define DELIVERY_MODE_INIT             5
#define DELIVERY_MODE_EXTINT           7


/**
 *\brief destination_mmode_t
 */
#define PHYSICAL_DESTINATION           0
#define LOGICAL_DESTINATION            1


typedef struct ioapic_intline_config {
    uint64_t vector:8;
    uint64_t delivery_mode:3;     /* @ delivery_mode_t */
    uint64_t destination_mode:1;  /* @ destination_mmode_t */
    uint64_t delivery_status:1;
    uint64_t trigger_mode:3;      /* @ ioapic_trigger_mode_t */
    uint64_t mask:1;
    uint64_t :39;
    uint64_t destination:8;
} ioapic_intline_config_t;


void cpu_local_apic_open (void);

void intel_local_apic_init (uint16_t core_id);

void apic_eoi_hook (void);

void intel_ioapic_init (void);

void ioapic_mask_irq (uint8_t inum);

void ioapic_unmask_irq (uint8_t inum);

void cpu_send_ipi_to_self();

#endif /* X64_APIC_H_ */






