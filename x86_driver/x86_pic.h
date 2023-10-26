/*
 * x86_i8259.h
 *
 *  Created on: 2023年9月13日
 *      Author: liziyou
 */

#ifndef X86_DRIVER_X86_PIC_H_
#define X86_DRIVER_X86_PIC_H_

#include <stdint.h>


/* i8259A PIC registers */
#define PIC_MASTER_CMD      0x20
#define PIC_MASTER_IMR      0x21
#define PIC_MASTER_ISR      PIC_MASTER_CMD
#define PIC_MASTER_POLL     PIC_MASTER_ISR
#define PIC_MASTER_OCW3     PIC_MASTER_ISR
#define PIC_SLAVE_CMD       0xa0
#define PIC_SLAVE_IMR       0xa1

/* i8259A PIC related value */
#define PIC_CASCADE_IR      2
#define MASTER_ICW4_DEFAULT 0x01
#define SLAVE_ICW4_DEFAULT  0x01
#define PIC_ICW4_AEOI       2

/**
  ICW2 is used as the start interrupt vector to specify the interrupt request in
  this 8259A, and bit0:3 must be set to 0; Therefore, its initial interrupt vec-
  tor must be a multiple of 8. For example,if the design of our OS says that the
  8 interrupt requests from Master 8259A are placed at the 32nd (0) to 39th pos-
  ition of the IDT, we should set ICW2 to 0x20.In this way, when an IRQ is rece-
  ived on this 8259A in the future, the lower 3 bits are automatically filled w-
  ith the IRQ number. For example, if it receives an IRQ6 and automatically fill
  6 into the last three bits, the resulting vector number is 0x26. After receiv
  the second INTA signal from the CPU, the 8259A will place the generated vector
   number on the Data Bus.
*/
#define I8259A_IRQ_BASE    0x20




/**
 * \brief    PIC disbale interrupt
 *
 * \details  The specified interrupt number was disbale
 *
 * \irq[in]  interrupt number
 *
 * \retval   none
 */
void disable_8259A_irq(uint8_t irq);


/**
 * \brief    PIC enbale interrupt
 *
 * \details  The specified interrupt number was enabled
 *
 * \irq[in]  interrupt number
 *
 * \retval   none
 */
void enable_8259A_irq(uint8_t irq);


/**
 * \brief   PIC init
 *
 * \details Intel Programable interrupt controller 8259A init
 *
 * \eoi[in] AEOI mode enable.
 *
 * \retval  none
 */
void x86_pic_i8259a_init(uint8_t eoi);


#endif /* X86_DRIVER_X86_PIC_H_ */



