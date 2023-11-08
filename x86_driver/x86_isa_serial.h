/*
 * x86_isa_serial.h
 *
 *  Created on: 2023年9月14日
 *      Author: liziyou
 */

#ifndef X86_DRIVER_X86_ISA_SERIAL_H_
#define X86_DRIVER_X86_ISA_SERIAL_H_

#include <stddef.h>

uint8_t x86_isa_serial_init();
void x86_serial_send(char c);
void x86_serial_send_str(char *str);
char x86_serial_receive();

#endif /* X86_DRIVER_X86_ISA_SERIAL_H_ */
