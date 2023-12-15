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
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "./x64_acpi.h"


uint64_t  g_acpi_config_space_addr;
uint64_t  g_acpi_config_space_len;

static char *string_search(char* src, uint64_t range, const char* need);


void acpi_init(void)
{
    apic_mcfg_table_t *p_mcfg;
    config_space_struct_t *cfg_table;

    printf("Config space addr %#llx \r\n", g_acpi_config_space_addr);

    p_mcfg = (apic_mcfg_table_t*)acpi_get_table("MCFG");
    cfg_table = (config_space_struct_t*)((void*)p_mcfg + sizeof(*p_mcfg));
    for (uint16_t k = -1; k > 0; k--) {

    }

    if (p_mcfg == NULL || cfg_table == NULL) {

        printf("ACPI Info Error!!! \r\n\r\n");
    }

    printf("****************************************************\r\n");
    printf("                ----- MCFG INFO -------             \r\n");
    printf("****************************************************\r\n");

    printf("signature %c %c %c %c",p_mcfg->signature[0],
                                   p_mcfg->signature[1],
                                   p_mcfg->signature[2],
                                   p_mcfg->signature[3]);
    printf("\r\n");
    printf("struct info length: %d \r\n", p_mcfg->table_length);
    printf("signature %c %c %c %c %c %c",p_mcfg->oem_id[0],
                                   p_mcfg->oem_id[1],
                                   p_mcfg->oem_id[2],
                                   p_mcfg->oem_id[3],
                                   p_mcfg->oem_id[4],
                                   p_mcfg->oem_id[5]);
    printf("\r\n");

    printf("PCIe Base Address: %#llx \r\n", cfg_table->base_addr);
    printf("PCIe bridge start No: %d \r\n", cfg_table->bus_start_num);
    printf("PCIe bridge end No: %d \r\n", cfg_table->bus_end_num);
    return;
}



static char *string_search(char* src, uint64_t range, const char* need)
{
    while (range--) {
        size_t i;
        i = 0;
        while (1) {
            if (need[i] == 0) {
                return (char *) src;
            }
            if (need[i] != src[i]) {
                break;
            }
            i++;
      }
        src++;
      }

    return (char *) NULL;
}


void* acpi_get_table(const char*  signature)
{
    void *p_table;

    p_table = (void*)string_search((char*)g_acpi_config_space_addr,
                                   g_acpi_config_space_len,
                                   signature);
    return p_table;
}










