/*
 * x86_common.h
 *
 *  Created on: 2023年9月13日
 *      Author: liziyou
 */

#ifndef X64_DRIVER_X86_COMMON_H_
#define X64_DRIVER_X86_COMMON_H_

#include <stdint.h>
#include <sys/cdefs.h>

typedef  unsigned char       u8;
typedef  unsigned short int  u16;
typedef  unsigned long int   u32;

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

#endif /* X64_DRIVER_X86_COMMON_H_ */
