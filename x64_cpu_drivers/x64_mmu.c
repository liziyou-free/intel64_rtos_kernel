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
#include "x64_mmu.h"

/**
 * Only support 4 level page, not support PCID, Most access 256 TBytes.
 * condition: (CR0.PG=1, CR4.PAE=1, IA32_EFER.LME=1, CR4.LA57=0, CR4.PCIDE=0)
 */


/*-----------------------------------------------------------------------------*/

/**
 * \brief Update MMU root translation table address.
 *
 * \param addr[in] root address
 * \param attr_flag[in] memory properties flag @ x64_cr3_memory_flag_t
 *
 * \retval none
 */
void x64_update_translate_table_addr(uint64_t addr, uint8_t attr_flag)
{
    uint64_t cr3 = 0;

    /* 4K aligned ? */
    if (ALIGN_CHECK(addr, 4096)) {
        for (;;);
    }
    cr3 = addr | attr_flag;
    __asm__ __volatile__("mov %0, %%cr3"::"r"(cr3));
    return;
}


/**
 * \brief Get the root translation table address of the MMU.
 *
 * \retval root address
 */
uint64_t x64_get_translate_table_addr (void)
{
  uint64_t cr3 = 0;

  __asm__ __volatile__("mov %%cr3,  %0":"=r"(cr3):);
  cr3 &= 0xfffffffffffff000ull;
  return cr3;
}


/**
 * \brife Linear-Address Translation with 4-Level Paging.
 *
 *                   Linear Address
 *   47      39 38     30 29     21 20     12 11      0
 *   --------------------------------------------------
 *   | level 1 | level 2 | level 3 | level 4 |  ADDR  |
 *   |  index  |  index  |  index  |  index  | OFFSET |
 *   --------------------------------------------------
 */
uint8_t x64_lookup_phyaddr (phyaddr_info *p_info, uint64_t va)
{
    uint8_t   shift;
    uint64_t  index;
    uint64_t  *tb;
    uint64_t  entry;
    uint64_t  offset;
    const uint8_t addr_bits = 12;
    const uint8_t index_bits = 9;

    /* Start searching from the root address */
    tb = (uint64_t *)x64_get_translate_table_addr();

    for (uint8_t j = 3; j >= 0; j--) {
        shift = ((j * index_bits) + addr_bits);
        index = (va >> shift) & 0x01ffull;
        if (index >= 512) {
            for (;;);
        }
        entry = tb[index];
        if (entry & X64_MMU_PRESENT_BIT) {
            /* When the PS bit is set, the next level page table is needed for
             * translation. When "j == 0", it means that the last level has
             * been reached. */
            if ((entry & X64_MMU_PS_BIT) || (j == 0) ) {
                switch (j) {
                case 2:
                    /* Use 1GB pages for mapping */
                    offset = va & 0x000000003fffffffull;
                    p_info->phy_addr = (entry & 0x0000ffffc0000000ull) | offset;
                    p_info->page_type = 1;
                    break;
                case 1:
                    /* Use 2MB pages for mapping */
                    offset = va & 0x00000000001fffffull;
                    p_info->phy_addr = (entry & 0x0000ffffffe00000ull) | offset;
                    p_info->page_type = 2;
                    break;
                case 0:
                    /* Use 4KB pages for mapping */
                    offset = va & 0x0000000000000fffull;
                    p_info->phy_addr = (entry & 0x0000fffffffff000ull) | offset;
                    p_info->page_type = 4;
                    break;
                default:
                    break;
                }
                break;
            } else {
                tb = (uint64_t *)(entry & 0x0000fffffffff000ull);
            }
        } else {
            /* Not present! */
            p_info->phy_addr = -1;
            p_info->page_type = 0;
            return 1;
        }
    }
    return 0;
}


uint8_t x64_mmu_create_entry (uint64_t *p_entry,
                              x64_mmu_entry_t type,
                              uint64_t addr,
                              uint8_t  pat,
                              uint64_t flag)
{
    uint8_t  ret = -1;

    switch (type) {
    case x64_mmu_pdpte_1g:
        flag |= X64_MMU_PS_BIT;
        flag |= pat ? (1 << 12) : (0 << 12);
        ret = ALIGN_CHECK(addr, 0x40000000); /* 1GB align check */
        break;

    case x64_mmu_pde_2m:
        flag |= X64_MMU_PS_BIT;
        flag |= pat ? (1 << 12) : (0 << 12);
        ret = ALIGN_CHECK(addr, 0x200000);  /* 2MB align check */
        break;

    case x64_mmu_pml5e:
    case x64_mmu_pml4e:
    case x64_mmu_pdpte_pde:
    case x64_mmu_pde_pt:
    case x64_mmu_pte_4k:
        flag &= ~X64_MMU_PS_BIT; /* These types have no PS bit */
        flag |= pat ? (1 << 7) : (0 << 7);
        ret = ALIGN_CHECK(addr, 0x1000);  /* 4KB align check */
        break;
    default:
        for (;;);
    }

    if (!ret){
        *p_entry = addr | flag;
    }

    return ret;
}


