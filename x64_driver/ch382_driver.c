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
#include "./x64_serial.h"
#include "../startup/x64_common.h"
#include "../x64_cpu_drivers/x64_apic.h"
#include "../x64_cpu_drivers/x64_pcie.h"


static uint32_t __g_ch382_currunt_clk = CH382_CRYSTAL;

static uint16_t __g_port_base;

/////////////////////////////////////
/////////////////////////////////////
uint32_t g_bdf;
uint32_t g_cap_addr;
void print_ch382()
{
    uint8_t reg;
    reg = inb(__g_port_base + IIR_REG_OFFSET);
    uint32_t data = pcie_atomic_read(g_bdf, g_cap_addr + 5);
    printf("\r\n IIR-reg: %#x, Pending-Reg %#x, Receive: %c \r\n", (uint32_t)reg, data);
    return;
}


 void ch382_interrupt_handler (void *p_param)
 {
     uint8_t     inchar;
     uint8_t     status;
     uint8_t     cnt;
     uint16_t    port_base;
//     printf("CH382 IRQ\r\n");
     port_base = __g_port_base; //(uint16_t)((uint64_t) p_param);
     cnt = 0;
     while (cnt++ < 10) {
         /* Read IIR register */
         status = inb(port_base + IIR_REG_OFFSET);
         if ((status & 0x01) != 0) {
             return;
         }
         switch (status & 0x0e) {
         case IIR_MS_INT:
             inb(port_base + MSR_REG_OFFSET);
//             printf("ch382 IIR_MS_INT\r\n");
             break;

         case IIR_RLS_INT:
             inb(port_base + LSR_REG_OFFSET);
//             printf("ch382 IIR_RLS_INT\r\n");
             break;

         case IIR_THE_INT:
//             printf("ch382 IIR_THE_INT\r\n");
             /*
              *  ... 用于非阻塞发送
              */
             break;

         case IIR_TP_INT:
         case IIR_RAD_INT:
             /* Read all data in fifo */
//             printf("ch382 IIR_RAD_INT\r\n");
             do {
                 inchar = inb(port_base + RBR_REG_OFFSET);
                 /* echo */
                 ch382_serial_send(port_base, inchar);
             } while (inb(port_base + LSR_REG_OFFSET) & LSR_DATARDY_BIT);
             break;

         default:
             break;
         }
     }
     return;
 }


static void ch382_enable_pll(uint32_t port_base)
{
    uint8_t reg;

    /* Disable DLAB */
    reg = inb(port_base + LCR_REG_OFFSET);
    reg &= ~ LCR_DLAB_BIT;
    outb(reg, port_base + LCR_REG_OFFSET);

    reg = inb(port_base + IER_REG_OFFSET);
    reg |= IER_CK2X_BIT;
    outb(reg, port_base + IER_REG_OFFSET);
    __g_ch382_currunt_clk *= 2;
    return;
}

static void ch382_disable_pll(uint16_t port_base)
{
    uint8_t reg;

    /* Disable DLAB */
    reg = inb(port_base + LCR_REG_OFFSET);
    reg &= ~ LCR_DLAB_BIT;
    outb(reg, port_base + LCR_REG_OFFSET);

    reg = inb(port_base + IER_REG_OFFSET);
    reg &= (~IER_CK2X_BIT);
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
    /* Set baudrate */
    divisor = __g_ch382_currunt_clk / 16 / baudrate;
    outb((divisor & 0x00ff), io_port + DLL_REG_OFFSET);
    outb((divisor >> 8), io_port + DLM_REG_OFFSET);
    /* Clear DLAB */
    reg &= (~LCR_DLAB_BIT);
    outb(reg, io_port + LCR_REG_OFFSET);
    return;
}


void ch382_serial_enable(uint16_t port_base)
{
    uint8_t    reg;

    /* Disable DLAB */
    reg = inb(port_base + LCR_REG_OFFSET);
    reg &= (~ LCR_DLAB_BIT);
    outb(reg, port_base + LCR_REG_OFFSET);

    reg = inb(port_base + IER_REG_OFFSET);
    reg &= (~0x0f);
    /* Enable all interrupt */
    reg |= (IER_MODEM_BIT | IER_LINES_BIT |  \
           IER_THRE_BIT | IER_RECV_BIT);
    outb(reg, port_base + IER_REG_OFFSET);

    reg = (MCR_OUT2_BIT);
    outb(reg, port_base + MCR_REG_OFFSET);
}


void ch383_serial_init(uint16_t port_base, uint32_t baudrate, uint8_t parity)
{
    uint8_t    reg;

    /* Reset */
    reg = 0x80;
    outb(reg, port_base + IER_REG_OFFSET);
    while(inb(port_base + IER_REG_OFFSET) & 0x80);

    /* Enable PLL */
    ch382_enable_pll(port_base);

    /* Set baud rate */
    ch382_serial_set_baudrate(port_base, baudrate);

    //reg = (FCR_RECVTG0_BIT | FCR_RECVTG1_BIT | FCR_FIFOEN_BIT);
    reg = 0;
    outb(reg, port_base + FCR_REG_OFFSET);

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

   ch382_serial_enable(port_base);

    return;
}


