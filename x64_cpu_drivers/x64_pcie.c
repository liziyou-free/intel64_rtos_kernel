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

#include <stdint.h>
#include "./x64_pcie.h"
#include "../startup/x64_common.h"



#define PRINT_PCI_DEV_SYMBOL() \
    printf("****************************************************\r\n"); \
    printf("*            ----- PCI DEVICE INFO -------         *\r\n"); \
    printf("****************************************************\r\n");




#define __ENABLE_CONFIG_SPACE_MAP__    (1ul << 31)


void pci_atomic_write(uint32_t bdf, uint32_t reg, uint32_t data)
{
    uint32_t addr_port;

    addr_port = ((bdf << 8) | (reg << 2)) | __ENABLE_CONFIG_SPACE_MAP__;
    /* Pay attention to locking later to ensure atomic access! */
    outl(addr_port, CONFIG_ADDRESS_PORT);
    outl(data, CONFIG_DATA_PORT);
}


uint32_t pci_atomic_read(uint32_t bdf, uint32_t reg)
{
    uint32_t addr_port;
    uint32_t data;

    addr_port = ((bdf << 8) | (reg << 2)) | __ENABLE_CONFIG_SPACE_MAP__;
    /* Pay attention to locking later to ensure atomic access! */
    outl(addr_port, CONFIG_ADDRESS_PORT);
    data = inl(CONFIG_DATA_PORT);
    return data;
}


#define BDF(b, d, f)    ((b << 8) | (d << 3) | f)


void pcie_read_config_header(uint32_t bdf, cfg_header_type0_t *obj)
{
    uint32_t *info;

    info = (uint32_t *)obj;

    for (int j = 0; j < 16; j++) {
        info[j] = pci_atomic_read(0, j);
    }
    return;
}


void dump_pcie_dev_info(uint32_t bdf)
{
    #define PRINT_LINE(str, __VA_ARGS__) \
                printf(str "\r\n", __VA_ARGS__)

    cfg_header_type0_t obj;

    PRINT_PCI_DEV_SYMBOL();

    pcie_read_config_header(bdf, &obj);
    PRINT_LINE("Vendor-ID: %#x", obj.vendor_id);
    PRINT_LINE("Device-ID: %#x", obj.device_id);
    PRINT_LINE("BA0: %#x", obj.base_addr[0]);
    PRINT_LINE("BA0: %#x", obj.base_addr[1]);
    PRINT_LINE("BA0: %#x", obj.base_addr[2]);
    PRINT_LINE("BA0: %#x", obj.base_addr[3]);
    PRINT_LINE("BA0: %#x", obj.base_addr[4]);
    PRINT_LINE("BA0: %#x", obj.base_addr[5]);
    PRINT_LINE("Class-Code: %#x", obj.class_code);
    PRINT_LINE("Header-Type: %#x", obj.header_type);
    PRINT_LINE("Interrupt-Line: %#x", obj.interrupt_line);
    PRINT_LINE("Interrupt-Pin: %#x", obj.interrupt_pin);
    return;
}


void pcie_device_enum()
{
    dump_pcie_dev_info(BDF(0, 9, 0));
    return;
}







