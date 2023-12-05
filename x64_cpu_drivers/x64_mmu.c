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

#include <string.h>
#include <stdint.h>
#include "x64_mmu.h"

/**
 * Only support 4 level page, not support PCID, Most access 256 TBytes.
 * condition: (CR0.PG=1, CR4.PAE=1, IA32_EFER.LME=1, CR4.LA57=0, CR4.PCIDE=0)
 */


/*******************************************************************************
  Memory pool allocation and free
********************************************************************************/

/**
 *           BitMap
 *    0                  63
 * 0  ******** ... ********
 * 1  ******** ... ********
 * 2  ******** ... ********
 * 3  ******** ... ********
 * 4  ...
 */

#define MEMSET_MMU_TB(addr)    memset((void *)addr, 0, sizeof(table_unit_t))
#define X64_MMU_TB_NUM_CONFIG    128

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
    if (blocks > pix_num) {
        /* Not allowed! */
        return 1;
    }

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


uint64_t x64_mmu_get_free_blocks (void){

    uint64_t row;
    uint8_t  column;
    uint64_t blocks;

    row = sizeof(table_poor) / sizeof(table_poor[0]) / 64;
    row += ((sizeof(table_poor) / sizeof(table_poor[0])) % 64) ? 1 : 0;
    blocks = 0;

    for (uint64_t j = 0; j < X64_MMU_TB_NUM_CONFIG; j++) {
        row = j / 64;
        column = j % 64;
        if (((bitmap[row] >> column) & 0x01ull) == 0) {
            blocks++;
        }
    }
    return blocks;
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
    if (!ALIGN_CHECK(addr, 4096) || index >=  X64_MMU_TB_NUM_CONFIG) {
        for (;;);
    }

    pix_pos = index;
    /* Turn off the corresponding pixel */
    row = pix_pos / 64;
    column = pix_pos % 64;
    bitmap[row] &= ~(0x01ull << column);
    MEMSET_MMU_TB(p_addr);
    return;
}


void mmu_print_table_poor_bitmap (void)
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


/******************************************************************************
  MMU MAP
*******************************************************************************/

