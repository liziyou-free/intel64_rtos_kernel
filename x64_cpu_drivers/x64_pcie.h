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


#pragma pack(push)
#pragma pack(1)

typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint16_t revision_id;
    uint16_t class_code;
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


struct  config_header_type1{

};


struct pci_compatible_config_space_t{

};


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


#endif /* X64_PCIE_H_ */












