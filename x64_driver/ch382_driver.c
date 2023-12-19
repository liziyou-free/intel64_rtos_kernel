/*
 * ch382_driver.c
 *
 *  Created on: 2023年12月18日
 *      Author: freedomli
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "./ch382_driver.h"
#include "../startup/x64_common.h"
#include "../x64_cpu_drivers/x64_pcie.h"


static uint32_t __g_ch382_currunt_clk = CH382_CRYSTAL;

static uint16_t __g_port_base;


static void ch382_enable_pll(uint32_t port_base)
{
    uint8_t reg;
    reg = inb(port_base + IER_REG_OFFSET);
    reg |= IER_CK2X_BIT;
    outb(reg, port_base + IER_REG_OFFSET);
    __g_ch382_currunt_clk *= 2;
    return;
}

static void ch382_disable_pll(uint16_t port_base)
{
    uint8_t reg;
    reg = inb(port_base + IER_REG_OFFSET);
    reg &= ~IER_CK2X_BIT;
    outb(reg, port_base + IER_REG_OFFSET);
    __g_ch382_currunt_clk = CH382_CRYSTAL;
    return;
}


void ch382_serial_set_baudrate(uint16_t io_port, uint32_t baudrate)
{
    uint8_t  reg;
    uint16_t divisor;

    /* Setup DLAB */
    reg = inb(io_port + LCR_REG_OFFSET);
    outb((reg | LCR_DLAB_BIT), io_port + LCR_REG_OFFSET);
    divisor = __g_ch382_currunt_clk / 16 / baudrate;
    outb((divisor & 0x00ff), io_port + DLL_REG_OFFSET);
    outb((divisor >> 8), io_port + DLM_REG_OFFSET);
    /* Clear DLAB */
    reg &= ~LCR_DLAB_BIT;
    outb(reg, io_port + LCR_REG_OFFSET);
    return;
}


void ch383_serial_init(uint16_t port_base, uint32_t baudrate, uint8_t parity)
{
    uint8_t    reg;

    /* Enable PLL */
    ch382_enable_pll(port_base);

    /* Set baud rate */
    ch382_serial_set_baudrate(port_base, baudrate);

    /* set stop bit */
    reg = 0;
    reg |= LCR_ONE_STOP;
    /* set parity bit */
    if (parity == 1) {
        reg |= LCR_ODD_PARITY;
    }  else if (parity == 0) {
        reg |= LCR_EVEN_PARITY;
    } else {
        for (;;);
    }
    /* set word length */
    reg |= LCR_EIGHT_BITS;
    outb(reg, port_base + LCR_REG_OFFSET);

    reg = 0;
    reg |= MCR_OUT2_BIT;
    reg |= MCR_DTR_BIT;
    outb(reg, port_base + MCR_REG_OFFSET);

    /* register interrupt handler */
//    p_para = (void*)(uint64_t)port_base;
//    x64_irq_handler_register(IRQ4, serial_interrupt_handler, p_para);

    /* 使能中断线 */
    //enable_8259A_irq(36);
    //ioapic_unmask_irq(IRQ4);

    //X86_uart_enable(port_base);

    return;
}


#define BDF(b, d, f)    ((b << 8) | (d << 3) | f)


void ch382_device_init ()
{
    bool     rtn;
    uint8_t  ret;
    uint16_t reg;
    uint16_t io_addr;
    uint32_t capbility;
    uint32_t bdf;
    cfg_header_type0_t cfg;

    ret = pcie_search_dev(CH382_VENDOR_ID, CH382_DEVICE_ID, &bdf, &cfg);
    if (ret == 0)
    {
        printf("###### Cap_Pointer:%#x ...\r\n", cfg.capability_pointer);
        io_addr = cfg.base_addr[0] & 0xfffe;
        __g_port_base = io_addr;

        capbility = pcie_get_capbility(bdf, MSI_ID, &rtn);
        if (rtn == true) {
            printf("Capbility: %#x\r\n", capbility);
        } else {
            printf("Capbility: Error!\r\n");
        }


        printf("__g_port_base: %#x\r\n", __g_port_base);
    } else {
        printf("\r\n\r\n No CH382 Device!!! \r\n");
        return;
    }

    ch383_serial_init(io_addr, 115200, 1);
    return;
}


void ch382_serial_send(uint16_t port, char c) {
    while ((inb(port + LSR_REG_OFFSET) & LSR_THRE_BIT) == 0);
    outb(c, port + THR_REG_OFFSET);
    return;
}


void ch382_serial_send_str(uint16_t port, char *str) {

    port = __g_port_base;
    while(*str != '\0') {
        ch382_serial_send(port, *str);
        str++;
    }
}












