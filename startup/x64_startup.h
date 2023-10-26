/*
 * x86_startup.h
 *
 *  Created on: 2023年9月8日
 *      Author: liziyou
 */

#ifndef STARTUP_X64_STARTUP_H_
#define STARTUP_X64_STARTUP_H_


/* Set Global descriptors table number */
#define X86_GDT_NUM  256

/* Set Global descriptors table number */
#define X86_IDT_NUM  256



/* Kernel use 'Flat' memory model */
#define  KERNEL_CODE_ADDRESS_START    0x00
#define  KERNEL_CODE_ADDRESS_SIZE     ((4u*1024*1024*1024)-1) /* 4GB */

#define  KERNEL_DATA_ADDRESS_START    0x00
#define  KERNEL_DATA_ADDRESS_SIZE     ((4u*1024*1024*1024)-1) /* 4GB */

#define  KERNEL_STACK_ADDRESS_START   0x00
#define  KERNEL_STACK_ADDRESS_SIZE    (1u*1024u)-1 /* 4GB */

#define  KERNEL_ISR_ADDRESS_START     0x00
#define  KERNEL_ISR_ADDRESS_SIZE      ((4u*1024*1024*1024)-1) /* 4GB */

#define  KERNEL_CODE_GDT_INDEX        1
#define  KERNEL_DATA_GDT_INDEX        2
#define  KERNEL_STACK_GDT_INDEX       3
#define  KERNEL_INT_EXCEPT_GDT_INDEX  4

                              )


#endif /* STARTUP_X64_STARTUP_H_ */



