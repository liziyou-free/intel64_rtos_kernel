/*
 * X64_pc_borad.h
 *
 *  Created on: 2023年9月13日
 *      Author: liziyou
 */


#define X64_USED_APIC_INTC    1

/*******************************************************************************
 * Interrupt number
*******************************************************************************/
/**
 *\note IRQ assigned
 */
#if defined(X64_USED_APIC_INTC)
#define IRQ0    32 /* System timer (cannot be changed) */
#define IRQ1    33 /* Keyboard controller (cannot be changed) */
#define IRQ2    34 /* 8254 timer or Cascaded signals from IRQs 8~15 */
#define IRQ3    35 /* Serial port controller for COM2/4 */
#define IRQ4    36 /* serial port controller for COM1/3 */
#define IRQ5    37 /* LPT port 2 or sound card */
#define IRQ6    38 /* Floppy disk controller */
#define IRQ7    39 /* LPT port 1 or sound card */
#define IRQ8    40 /* Real time clock (RTC) */
#define IRQ9    41 /* Open interrupt/available or SCSI host adapter */
#define IRQ10   42 /* Open interrupt/available or SCSI or NIC */
#define IRQ11   43 /* Open interrupt/available or SCSI or NIC */
#define IRQ12   44 /* Mouse on PS/2 connector */
#define IRQ13   45 /* Math coprocessor */
#define IRQ14   46 /* Primary ATA channel */
#define IRQ15   47 /* Secondary ATA channel */
#elif defined(X64_USED_8259_INTC)
enum {
    X64_TIMER8254_IRQ = 0,
    X64_KEYBOARD_IRQ,
    X64_CASCADE_FOR_CONTROLLER_2,
    X64_SERIAL_PORT2_IRQ,
    X64_SERIAL_PORT1_IRQ,
    X64_PARALLEL_PORT2_SOUNDCARD_IRQ,
    X64_FLOPPY_DISKETTE_IRQ,
    X64_PARALLEL_PORT1_IRQ,
    X64_REALTIME_CLOCK_IRQ,
    X64_REDIRECTED_TO_IRQ2,
    /* 10~12 not assigned */
    X64_MATH_COPROCESSOR_IRQ = 13,
    X64_HARDDISK_CONTROLLER1_IRQ,
    X64_HARDDISK_CONTROLLER2_IRQ
};
#else
  #error "Please select interrupt controller!"
#endif


/**
 * \brief X64 port address def
 */
#define X64_PORT_COM1      0x3F8
#define X64_PORT_COM2      0x2F8
#define X64_PORT_COM3      0x3E8
#define X64_PORT_COM4      0x2E8
#define X64_PORT_COM5      0x5F8
#define X64_PORT_COM6      0x4F8
#define X64_PORT_COM7      0x5E8
#define X64_PORT_COM8      0x4E8

/* Intel 8254 timer1 */
#define X64_PORT_TIMER1                 0x040






