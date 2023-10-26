/*
 * x86_i8259.c
 *
 *  Created on: 2023年9月13日
 *      Author: liziyou
 */

#ifndef X86_DRIVER_X86_I8259_C_
#define X86_DRIVER_X86_I8259_C_

#include "x86_pic.h"
#include "./x86_common.h"

static int i8259A_auto_eoi = -1;

/*
 * This contains the irq mask for both 8259A irq controllers,
 */
static unsigned int cached_irq_mask = 0xffff;

#define cached_master_mask  (cached_irq_mask)
#define cached_slave_mask   (cached_irq_mask >> 8)


/**
 * \brief    PIC disbale interrupt
 *
 * \details  The specified interrupt number was disbale
 *
 * \irq[in]  interrupt number
 *
 * \retval   none
 */
void disable_8259A_irq(uint8_t irq)
{
    unsigned int mask;

    irq -= I8259A_IRQ_BASE;
    mask = 1 << irq;
    cached_irq_mask |= mask;
    if (irq & 8)
        outb(cached_slave_mask, PIC_SLAVE_IMR);
    else
        outb(cached_master_mask, PIC_MASTER_IMR);
    return;
}


/**
 * \brief    PIC enbale interrupt
 *
 * \details  The specified interrupt number was enabled
 *
 * \irq[in]  interrupt number
 *
 * \retval   none
 */
void enable_8259A_irq(uint8_t irq)
{
    unsigned int mask;

    irq -= I8259A_IRQ_BASE;
    mask = ~(1 << irq);
    cached_irq_mask &= mask;
    if (irq & 8)
        outb(cached_slave_mask, PIC_SLAVE_IMR);
    else
        outb(cached_master_mask, PIC_MASTER_IMR);
    return;
}


/**
 * \brief   PIC init
 *
 * \details Intel Programable interrupt controller 8259A init
 *
 * \eoi[in] AEOI mode enable.
 *
 * \retval  none
 */
void x86_pic_i8259a_init(uint8_t eoi) {

    i8259A_auto_eoi = eoi;

    outb(0xff, PIC_MASTER_IMR); /* mask all of 8259A-1 */
    outb(0xff, PIC_SLAVE_IMR);  /* mask all of 8259A-2 */

    /*
     * outb - this has to work on a wide range of PC hardware.
     */
    outb(0x11, PIC_MASTER_CMD);   /* ICW1: select 8259A-1 init */
    outb(I8259A_IRQ_BASE + 0, PIC_MASTER_IMR);    /* ICW2: 8259A-1 IR0 mapped to I8259A_IRQ_BASE + 0x00 */
    outb(1U << PIC_CASCADE_IR, PIC_MASTER_IMR);   /* 8259A-1 (the master) has a slave on IR2 */
    if (eoi)   /* master does Auto EOI */
        outb(MASTER_ICW4_DEFAULT | PIC_ICW4_AEOI, PIC_MASTER_IMR);
    else        /* master expects normal EOI */
        outb(MASTER_ICW4_DEFAULT, PIC_MASTER_IMR);

    outb(0x11, PIC_SLAVE_CMD);    /* ICW1: select 8259A-2 init */
    outb(I8259A_IRQ_BASE + 8, PIC_SLAVE_IMR); /* ICW2: 8259A-2 IR0 mapped to I8259A_IRQ_BASE + 0x08 */
    outb(PIC_CASCADE_IR, PIC_SLAVE_IMR);  /* 8259A-2 is a slave on master's IR2 */
    outb(SLAVE_ICW4_DEFAULT, PIC_SLAVE_IMR); /* (slave's support for AEOI in flat mode is to be investigated) */
//    if (eoi)
//        /*
//         * In AEOI mode we just have to mask the interrupt
//         * when acking.
//         */
//        i8259A_chip.irq_mask_ack = disable_8259A_irq;
//    else
//        i8259A_chip.irq_mask_ack = mask_and_ack_8259A;

    outb(cached_master_mask, PIC_MASTER_IMR); /* restore master IRQ mask */
    outb(cached_slave_mask, PIC_SLAVE_IMR);   /* restore slave IRQ mask */
    return;
}

#endif /* X86_DRIVER_X86_I8259_C_ */
