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


static void ch382_interrupt_handler(void *param)
{
    printf("\r\n%%%%%%%%%%%%% CH382 IRQ %%%%%%%%%%%%%%%%%%\r\n");
}


void print_ch382()
{
  uint8_t reg;
  reg = inb(__g_port_base + IIR_REG_OFFSET);
  printf("\r\n IIR-Reg: %#x", reg);
  return;

}


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

    reg = inb(port_base + IER_REG_OFFSET);
    reg |= 0x0f;
    outb(reg, port_base + IER_REG_OFFSET);

    return;
}


#define BDF(b, d, f)    ((b << 8) | (d << 3) | f)


uint64_t arch_msi_address(uint64_t *data, size_t vector, uint32_t processor, uint8_t edgetrigger, uint8_t deassert) {
    *data = (vector & 0xFF) | (edgetrigger == 1 ? 0 : (1 << 15)) | (deassert == 1 ? 0 : (1 << 14));
    return ((0x0FEE << 20) | (processor << 12) | (3 << 2));
}

void ch382_device_init ()
{
    bool     rtn;
    uint8_t  ret;
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
        printf("__g_port_base: %#x\r\n", __g_port_base);

        capbility = pcie_get_capbility(bdf, MSI_ID, &rtn);
        if (rtn == true) {
            printf("Capbility: %#x\r\n", capbility);
        } else {
            printf("Capbility: Error!\r\n");
        }

        uint32_t addr = pcie_get_capbility_base_addr(bdf, MSI_ID);
        addr /= 4;

        printf("\r\n---------- MSI-X No.1-----addr: %#x-------\r\n", addr);
        for(int j = 0; j < 6; j++) {
            uint32_t regval = pcie_atomic_read(bdf, addr + j);
            printf("info %d: %#x\r\n", j, regval);
        }
        printf("\r\n-----------------------------\r\n");

        uint64_t datareg;
        uint64_t addreg;
        addreg = arch_msi_address(&datareg, 80, 1, 1, 1);

//        pcie_atomic_write(bdf, addr + 1, (0xFEE << 20) | (0 << 12) | (1 << 2));
        pcie_atomic_write(bdf, addr + 1, addreg);

        pcie_atomic_write(bdf, addr + 2, 0);
//        pcie_atomic_write(bdf, addr + 3, 80);
        pcie_atomic_write(bdf, addr + 3, datareg);

        pcie_atomic_write(bdf, addr + 4, 0);

        uint32_t data = pcie_atomic_read(bdf, addr + 0);
        data &= (~(7 << 20));
        data |= ((0 << 20) | (1 << 16));
        pcie_atomic_write(bdf, addr + 0, data);


        printf("\r\n---------- MSI-X No.2-----addr: %#x-------\r\n", addr);
        for(int j = 0; j < 6; j++) {
            uint32_t regval = pcie_atomic_read(bdf, addr + j);
            printf("info %d: %#x\r\n", j, regval);
        }
        printf("\r\n-----------------------------\r\n");

    } else {
        printf("\r\n\r\n No CH382 Device!!! \r\n");
        return;
    }

    ch383_serial_init(io_addr, 115200, 1);

    /* register interrupt handler */
    x64_irq_handler_register(80, ch382_interrupt_handler, NULL);

    /* 使能中断线 */
//    ioapic_unmask_irq(80);

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