void x64_update_translate_table_addr (uint64_t addr, uint64_t attr_flag)
{
    uint64_t cr3 = 0;

    /* 4K aligned ? */
    if (!ALIGN_CHECK(addr, 4096)) {
        for (;;);
    }
    cr3 = addr | attr_flag;
    __asm__ __volatile__("mov %0, %%cr3"::"r"(cr3));
    return;
}


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

    for (int8_t j = 3; j >= 0; j--) {
        shift = ((j * index_bits) + addr_bits);
        index = (va >> shift) & 0x01ffull;
        if (tb == NULL) {
            return 1;  /* Error */
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


static uint8_t mmu_setup_table (uint64_t va,
                                uint64_t pa,
                                uint64_t flag,
                                mmu_table_type_t type,
                                uint16_t pages,
                                table_unit_t *(*pfn_alloc) (uint32_t blocks))
{
    uint8_t   n;
    uint8_t   shift;
    uint64_t  index;
    uint64_t  *root;
    uint64_t  *tb;
    uint64_t  *addr;
    uint64_t  entry;
    uint64_t  offset;
    const uint8_t addr_bits = 12;
    const uint8_t index_bits = 9;

    if (pages == 0) {
        /* Must return 0! */
        return 0;
    }

    switch (type) {
    case mmu_table_1g:
        n = 2;
        offset = 0x40000000ull;
        break;
    case mmu_table_2m:
        n = 1;
        offset = 0x200000ull;
        break;
    case mmu_table_4k:
        n = 0;
        offset = 0x1000ull;
        /* 4kB page does not have 'PS' bit, replaced by 'PAT' bit */
        flag &= ~X64_MMU_PS_BIT;
        if (flag & X64_MMU_PAT_BIT) {
            flag &= ~X64_MMU_PAT_BIT;
            flag |= (1ull << 7);
        }
        break;
    }

    root = (uint64_t *)x64_get_translate_table_addr();

    for (uint64_t k = 0; k < pages; k++) {
        tb = root;
        /* Start searching from the root address */
        for (int8_t j = 3; j >= n; j--) {
            shift = ((j * index_bits) + addr_bits);
            index = (va >> shift) & 0x01ffull;
            if (tb == NULL) {
                return 1;  /* Error */
            }
            if (j == n) {
                /* Write physical address */
                tb[index] = pa | flag;
                break;
            } else {
                entry = tb[index];
                if (entry == 0) {
                    addr = (uint64_t *)pfn_alloc(1);
                    tb[index] = ((uint64_t)addr) | flag;
                } else {
                    addr = (uint64_t *)(entry & 0x0000fffffffff000ull);
                }
                tb = addr;
            }
        }
        pa += offset;
        va += offset;
    }
    return 0;
}


/*------------------------------------------------------------------------------*/
/**
 * \brief Create a mapping table and update each parameter
 */
#define MMU_SETUP_TABLE(va, pa, flag, type, pages, page_size, map_size, pfn_alloc) \
  do { \
    mmu_setup_table(va, pa, flag, type, pages, pfn_alloc); \
    va += pages * page_size; \
    pa += pages * page_size; \
    map_size -= pages * page_size; \
} while(0)


uint8_t x64_mmu_mmap_setup (uint64_t va,
                            uint64_t pa,
                            uint64_t map_size,
                            uint64_t flag,
                            uint8_t  cover,
                            table_unit_t *(*pfn_alloc) (uint32_t blocks))
{
    uint64_t page4k_num;
    uint64_t page2m_num;
    uint64_t page1g_num;
    uint64_t pa_blocks;
    uint64_t va_blocks;
    uint64_t pa_prefix_byts;
    uint64_t va_prefix_byts;

    /* Must meet at least 4K alignment requirements */
    if (!ALIGN_CHECK(va, 0x1000) || !ALIGN_CHECK(pa, 0x1000) || \
        !ALIGN_CHECK(map_size, 0x1000) || (map_size == 0)) {
        /* Unable to setup address mapping */
        return -1;
    }

    /* size < 2MB */
    if (map_size < 0x200000ull) {
        /* all using 4k pages */
        goto  use_4kpages;
    }

    /* 2MB <= size < 1GB */
    else if (map_size >= 0x200000ull && map_size < 0x40000000ull) {
        if (ALIGN_CHECK(pa, 0x200000) && ALIGN_CHECK(va, 0x200000)){
            /* PA and VA are both 2MB aligned */
            page2m_num = map_size / 0x200000ull;
            MMU_SETUP_TABLE(va, pa, flag, mmu_table_2m, page2m_num, \
                            0x200000ull, map_size, pfn_alloc);

            /* The remaining memory area, all using 4k pages */
            goto  use_4kpages;
        }
        else {
            /* Find how many 4k-pages there are from the nearest 2MB alignment */
            pa_blocks = (((pa / 0x200000ull + 1ull) * 0x200000ull) - pa) / 0x1000;
            va_blocks = (((va / 0x200000ull + 1ull) * 0x200000ull) - va) / 0x1000;
            if (pa_blocks == va_blocks) {
                /**
                 * \note Step:
                 *  1: Use 4KB-pages until 2MB aligned
                 *  2: Now VA and PA are 2MB aligned, using 2MB-pages for those
                 *     areas that satisfy 2MB
                 *  3: Continue to use 4KB pages in the remaining area
                 */
                /* Step 1 */
                MMU_SETUP_TABLE(va, pa, flag, mmu_table_4k, pa_blocks, \
                                0x1000ull, map_size, pfn_alloc);
                /* Step 2 */
                page2m_num = map_size / 0x200000ull;
                MMU_SETUP_TABLE(va, pa, flag, mmu_table_2m, page2m_num, \
                                0x200000ull, map_size, pfn_alloc);
                /* Step 3 */
                goto  use_4kpages;
            }
            else {
                /* Alignment requirements are not met, all using 4k pages */
                goto  use_4kpages;
            }
        }
    }
    /* size >= 1G */
    else {
        if (ALIGN_CHECK(pa, 0x40000000) && ALIGN_CHECK(va, 0x40000000)) {
            /* PA and VA are both 1GB aligned */
            page1g_num = map_size / 0x40000000ull;
            MMU_SETUP_TABLE(va, pa, flag, mmu_table_1g, page1g_num, \
                            0x40000000, map_size, pfn_alloc);

            page2m_num = map_size / 0x200000ull;
            MMU_SETUP_TABLE(va, pa, flag, mmu_table_2m, page2m_num, \
                            0x200000ull, map_size, pfn_alloc);

            /* The remaining memory area, all using 4k pages */
            goto  use_4kpages;
        }
        else {
            /* Find how many bytes there are from the nearest 1GB alignment */
            pa_prefix_byts = (((pa / 0x40000000ull + 1) * 0x40000000ull) - pa);
            va_prefix_byts = (((va / 0x40000000ull + 1) * 0x40000000ull) - va);
            if (pa_prefix_byts == va_prefix_byts) {
                if (!ALIGN_CHECK(pa, 0x200000ull) || \
                    !ALIGN_CHECK(va, 0x200000ull)) {
                    /* 4k aligned! */
                    /* Find how many 4k-pages there are from the nearest 2MB alignment */
                    pa_blocks = (((pa / 0x200000ull + 1) * 0x200000ull) - pa) / 0x1000;
                    va_blocks = (((va / 0x200000ull + 1) * 0x200000ull) - pa) / 0x1000;
                    MMU_SETUP_TABLE(va, pa, flag, mmu_table_4k, pa_blocks, \
                                    0x1000ull, map_size, pfn_alloc);
                }
                page2m_num = map_size / 0x200000ull;
                MMU_SETUP_TABLE(va, pa, flag, mmu_table_2m, page2m_num, \
                                0x200000ull, map_size, pfn_alloc);

                page1g_num = map_size / 0x40000000ull;
                MMU_SETUP_TABLE(va, pa, flag, mmu_table_1g, page1g_num, \
                                0x40000000, map_size, pfn_alloc);

                page2m_num = map_size / 0x200000ull;
                MMU_SETUP_TABLE(va, pa, flag, mmu_table_2m, page2m_num, \
                                0x200000ull, map_size, pfn_alloc);
                /* The remaining memory area, all using 4k pages */
                goto  use_4kpages;
            }
            else {
                /* Alignment requirements are not met, all using 4k pages */
                goto  use_4kpages;
            }
        }
    }
use_4kpages:
    page4k_num = map_size / 0x1000;
    MMU_SETUP_TABLE(va, pa, flag, mmu_table_4k, page4k_num, \
                    0x1000ull, map_size, pfn_alloc);
    return 0;
}



//uint8_t x64_mmu_mmap_setup (uint64_t va,
//                            uint64_t pa,
//                            uint64_t map_size,
//                            uint64_t flag,
//                            uint8_t  cover,
//                            table_unit_t *(*pfn_alloc) (uint32_t blocks))
//{
//    uint64_t page4k_num;
//    uint64_t page2m_num;
//    uint64_t page1g_num;
//    uint64_t pa_blocks;
//    uint64_t va_blocks;
//    uint8_t  page4k_flag;
//
//    page4k_flag = 0;
//
//    /* Must meet at least 4K alignment requirements */
//    if (!ALIGN_CHECK(va, 0x1000) || !ALIGN_CHECK(pa, 0x1000) || \
//        !ALIGN_CHECK(map_size, 0x1000) || (map_size == 0)) {
//        /* Unable to setup address mapping */
//        return -1;
//    }
//
//    /* size < 2MB */
//    if (map_size < 0x200000ull) {
//        page4k_flag = 1;
//        goto  all_use_4kpages;
//    }
//
//    /* 2MB <= size < 1GB */
//    else if (map_size >= 0x200000ull && map_size < 0x40000000ull) {
//        if (ALIGN_CHECK(pa, 0x200000) && ALIGN_CHECK(va, 0x200000)){
//            /* PA and VA are both 2MB aligned */
//            page2m_num = map_size / 0x200000ull;
//            MMU_SETUP_TABLE(va, pa, flag, mmu_table_2m, page2m_num, \
//                            0x200000ull, map_size, pfn_alloc);
//
//            page4k_num = (map_size % 0x200000ull) / 0x1000ull;
//            MMU_SETUP_TABLE(va, pa, flag, mmu_table_4k, page4k_num, \
//                            0x1000ull, map_size, pfn_alloc);
//        }
//        else {
//            /* Find how many 4k-pages there are from the nearest 2MB alignment */
//            pa_blocks = (((pa / 0x200000ull + 1) * 0x200000ull) - pa) / 0x1000;
//            va_blocks = (((va / 0x200000ull + 1) * 0x200000ull) - pa) / 0x1000;
//            if (pa_blocks == va_blocks) {
//                /**
//                 * \note Step:
//                 *  1: Use 4KB-pages until 2MB aligned
//                 *  2: Now VA and PA are 2MB aligned, using 2MB-pages for those
//                 *     areas that satisfy 2MB
//                 *  3: Continue to use 4KB pages in the remaining area
//                 */
//                MMU_SETUP_TABLE(va, pa, flag, mmu_table_4k, pa_blocks, \
//                                0x1000ull, map_size, pfn_alloc);
//
//                page2m_num = map_size / 0x200000ull;
//                MMU_SETUP_TABLE(va, pa, flag, mmu_table_2m, page2m_num, \
//                                0x200000ull, map_size, pfn_alloc);
//
//                page4k_num = map_size / 0x1000ull;
//                MMU_SETUP_TABLE(va, pa, flag, mmu_table_4k, page4k_num, \
//                                0x1000ull, map_size, pfn_alloc);
//            }
//            else {
//                /* All using 4k pages */
//                page4k_flag = 1;
//                goto  all_use_4kpages;
//            }
//        }
//    }
//    /* size >= 1G */
//    else {
//        if (ALIGN_CHECK(pa, 0x40000000) && ALIGN_CHECK(va, 0x40000000)) {
//            /* PA and VA are both 1GB aligned */
//            page1g_num = map_size / 0x40000000ull;
//            MMU_SETUP_TABLE(va, pa, flag, mmu_table_1g, page1g_num, \
//                            0x40000000, map_size, pfn_alloc);
//
//            page2m_num = map_size / 0x200000ull;
//            MMU_SETUP_TABLE(va, pa, flag, mmu_table_2m, page2m_num, \
//                            0x200000ull, map_size, pfn_alloc);
//
//            page4k_num = map_size / 0x1000ull;
//            MMU_SETUP_TABLE(va, pa, flag, mmu_table_4k, page4k_num, \
//                            0x1000ull, map_size, pfn_alloc);
//        }
//        else if (ALIGN_CHECK(pa, 0x200000) && ALIGN_CHECK(va, 0x200000)) {
//            /* Find how many 2M-pages there are from the nearest 1GB alignment */
//            pa_blocks = (((pa / 0x40000000ull + 1) * 0x40000000ull) - pa) / 0x200000ull;
//            va_blocks = (((va / 0x40000000ull + 1) * 0x40000000ull) - pa) / 0x200000ull;
//            if (pa_blocks == va_blocks) {
//                MMU_SETUP_TABLE(va, pa, flag, mmu_table_2m, pa_blocks, \
//                                0x200000ull, map_size, pfn_alloc);
//
//                page1g_num = map_size / 0x40000000ull;
//                MMU_SETUP_TABLE(va, pa, flag, mmu_table_1g, page1g_num, \
//                                0x40000000ull, map_size, pfn_alloc);
//
//                page2m_num = map_size / 0x200000ull;
//                MMU_SETUP_TABLE(va, pa, flag, mmu_table_2m, page2m_num, \
//                                0x200000ull, map_size, pfn_alloc);
//
//                page4k_num = map_size / 0x1000ull;
//                MMU_SETUP_TABLE(va, pa, flag, mmu_table_4k, page4k_num, \
//                                0x1000ull, map_size, pfn_alloc);
//            }
//        } else {
//
//        }
//    }
//
//all_use_4kpages:
//    if (page4k_flag) {
//        page4k_num = map_size / 0x1000;
//        MMU_SETUP_TABLE(va, pa, flag, mmu_table_4k, page4k_num, \
//                        0x1000ull, map_size, pfn_alloc);
//    }
//    return 0;
//}




//uint8_t x64_mmu_mmap_setup (uint64_t va,
//                            uint64_t pa,
//                            uint64_t size,
//                            uint64_t flag,
//                            uint8_t  cover,
//                            table_unit_t *(*pfn_alloc) (uint32_t blocks))
//{
//    uint64_t page4k_num;
//    uint64_t page2m_num;
//    uint64_t page1g_num;
//    uint64_t pa_blocks;
//    uint64_t va_blocks;
//    uint8_t  page4k_flag;
//
//    page4k_flag = 0;
//
//    /* Must meet at least 4K alignment requirements */
//    if (!ALIGN_CHECK(va, 0x1000) || !ALIGN_CHECK(pa, 0x1000) || \
//        !ALIGN_CHECK(size, 0x1000) || (size == 0)) {
//        /* Unable to setup address mapping */
//        return -1;
//    }
//
//    /* size < 2MB */
//    if (size < 0x200000ull) {
//        page4k_flag = 1;
//        goto  all_use_4kpages;
//    }
//
//    /* 2MB <= size < 1GB */
//    else if (size >= 0x200000ull && size < 0x40000000ull) {
//        if (ALIGN_CHECK(pa, 0x200000) && ALIGN_CHECK(va, 0x200000)){
//            /* PA and VA are both 2MB aligned */
//            page2m_num = size / 0x200000ull;
//            mmu_setup_table(va, pa, flag, mmu_table_2m, page2m_num, pfn_alloc);
//            va += page2m_num * 0x200000ull;
//            pa += page2m_num * 0x200000ull;
//            size -= page2m_num * 0x200000ull;
//            page4k_num = (size % 0x200000ull) / 0x1000ull;
//            mmu_setup_table(va, pa, flag, mmu_table_4k, page4k_num, pfn_alloc);
//        }
//        else {
//            /* Find how many 4k-pages there are from the nearest 2MB alignment */
//            pa_blocks = (((pa / 0x200000ull + 1) * 0x200000ull) - pa) / 0x1000;
//            va_blocks = (((va / 0x200000ull + 1) * 0x200000ull) - pa) / 0x1000;
//            if (pa_blocks == va_blocks) {
//                /**
//                 * \note Step:
//                 *  1: Use 4KB-pages until 2MB aligned
//                 *  2: Now VA and PA are 2MB aligned, using 2MB-pages for those
//                 *     areas that satisfy 2MB
//                 *  3: Continue to use 4KB pages in the remaining area
//                 */
//                mmu_setup_table(va, pa, flag, mmu_table_4k, pa_blocks, pfn_alloc);
//                va += va_blocks * 0x1000ull;
//                pa += pa_blocks * 0x1000ull;
//                size -= pa_blocks * 0x1000ull;
//                page2m_num = size / 0x200000ull;
//                mmu_setup_table(va, pa, flag, mmu_table_2m, page2m_num, pfn_alloc);
//                va += page2m_num * 0x200000ull;
//                pa += page2m_num * 0x200000ull;
//                size -= page2m_num * 0x200000ull;
//                page4k_num = size / 0x1000ull;
//                mmu_setup_table(va, pa, flag, mmu_table_4k, page4k_num, pfn_alloc);
//            }
//            else {
//                /* All using 4k pages */
//                page4k_flag = 1;
//                goto  all_use_4kpages;
//            }
//        }
//    }
//    /* size >= 1G */
//    else {
//        if (ALIGN_CHECK(pa, 0x40000000) && ALIGN_CHECK(va, 0x40000000)) {
//            /* PA and VA are both 1GB aligned */
//            page1g_num = size / 0x40000000ull;
//            mmu_setup_table(va, pa, flag, mmu_table_1g, page1g_num, pfn_alloc);
//            va += page1g_num * 0x40000000ull;
//            pa += page1g_num * 0x40000000ull;
//            size -= page1g_num * 0x40000000ull;
//            page2m_num = size / 0x200000ull;
//            mmu_setup_table(va, pa, flag, mmu_table_2m, page2m_num, pfn_alloc);
//            va += page2m_num * 0x200000ull;
//            pa += page2m_num * 0x200000ull;
//            size -= page2m_num * 0x200000ull;
//            page4k_num = size / 0x1000ull;
//            mmu_setup_table(va, pa, flag, mmu_table_4k, page4k_num, pfn_alloc);
//        }
//        else if (ALIGN_CHECK(pa, 0x200000) && ALIGN_CHECK(va, 0x200000)) {
//            /* Find how many 2M-pages there are from the nearest 1GB alignment */
//            pa_blocks = (((pa / 0x40000000ull + 1) * 0x40000000ull) - pa) / 0x200000ull;
//            va_blocks = (((va / 0x40000000ull + 1) * 0x40000000ull) - pa) / 0x200000ull;
//            if (pa_blocks == va_blocks) {
//                mmu_setup_table(va, pa, flag, mmu_table_2m, pa_blocks, pfn_alloc);
//                va += va_blocks * 0x200000ull;
//                pa += pa_blocks * 0x200000ull;
//                size -= pa_blocks * 0x200000ull;
//                page1g_num = size / 0x40000000ull;
//                mmu_setup_table(va, pa, flag, mmu_table_1g, page1g_num, pfn_alloc);
//                va += page1g_num * 0x40000000ull;
//                pa += page1g_num * 0x40000000ull;
//                size -= page1g_num * 0x40000000ull;
//                page2m_num = size / 0x200000ull;
//                mmu_setup_table(va, pa, flag, mmu_table_2m, page2m_num, pfn_alloc);
//                va += page2m_num * 0x200000ull;
//                pa += page2m_num * 0x200000ull;
//                size -= page2m_num * 0x200000ull;
//                page4k_num = size / 0x1000ull;
//                mmu_setup_table(va, pa, flag, mmu_table_4k, page4k_num, pfn_alloc);
//            }
//        } else {
//
//        }
//    }
//
//all_use_4kpages:
//    if (page4k_flag) {
//        page4k_num = size / 0x1000;
//        mmu_setup_table(va, pa, flag, mmu_table_4k, page4k_num, pfn_alloc);
//    }
//    return 0;
//}


void x64_mmu_tlb_enable (void)
{

}


void x64_mmu_tlb_disable (void)
{

}


void x64_mmu_tlb_invalid (void)
{

}




//
//uint8_t x64_mmu_create_entry (uint64_t *p_entry,
//                              x64_mmu_entry_t type,
//                              uint64_t addr,
//                              uint8_t  pat,
//                              uint64_t flag)
//{
//    uint8_t  ret = -1;
//
//    switch (type) {
//    case x64_mmu_pdpte_1g:
//        flag |= X64_MMU_PS_BIT;
//        flag |= pat ? (1 << 12) : (0 << 12);
//        ret = !ALIGN_CHECK(addr, 0x40000000); /* 1GB align check */
//        break;
//
//    case x64_mmu_pde_2m:
//        flag |= X64_MMU_PS_BIT;
//        flag |= pat ? (1 << 12) : (0 << 12);
//        ret = !ALIGN_CHECK(addr, 0x200000);  /* 2MB align check */
//        break;
//
//    case x64_mmu_pml5e:
//    case x64_mmu_pml4e:
//    case x64_mmu_pdpte_pde:
//    case x64_mmu_pde_pt:
//    case x64_mmu_pte_4k:
//        flag &= ~X64_MMU_PS_BIT; /* These types have no PS bit */
//        flag |= pat ? (1 << 7) : (0 << 7);
//        ret = !ALIGN_CHECK(addr, 0x1000);  /* 4KB align check */
//        break;
//    default:
//        for (;;);
//    }
//
//    if (!ret){
//        *p_entry = addr | flag;
//    }
//
//    return ret;
//}
//
//
