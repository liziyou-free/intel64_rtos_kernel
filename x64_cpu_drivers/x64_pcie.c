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
#include <stdio.h>
#include <string.h>
#include "./x64_pcie.h"
#include "../startup/x64_common.h"

#define PRINT_PCI_DEV_SYMBOL() \
    printf("\r\n"); \
    printf("****************************************************\r\n"); \
    printf("*            ----- PCI DEVICE INFO -------         *\r\n"); \
    printf("****************************************************\r\n");

#define PRINT_PCI_BRIDGE_SYMBOL() \
    printf("\r\n"); \
    printf("****************************************************\r\n"); \
    printf("*            ----- PCI BRIDGE INFO -------         *\r\n"); \
    printf("****************************************************\r\n");


#define __ENABLE_CONFIG_SPACE_MAP__    (1ul << 31)

#define BDF(b, d, f)    ((b << 8) | (d << 3) | f)


char* classcode_to_text(class_code_t *classcode);


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



void pcie_read_config_header(uint32_t bdf, void *obj)
{
    uint32_t *info;

    info = (uint32_t *)obj;

    for (int j = 0; j < 16; j++) {
        info[j] = pci_atomic_read(bdf, j);
    }
    return;
}


void dump_pcie_dev_info(uint32_t bdf)
{
    #define PRINT_LINE(str, ...) \
                printf(str "\r\n", __VA_ARGS__)

    cfg_header_type0_t *obj;
    cfg_header_type1_t *obj_type1;
    uint8_t pci_cfg[sizeof(cfg_header_type0_t)];

    pcie_read_config_header(bdf, (void *)&pci_cfg[0]);
    obj = (cfg_header_type0_t *)&pci_cfg[0];

    if (obj->vendor_id == 0xffff) return;
    if (obj->class_code.base_class == 6) {
        obj_type1 = (cfg_header_type1_t *)&pci_cfg[0];
        PRINT_PCI_BRIDGE_SYMBOL();
        PRINT_LINE("BDF:  %d:%d:%d", (bdf >> 8), ((bdf >> 3) & 0x01F), (bdf & 0x07));
        PRINT_LINE("Class-Code: %s", classcode_to_text(&obj_type1->class_code));
        PRINT_LINE("Vendor-ID: %#x", obj_type1->vendor_id);
        PRINT_LINE("Device-ID: %#x", obj_type1->device_id);
        PRINT_LINE("Header-Type: %#x", obj_type1->header_type);
        PRINT_LINE("Command: %#x", obj_type1->command);
        PRINT_LINE("Status: %#x", obj_type1->status);
        PRINT_LINE("BA0: %#x", obj_type1->base_addr[0]);
        PRINT_LINE("BA1: %#x", obj_type1->base_addr[1]);
        PRINT_LINE("Primary bus num: %#x", obj_type1->primary_bus_num);
        PRINT_LINE("Secondary bus num: %#x", obj_type1->secondary_bus_num);
        PRINT_LINE("Subordinate bus num: %#x", obj_type1->subordinate_bus_num);
        PRINT_LINE("I/O base: %#x", obj_type1->io_base);
        PRINT_LINE("I/O limit: %#x", obj_type1->io_limit);
        PRINT_LINE("Memory base: %#x", obj_type1->memory_base);
        PRINT_LINE("Memory limit: %#x", obj_type1->memory_limit);

        PRINT_LINE("Interrupt-Line: %#x", obj_type1->interrupt_line);
        PRINT_LINE("Interrupt-Pin: %#x", obj_type1->interrupt_pin);
    } else {
        PRINT_PCI_DEV_SYMBOL();
        PRINT_LINE("BDF:  %d:%d:%d", (bdf >> 8), ((bdf >> 3) & 0x01F), (bdf & 0x07));
        PRINT_LINE("Class-Code: %s", classcode_to_text(&obj->class_code));
        PRINT_LINE("Vendor-ID: %#x", obj->vendor_id);
        PRINT_LINE("Device-ID: %#x", obj->device_id);
        PRINT_LINE("Header-Type: %#x", obj->header_type);
        PRINT_LINE("Command: %#x", obj->command);
        PRINT_LINE("Status: %#x", obj->status);
        PRINT_LINE("BA0: %#x", obj->base_addr[0]);
        PRINT_LINE("BA1: %#x", obj->base_addr[1]);
        PRINT_LINE("BA2: %#x", obj->base_addr[2]);
        PRINT_LINE("BA3: %#x", obj->base_addr[3]);
        PRINT_LINE("BA4: %#x", obj->base_addr[4]);
        PRINT_LINE("BA5: %#x", obj->base_addr[5]);
        PRINT_LINE("SubSytem Vendor-Id: %#x", obj->subsystem_vendor_id);
        PRINT_LINE("SubSytem Id: %#x", obj->subsystem_id);
        PRINT_LINE("Interrupt-Line: %#x", obj->interrupt_line);
        PRINT_LINE("Interrupt-Pin: %#x", obj->interrupt_pin);
    }
    return;
}


