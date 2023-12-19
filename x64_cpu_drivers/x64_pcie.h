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

#ifndef X64_PCIE_H_
#define X64_PCIE_H_


#include <stdbool.h>

/**
 * \brief pci capability id assigned by PCI-SIG
 */
#define NULL_ID                          0x00
#define POWER_MANAGE_INTERFACE_ID        0x01
#define AGP_ID                           0x02
#define VPD_ID                           0x03
#define SLOT_IDENTIFICATION_ID           0x04
#define MSI_ID                           0x05
#define COMPACT_PCI_HOT_SWAP_ID          0x06
#define PCI_X_ID                         0x07
#define HYPER_TRANSPORT_ID               0x08
#define VENDOR_SPECIFIC_ID               0x09
#define DEBUG_PORT_OD                    0x0A
#define COMPACT_PCI_CENTRAL_RESORCE_ID   0x0B
#define PCI_HOT_PLUG_ID                  0x0C
#define PCI_BRIDGE_SUBSYS_VENDOR_ID      0x0D
#define AGP_8X_ID                        0x0E
#define SECURE_DEVICE_ID                 0x0F
#define PCI_EXPRESS_ID                   0x10
#define MSI_X_ID                         0x11
#define SERIAL_ATA_ID                    0x12
#define ADVANCED_FEATURES_ID             0x13
#define ENHANCED_ALLOCA_ID               0x14
#define FLATTENING_PORTAL_BRIDGE_ID      0x15




#define PCI_STA_REG_CAP_BIT              (1 << 4)



#pragma pack(push)
#pragma pack(1)

typedef struct {
    uint8_t Programming_Interface;
    uint8_t sub_class;
    uint8_t base_class;
} class_code_t;

typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t  revision_id;
    class_code_t class_code;
    uint8_t  cache_line_size;
    uint8_t  latency_timer;
    uint8_t  header_type;
    uint8_t  bist;
    uint32_t base_addr[6];
    uint32_t cardbus_cis_pointer;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    uint32_t expansion_rom_base_addr;
    uint8_t  capability_pointer;
    uint8_t  reserved[7];
    uint8_t  interrupt_line;
    uint8_t  interrupt_pin;
    uint8_t  min_gnt;
    uint8_t  max_lat;
} cfg_header_type0_t;


typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t  revision_id;
    class_code_t class_code;
    uint8_t  cache_line_size;
    uint8_t  latency_timer;
    uint8_t  header_type;
    uint8_t  bist;
    uint32_t base_addr[2];
    uint8_t  primary_bus_num;
    uint8_t  secondary_bus_num;
    uint8_t  subordinate_bus_num;
    uint8_t  secondary_latency_timer;
    uint8_t  io_base;
    uint8_t  io_limit;
    uint16_t secondary_status;
    uint16_t memory_base;
    uint16_t memory_limit;
    uint16_t prefetchable_memory_base;
    uint16_t prefetchable_memory_limit;
    uint32_t prefetchable_base_upper32;
    uint32_t prefetchable_limmit_upper32;
    uint16_t io_base_upper16;
    uint16_t io_limit_upper16;
    uint8_t  capability_pointer;
    uint8_t  reserved[3];
    uint32_t expansion_rom_base_addr;
    uint8_t  interrupt_line;
    uint8_t  interrupt_pin;
    uint16_t bridge_control;
} cfg_header_type1_t;



struct pci_compatible_config_space_t{

};




typedef struct {
  uint8_t dev_type;

}pci_bus;


#pragma pack(pop)


/**
 * \brief Configuration Address Port At 0x0CF8
 *
 *  31  30        24 23    16 15     11 10       8 7       2 1  0
 *  --------------------------------------------------------------
 *  | En | Reserved |Bus Num | Dev Num | Func Num | Reg Idx | 00 |
 *  --------------------------------------------------------------
 */
#define CONFIG_ADDRESS_PORT        0x0CF8

#define CONFIG_DATA_PORT           0x0CFC


void pcie_device_enum();
int8_t pcie_search_dev(uint16_t vendor_id, uint16_t dev_id, uint32_t *p_bdf, cfg_header_type0_t *obj);
uint32_t pcie_get_capbility (uint32_t bdf, uint16_t cap_id, bool *ret);

#endif /* X64_PCIE_H_ */












