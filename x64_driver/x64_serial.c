/*
 * x86_serial_port.c
 *
 *  Created on: 2023年9月12日
 *      Author: liziyou
 */

#include "x64_serial.h"

#include "../startup/x64_common.h"
#include "../x64_cpu_drivers/x64_apic.h"


#define PORT    0x3f8          // COM1


uint8_t x86_isa_serial_init(){
   outb(0x0D, PORT + 1);    // Disable all interrupts
   outb(0x80, PORT + 3);    // Enable DLAB (set baud rate divisor)
   outb(0x03, PORT + 0);    // Set divisor to 3 (lo byte) 38400 baud
   outb(0x00, PORT + 1);    //                  (hi byte)
   outb(0x03, PORT + 3);    // 8 bits, no parity, one stop bit
   outb(0xC7, PORT + 2);    // Enable FIFO, clear them, with 14-byte threshold
   outb(0x0B, PORT + 4);    // IRQs enabled, RTS/DSR set
   outb(0x1E, PORT + 4);    // Set in loopback mode, test the serial chip
   outb(0xAE, PORT + 0);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
   // Check if serial is faulty (i.e: not same byte as sent)
   if(inb(PORT + 0) != 0xAE) {
      return 1;
   }
   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
   outb(0x0F, PORT + 4);

   /* 使能中断线 */
   //enable_8259A_irq(36);
   ioapic_unmask_irq(36);
   return 0;
}


void x86_serial_send(char c) {
    while ((inb(PORT + 5) & 0x20) == 0);
    outb(c, PORT);
}


void x86_serial_send_str(char *str) {
    while(*str != '\0') {
        x86_serial_send(*str);
        str++;
    }
}


char x86_serial_receive() {
    while ((inb(PORT + 5) & 1) == 0);
    return inb(PORT);
}

