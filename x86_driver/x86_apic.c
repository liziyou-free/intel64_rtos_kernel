/*
 * x86_apic.c
 *
 *  Created on: 2023年9月13日
 *      Author: liziyou
 */


#include "./x86_apic.h"

#define X86_USED_PIC_8259A


#ifdef X86_USED_PIC_8259A  /* PIC 8259A only support signle processsor. */


void x86_apic_init() {

    /* Initial APIC to a well known state. */
    X86_APIC_LDR = 0xFFFFFFFF;
    X86_APIC_LDR = (X86_APIC_LDR & 0xFFFFFFFF) | 0x01;
    X86_APIC_SPURIOUS_INT = 1 << 8;
}

#elif X86_USED_APIC  /* Support SMP */


#else
    #error "Must select a interrupt controller!"
#endif