/*******************************************************************************
  Memory pool allocation algorithm
*******************************************************************************/

/**
 *           BitMap
 *    0                  63
 * 0  ******** ... ********
 * 1  ******** ... ********
 * 2  ******** ... ********
 * 3  ******** ... ********
 * 4  ...
 */


#define X64_MMU_TB_NUM_CONFIG    260

static table_unit_t table_poor[X64_MMU_TB_NUM_CONFIG]__attribute__((aligned(4096)));
static uint64_t bitmap[X64_MMU_TB_NUM_CONFIG / 64 + 1] = {0};

static uint8_t mmu_search_valid_area (uint64_t *p_position, uint32_t blocks)
{
    uint64_t pix_num;
    uint64_t row;
    uint8_t  column;
    uint8_t pix;
    uint16_t k = 0;

    pix_num = X64_MMU_TB_NUM_CONFIG;

    for (uint64_t pos = 0; pos <= (pix_num - blocks); pos++) {
        /* Convert pixel numbers to coordinates */
        row = pos / 64;
        column = pos % 64;
        pix = (bitmap[row] >> column) & 0x01ull;
        if (pix == 0) {
            for (k = 1; k < blocks; k++) {
                row = (pos + k) / 64;
                column = (pos + k) % 64;
                pix = (bitmap[row] >> column) & 0x01ull;
                if (pix) {
                    /* There is not enough free blocks in this area, jump to the
                     * next pixel to search. */
                    pos += (k + 1);
                    break;
                }
            }
            if (k == blocks) {
                /* Light up the pixel corresponding to the memory block to be
                 * used */
                for (k = 0; k < blocks; k++) {
                    row = (pos + k) / 64;
                    column = (pos + k) % 64;
                    bitmap[row] |= 0x01ull << column;
                }
                *p_position = pos;
                return 0;
            }
        }
    }
    return 1;
}


table_unit_t *x64_mmu_alloc_table (uint32_t blocks)
{
    uint8_t   ret;
    uint64_t  index = 0;

    ret = mmu_search_valid_area(&index, blocks);
    return ret ? ((void*)0) : (table_unit_t *)&table_poor[index];
}


void x64_mmu_free_table (void *p_addr)
{
    uint64_t index;
    uint64_t pix_pos;
    uint64_t addr;
    uint64_t row;
    uint8_t  column;

    addr = (uint64_t)p_addr;
    index = (addr - ((uint64_t)&table_poor[0])) / sizeof(table_unit_t);
    /* The address is misaligned or outside the memory pool range, indicating an
     * illegal parameter */
    if (ALIGN_CHECK(index, 4096) || index >=  X64_MMU_TB_NUM_CONFIG) {
        for (;;);
    }

    pix_pos = index;
    /* Turn off the corresponding pixel */
    row = pix_pos / 64;
    column = pix_pos % 64;
    bitmap[row] &= ~(0x01ull << column);
    return;
}


void mmu_print_tb_poor_bitmap(void)
{
    #include "../x64_driver/x64_serial.h"
    #define PUTCHAR(c) x86_serial_send(0x3f8, c)
    uint64_t row;
    uint8_t  column;

    row = sizeof(table_poor) / sizeof(table_poor[0]) / 64;
    row += ((sizeof(table_poor) / sizeof(table_poor[0])) % 64) ? 1 : 0;
    PUTCHAR('\r'); PUTCHAR('\n');
    PUTCHAR('\r'); PUTCHAR('\n');
    for (uint64_t j = 0; j < X64_MMU_TB_NUM_CONFIG; j++) {
        if (j % 64 == 0) {
            PUTCHAR('\r'); PUTCHAR('\n');
        }
        row = j / 64;
        column = j % 64;
        if ((bitmap[row] >> column) & 0x01ull) {
            PUTCHAR('*');
        } else {
            PUTCHAR('.');
        }
    }
    return;
}


uint8_t x64_mmu_mmap_setup (uint64_t va,
                            uint64_t pa,
                            uint64_t size,
                            uint8_t  cover,
                            uint64_t *(*pfn_alloc)(uint16_t num))
{
    /* Must meet at least 4K alignment requirements */
    if (ALIGN_CHECK(va, 0x1000) && ALIGN_CHECK(pa, 0x1000) && \
        ALIGN_CHECK(size, 0x1000) && (size != 0)) {
        /* Unable to setup address mapping */
        return -1;
    }

    /* size <= 4K */
    if (size <= 0x1000) {

    }

    return 0;
}













