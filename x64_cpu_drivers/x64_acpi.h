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

#ifndef X64_ACPI_H_
#define X64_ACPI_H_

#pragma pack(push)
#pragma pack(1)

typedef struct {
    uint8_t  signature[4];    /* "MCFG" */
    uint32_t table_length;
    uint8_t  revision;        /* '1' */
    uint8_t  checksum;
    uint8_t  oem_id[6];
    uint8_t  oem_table_id[8];
    uint32_t oem_revision;
    uint8_t  creator_id[4];
    uint32_t creator_revision;
    uint64_t reserved;
}apic_mcfg_table_t;

typedef struct {
    uint64_t base_addr;      /* Base address of enhanced configuration mechanism */
    uint16_t segment_num;    /* PCI Segment Group Number */
    uint8_t  bus_start_num;  /* Start PCI bus number decoded by this host bridge */
    uint8_t  bus_end_num;    /* End PCI bus number decoded by this host bridge */
    uint32_t reserved;
}config_space_struct_t;

#pragma pack(pop)


void acpi_init(void);
void* acpi_get_table(const char*  signature);

#endif /* X64_ACPI_H_ */





