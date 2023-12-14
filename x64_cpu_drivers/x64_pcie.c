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


#define __ENABLE_CONFIG_SPACE_MAP__    (1ul << 31)


void pci_config_space_write(uint32_t bdf, uint32_t reg, uint32_t data)
{
    uint32_t addr_port;

    addr_port = ((bdf << 8) | (reg << 2)) | __ENABLE_CONFIG_SPACE_MAP__;
    /* Pay attention to locking later to ensure atomic access! */
    outl(addr_port, CONFIG_ADDRESS_PORT);
    outl(data, CONFIG_DATA_PORT);
}


uint32_t pci_config_space_read(uint32_t bdf, uint32_t reg)
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

void pcie_device_enum()
{
    uint32_t info[16] = {0};

    for (int j = 0; j < 16; j++) {
        info[j] = pci_config_space_read(0, j);
    }
    for (int j = 0; j < 16; j++) {
        info[j] = pci_config_space_read(BDF(0, 3, 0), j);
    }
    return;
}






