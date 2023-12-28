/*
 * x86_serial_port.c
 *
 *  Created on: 2023年9月12日
 *      Author: liziyou
 */

#include "x64_serial.h"
#include "../startup/x64_common.h"
#include "../Board/x64_pc_board.h"
#include "../startup/x64_isr_handler.h"
#include "../x64_cpu_drivers/x64_apic.h"


#define __8250_CLK_FRENQ      115200


void serial_interrupt_handler(void *p_param);


/**
 * \brief Enable uart
 */
void X86_uart_enable(uint16_t port_base)
{
    uint8_t    reg;
    /* Disable DLAB */
    reg = inb(port_base + X86_COM_LCR_REG_OFFSET);
    reg &= ~ COM_LCR_DIVISOR_LATCH_ACCESS;
    outb(reg, port_base + X86_COM_LCR_REG_OFFSET);

//    reg = COM_IER_RECEIVER_LINE_STATUS | COM_IER_RECEIVED_DATA_AVAILABLE;
//    outb(reg, port_base + X86_COM_IER_REG_OFFSET);

    reg = COM_FCR_TRIGGER_LEVEL_14BYTE | COM_FCR_CLEAR_TRANSMIT_FIFO |
          COM_FCR_CLEAR_RECEIVE_FIFO | COM_FCR_FIFO_ENABLE;
    outb(reg, port_base + X86_COM_FCR_REG_OFFSET);
    inb(port_base);
    return;
}


/**
 * \brief Disable usart
 */
void X86_uart_disable(uint16_t port_base)
{
    uint8_t  reg;

    /* Wait for the send to complete */
    do {
        reg = inb(port_base + X86_COM_LSR_REG_OFFSET);
    } while (!(reg & COM_LSR_EMPTY_DATA_HOLDING));

    /* Uart disable  */
    reg = inb(port_base + X86_COM_LCR_REG_OFFSET);
    reg &= ~ COM_LCR_DIVISOR_LATCH_ACCESS;
    outb(reg, port_base + X86_COM_LCR_REG_OFFSET);    // Disable DLAB

    reg = 0;
    outb(reg, port_base + X86_COM_IER_REG_OFFSET);    // Disable all interrupts

    return;
}


void serial_init(uint16_t port_base, uint32_t baudrate, uint8_t parity)
{
    uint8_t    reg;
    uint16_t   data;
    void*      p_para;

    /* Set baud rate */
    reg = COM_LCR_DIVISOR_LATCH_ACCESS;
    outb(reg, port_base + X86_COM_LCR_REG_OFFSET);    //set DLAB
    data = __8250_CLK_FRENQ / baudrate;
    reg = (uint8_t)(data & 0x00ff);
    outb(reg, port_base + X86_COM_DLL_REG_OFFSET);    // low byte
    reg = (uint8_t)(data >> 8);
    outb(reg, port_base + X86_COM_DLH_REG_OFFSET);    // high byte
    reg = 0;
    /* set stop bit */
    reg |= COM_LCR_ONE_STOP_BIT;

    /* set parity bit */
    if (parity == 1) {
        reg |= COM_LCR_ODD_PARITY;
    }  else if (parity == 0) {
        reg |= COM_LCR_EVEN_PARITY;
    } else {
        for (;;);
    }

    /* set word length */
    reg |= COM_LCR_EIGHT_WORD_LENGTH;

    outb(reg, port_base + X86_COM_LCR_REG_OFFSET);

    reg = 0;
    reg |= COM_MCR_AUXILIARY_OUTPUT2;
    reg |= COM_MCR_DTR;
    outb(reg, port_base + X86_COM_MCR_REG_OFFSET);

    /* register interrupt handler */
    p_para = (void*)(uint64_t)port_base;
    x64_irq_handler_register(IRQ4, serial_interrupt_handler, p_para);

    /* 使能中断线 */
    //enable_8259A_irq(36);
    ioapic_unmask_irq(IRQ4);
    ioapic_mask_irq(IRQ4);

    X86_uart_enable(port_base);

    return;
}


