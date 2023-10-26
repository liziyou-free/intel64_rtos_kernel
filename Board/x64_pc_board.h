/*
 * X64_pc_borad.h
 *
 *  Created on: 2023年9月13日
 *      Author: liziyou
 */


/* Standard IRQ assigned */

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






