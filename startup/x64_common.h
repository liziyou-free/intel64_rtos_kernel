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

#ifndef X64_DRIVER_X86_COMMON_H_
#define X64_DRIVER_X86_COMMON_H_

#include <stdint.h>
#include <sys/cdefs.h>

typedef  unsigned char       u8;
typedef  unsigned short int  u16;
typedef  unsigned long int   u32;


static inline uint64_t x64_read_msr(uint32_t msr)
{
  uint32_t low;
  uint32_t high;

  __asm__ __volatile__("rdmsr" : "=a" (low), "=d" (high) : "c" (msr));
  return low | ((unsigned long)high << 32);
}


static inline void x64_write_msr(uint32_t msr, uint64_t val)
{
  __asm__ __volatile__ ("wrmsr"
                       : /* no output */
                       : "c" (msr), "a" (val), "d" (val >> 32)
                       : "memory");
}


#define BUILDIO(bwl, bw, type)                      \
static __always_inline void __out##bwl(type value, u16 port)        \
{                                   \
    __asm__ __volatile__("out" #bwl " %" #bw "0, %w1"           \
                         : : "a"(value), "Nd"(port));           \
}                                   \
                                    \
static __always_inline type __in##bwl(u16 port)             \
{                                   \
    type value;                         \
    __asm__ __volatile__("in" #bwl " %w1, %" #bw "0"            \
                         : "=a"(value) : "Nd"(port));           \
    return value;                           \
}

BUILDIO(b, b, u8)
BUILDIO(w, w, u16)
BUILDIO(l,  , u32)
#undef BUILDIO

#define inb __inb
#define inw __inw
#define inl __inl
#define outb __outb
#define outw __outw
#define outl __outl


/* transplant */
#define atomic_read32(reg)  (*((volatile uint32_t*)(reg)))
#define atomic_write32(reg, value)  ((*((volatile uint32_t*)(reg))) = (value))

//static __always_inline  uint32_t atomic_read32(uint64_t reg) {
//  uint32_t value;
//  __asm__ __volatile__("mov %1, %0"
//                       : "=r"(value)
//                       : "0"(*((uint32_t*)reg)));
//  return value;
//}
//
//
//static __always_inline void atomic_write32(uint64_t reg, uint32_t value) {
//  __asm__ __volatile__("mov %0, %1"
//                       :
//                       : "r"(value), "o"(*((uint32_t*)reg))
//                       );
//  return;
//}

#endif /* X64_DRIVER_X86_COMMON_H_ */