void pcie_device_enum()
{

    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t device = 0; device < 32; device++) {
            for (uint8_t function = 0; function < 8; function++) {
                dump_pcie_dev_info(BDF(bus, device, function));
            }
        }
    }

    return;
}



int8_t pcie_search_dev(uint16_t vendor_id, uint16_t dev_id, cfg_header_type0_t *obj)
{
    uint8_t pci_cfg[sizeof(cfg_header_type0_t)];
    cfg_header_type0_t *cfg;
    uint32_t bdf;

    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t device = 0; device < 32; device++) {
            for (uint8_t function = 0; function < 8; function++) {
                bdf = BDF(bus, device, function);
                memset(pci_cfg, 0, sizeof(*pci_cfg));
                pcie_read_config_header(bdf, (void *)&pci_cfg[0]);
                cfg = (cfg_header_type0_t *)&pci_cfg[0];
                if (cfg->vendor_id == vendor_id && cfg->device_id == dev_id)
                {
                    memcpy(obj, cfg, sizeof(*cfg));
                    return 0;
                }
            }
        }
    }

    return 1;
}


char* classcode_to_text(class_code_t *classcode)
{
    const char* p_text;

    switch (classcode->base_class) {
      case 0x00:
          #define  CLASS0_TEXT  "Device "
          p_text = CLASS0_TEXT;
          break;
      case 0x01:
          #define  CLASS1_TEXT   "Mass storage controller "
          p_text = CLASS1_TEXT;
          break;
      case 0x02:
          #define  CLASS2_TEXT   "Network controller "
          p_text = CLASS2_TEXT;
          break;
      case 0x03:
          #define  CLASS3_TEXT   "Display controller "
          p_text = CLASS3_TEXT;
          break;
      case 0x04:
          #define  CLASS4_TEXT   "Multimedia device "
          p_text = CLASS4_TEXT;
          break;
      case 0x05:
          #define  CLASS5_TEXT   "Memory controller "
          p_text = CLASS5_TEXT;
          break;
      case 0x06:
          #define  CLASS6_TEXT   "Bridge device "
          p_text = CLASS6_TEXT;
          break;
      case 0x07:
          #define  CLASS7_TEXT   "Simple communication controllers "
          p_text = CLASS7_TEXT;
          break;
      case 0x08:
          #define  CLASS8_TEXT   "Base system peripherals "
          p_text = CLASS8_TEXT;
          break;
      case 0x09:
          #define  CLASS9_TEXT   "Input devices "
          p_text = CLASS9_TEXT;
          break;
      case 0x0A:
          #define  CLASS10_TEXT   "Docking stations "
          p_text = CLASS10_TEXT;
          break;
      case 0x0B:
          #define  CLASS11_TEXT   "Processors "
          p_text = CLASS11_TEXT;
          break;
      case 0x0C:
          #define  CLASS12_TEXT   "Serial bus controllers "
          p_text = CLASS12_TEXT;
          break;
      case 0x0D:
          #define  CLASS13_TEXT   "Wireless controller "
          p_text = CLASS13_TEXT;
          break;
      case 0x0E:
          #define  CLASS14_TEXT   "Intelligent I/O controllers "
          p_text = CLASS14_TEXT;
          break;
      case 0x0F:
          #define  CLASS15_TEXT   "Satellite communication controllers "
          p_text = CLASS15_TEXT;
          break;
      case 0x10:
          #define  CLASS16_TEXT   "Encryption/Decryption controllers "
          p_text = CLASS16_TEXT;
          break;
      case 0x11:
          #define  CLASS17_TEXT   "Data acquisition and signal processing controllers "
          p_text = CLASS17_TEXT;
          break;
      case 0x12:
          #define  CLASS18_TEXT   "Processing accelerators "
          p_text = CLASS18_TEXT;
          break;
      case 0x13:
          #define  CLASS19_TEXT   "Non-Essential Instrumentation "
          p_text = CLASS19_TEXT;
          break;
      default:
          break;
    }

    return (char*)p_text;
}