uint32_t arch_msi_address(uint32_t *data, size_t vector, uint32_t processor, uint8_t edgetrigger, uint8_t deassert) {
    *data = (vector & 0xFF) | (edgetrigger == 1 ? 0 : (1 << 15)) | (deassert == 1 ? 0 : (1 << 14));
    return ((0xFEEull << 20) | (processor << 12) | (3 << 2));
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

        uint32_t command_reg = pcie_atomic_read(bdf, 1);
        command_reg |= ((1 << 4) | (1 << 8) | 7);
        command_reg &= (~ (1 << 10));
        pcie_atomic_write(bdf, 1, command_reg);


//        uint32_t addr = pcie_get_capbility_base_addr(bdf, MSI_X_ID);
//        addr /= 4;
//        g_cap_addr = addr;
//        printf("\r\n---------- MSI-X No.1-----addr: %#x-------\r\n", addr);
//        for(int j = 0; j < 6; j++) {
//            uint32_t regval = pcie_atomic_read(bdf, addr + j);
//            printf("info %d: %#x\r\n", j, regval);
//        }
//        printf("\r\n-----------------------------\r\n");
//
//        uint32_t datareg;
//        uint32_t addreg;
//        addreg = arch_msi_address(&datareg, 80, 0xff, 1, 0);
//
////        pcie_atomic_write(bdf, addr + 1, (0xFEE << 20) | (0 << 12) | (1 << 2));
////        addreg = 0xfee00398;
//        pcie_atomic_write(bdf, addr + 1, addreg);
//
//        pcie_atomic_write(bdf, addr + 2, 0);
//
////        pcie_atomic_write(bdf, addr + 3, 80);
//        pcie_atomic_write(bdf, addr + 3, datareg);
//
//        pcie_atomic_write(bdf, addr + 4, 0x00); //mask
//
//        pcie_atomic_write(bdf, addr + 5, 0x00); //pend
//
//        uint32_t data = pcie_atomic_read(bdf, addr + 0);
//        data &= (~(7 << 20));
////        data |= ((0 << 20) | (1 << 16));
//
//        data |= ((5 << 20) | (1 << 16));
//        pcie_atomic_write(bdf, addr + 0, data);
//
//
//        printf("\r\n---------- MSI-X No.2-----addr: %#x-------\r\n", addr);
//        for(int j = 0; j < 6; j++) {
//            uint32_t regval = pcie_atomic_read(bdf, addr + j);
//            printf("info %d: %#x\r\n", j, regval);
//        }
//        printf("\r\n-----------------------------\r\n");

    } else {
        printf("\r\n\r\n No CH382 Device!!! \r\n");
        return;
    }


    ioapic_intline_config_t int_cfg;
    int_cfg.trigger_mode = TRIGGER_LEVEL_ACTIVE_LOW;
    int_cfg.mask = 0;
    int_cfg.destination_mode = LOGICAL_DESTINATION;  /* use logic id */
    int_cfg.destination = 1;  /* BSP core defualt logic id */
    int_cfg.delivery_mode = DELIVERY_MODE_FIXED;    /* fixed mode */
    int_cfg.vector = 49;
    ioapic_config_int_line(49-32, &int_cfg);

    /* register interrupt handler */
    x64_irq_handler_register(49, ch382_interrupt_handler, NULL); // PassThrough IRQ: 42
//    ioapic_unmask_irq(49);

    ch383_serial_init(io_addr, 115200, 1);

    printf("CH382 REG0: %#x \r\n", inb(io_addr + 0xc0));
    printf("CH382 REG1: %#x \r\n", inb(io_addr + 0xc1));
    printf("CH382 REG2: %#x \r\n", inb(io_addr + 0xc2));
    printf("CH382 REG3: %#x \r\n", inb(io_addr + 0xc3));
    printf("CH382 REG4: %#x \r\n", inb(io_addr + 0xc4));
    printf("CH382 REG5: %#x \r\n", inb(io_addr + 0xc5));
    printf("CH382 REG6: %#x \r\n", inb(io_addr + 0xc6));

    printf("CH382 REG0: %#x \r\n", inb(io_addr + 0xc0));
    printf("CH382 REG1: %#x \r\n", inb(io_addr + 0xc1));
    printf("CH382 REG2: %#x \r\n", inb(io_addr + 0xc2));
    printf("CH382 REG3: %#x \r\n", inb(io_addr + 0xc3));
    printf("CH382 REG4: %#x \r\n", inb(io_addr + 0xc4));
    printf("CH382 REG5: %#x \r\n", inb(io_addr + 0xc5));
    printf("CH382 REG6: %#x \r\n", inb(io_addr + 0xc6));
}



void ch382_serial_send(uint16_t port, char c) {
    port = __g_port_base;
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


char ch382_serial_receive(uint16_t port) {
    char c;
    port = __g_port_base;
    while ((inb(port + LSR_REG_OFFSET) & LSR_DATARDY_BIT) == 0);
    while (inb(port + LSR_REG_OFFSET) & LSR_DATARDY_BIT) {
       c = inb(port + RBR_REG_OFFSET);
    }
    return c;
}