void x86_serial_send(uint16_t port, char c) {
    while ((inb(port + 5) & 0x20) == 0);
    outb(c, port);
}


void x86_serial_send_str(uint16_t port, char *str) {
    while(*str != '\0') {
        x86_serial_send(port, *str);
        str++;
    }
}


void x86_serial_send_bin(uint16_t port, char *src, uint64_t len) {
    while(len--) {
        x86_serial_send(port, *src++);
    }
}

char x86_serial_receive(uint16_t port) {
    while ((inb(port + 5) & 1) == 0);
    return inb(port);
}


void serial_interrupt_handler (void *p_param)
{
    uint8_t     inchar;
    uint8_t     status;
    uint8_t     cnt;
    uint16_t    port_base;

    port_base =  (uint16_t)((uint64_t) p_param);

    cnt = 0;
    while (cnt++ < 10) {
        /* Read IIR register */
        status = inb(port_base + X86_COM_IIR_REG_OFFSET);
        if ((status & 0x01) != 0) {
            return;
        }
        switch (status & 0x0e) {
        case COM_IIR_MODEM_STATUS_INT:
            inb(port_base + X86_COM_MSR_REG_OFFSET);
            break;

        case COM_IIR_RECEIVER_LINE_STATUS_INT:
            inb(port_base + X86_COM_LSR_REG_OFFSET);

            break;

        case COM_IIR_TRANS_HOLD_EMPTY_INT:
            /*
             *  ... 用于非阻塞发送
             */
            break;

        case COM_IIR_RECEIVED_AVAILABLE_DATA_INT:
        case COM_IIR_TIMEOUT_PENDING_INT:
          /* Read all data in fifo */
            do {
                inchar = inb(port_base + X86_COM_RBR_REG_OFFSET);
                /* echo */
                x86_serial_send(port_base, inchar);
            } while (inb(port_base + X86_COM_LSR_REG_OFFSET) & COM_LSR_DATA_READY);
            break;

        default:
            break;
        }
    }
    return;
}


void X86_uart_tx_trigger (uint16_t com_port, char *buf)
{
    uint8_t    reg;
    /**
     *   ......
     */
    /* Disable DLAB */
    reg = inb(com_port + X86_COM_LCR_REG_OFFSET);
    reg &= ~ COM_LCR_DIVISOR_LATCH_ACCESS;
    outb(reg, com_port + X86_COM_LCR_REG_OFFSET);

    /* Enable trensmitter empty interrupt */
    reg = inb(com_port + X86_COM_IER_REG_OFFSET);
    reg |= COM_IER_TRANSMITTER_HOLD_EMPTY;
    outb(reg, com_port + X86_COM_IER_REG_OFFSET);
    return;
}



/*********************************************************************/

//uint8_t x86_isa_serial_init(){
//   outb(0x0D, PORT + 1);    // Disable all interrupts
//   outb(0x80, PORT + 3);    // Enable DLAB (set baud rate divisor)
//   outb(0x03, PORT + 0);    // Set divisor to 3 (lo byte) 38400 baud
//   outb(0x00, PORT + 1);    //                  (hi byte)
//   outb(0x03, PORT + 3);    // 8 bits, no parity, one stop bit
//   outb(0xC7, PORT + 2);    // Enable FIFO, clear them, with 14-byte threshold
//   outb(0x0B, PORT + 4);    // IRQs enabled, RTS/DSR set
//   outb(0x1E, PORT + 4);    // Set in loopback mode, test the serial chip
//   outb(0xAE, PORT + 0);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
//   // Check if serial is faulty (i.e: not same byte as sent)
//   if(inb(PORT + 0) != 0xAE) {
//      return 1;
//   }
//   // If serial is not faulty set it in normal operation mode
//   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
//   outb(0x0F, PORT + 4);
//
//   x64_irq_handler_register(36, serial_interrupt_handler, NULL);
//
//   /* 使能中断线 */
//   //enable_8259A_irq(36);
//   ioapic_unmask_irq(36);
//   return 0;
//}
