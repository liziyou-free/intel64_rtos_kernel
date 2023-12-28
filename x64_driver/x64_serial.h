/*
 * x86_isa_serial.h
 *
 *  Created on: 2023年9月14日
 *      Author: liziyou
 */

#ifndef X86_DRIVER_X86_ISA_SERIAL_H_
#define X86_DRIVER_X86_ISA_SERIAL_H_

#include <stddef.h>
#include <stdint.h>

/* Register offset */
#define X86_COM_THR_REG_OFFSET    0
#define X86_COM_RBR_REG_OFFSET    0
#define X86_COM_DLL_REG_OFFSET    0
#define X86_COM_IER_REG_OFFSET    1
#define X86_COM_DLH_REG_OFFSET    1
#define X86_COM_IIR_REG_OFFSET    2
#define X86_COM_FCR_REG_OFFSET    2
#define X86_COM_LCR_REG_OFFSET    3
#define X86_COM_MCR_REG_OFFSET    4
#define X86_COM_LSR_REG_OFFSET    5
#define X86_COM_MSR_REG_OFFSET    6
#define X86_COM_SR_REG_OFFSET     7


/* IER register */
#define COM_IER_MODEM_STATUS                   (1 << 3)
#define COM_IER_RECEIVER_LINE_STATUS           (1 << 2)
#define COM_IER_TRANSMITTER_HOLD_EMPTY         (1 << 1)
#define COM_IER_RECEIVED_DATA_AVAILABLE        (1 << 0)

/* IIR register */
// bit[7:6]
#define COM_IIR_CHIP_NO_FIFO                    (0 << 6)
#define COM_IIR_FIFO_NOT_FUNCTIONING            (2 << 6)
#define COM_IIR_FIFIO_ENABLE                    (3 << 6)
// bit[3:1]
#define COM_IIR_MODEM_STATUS_INT                (0 << 1)
#define COM_IIR_TRANS_HOLD_EMPTY_INT            (1 << 1)
#define COM_IIR_RECEIVED_AVAILABLE_DATA_INT     (2 << 1)
#define COM_IIR_RECEIVER_LINE_STATUS_INT        (3 << 1)
#define COM_IIR_TIMEOUT_PENDING_INT             (6 << 1)
// bit[0]
#define COM_IIR_SELF_HAVE_INTERRUPT             (1 << 0)


/* FCR register */
// bit[7:6]
#define COM_FCR_TRIGGER_LEVEL_1BYTE             (0 << 6)
#define COM_FCR_TRIGGER_LEVEL_4BYTE             (1 << 6)
#define COM_FCR_TRIGGER_LEVEL_8BYTE             (2 << 6)
#define COM_FCR_TRIGGER_LEVEL_14BYTE            (3 << 6)
// bit[3]
#define COM_FCR_DMA_MODE_SELECT                 (1 << 3)
// bit[2]
#define COM_FCR_CLEAR_TRANSMIT_FIFO             (1 << 2)
// bit[1]
#define COM_FCR_CLEAR_RECEIVE_FIFO              (1 << 1)
// bit[0]
#define COM_FCR_FIFO_ENABLE                     (1 << 0)


/* LCR register */
// bit[7]
#define COM_LCR_DIVISOR_LATCH_ACCESS            (1 << 7)
// bit[6]
#define COM_LCR_SET_BREAK_ENABLE                (1 << 6)
// bit[5:3]
#define COM_LCR_NO_PARITY                       (0 << 3)
#define COM_LCR_ODD_PARITY                      (1 << 3)
#define COM_LCR_EVEN_PARITY                     (3 << 3)
#define COM_LCR_MARK_BIT                        (5 << 3)
#define COM_LCR_SPACE_BIT                       (7 << 3)
// bit[2]
#define COM_LCR_ONE_STOP_BIT                    (0 << 2)
#define COM_LCR_TWO_STOP_BIT                    (1 << 2)
// bit[1:0]
#define COM_LCR_FIVE_WORD_LENGTH                (0 << 0)
#define COM_LCR_SIX_WORD_LENGTH                 (1 << 0)
#define COM_LCR_SEVEN_WORD_LENGTH               (2 << 0)
#define COM_LCR_EIGHT_WORD_LENGTH               (3 << 0)


/* MCR registers */
// bit[5]
#define COM_MCR_AUTO_FLOW_CONTROL               (1 << 5)
// bit[4]
#define COM_MCR_LOOPBACK_MODE                   (1 << 4)
// bit[3]
#define COM_MCR_AUXILIARY_OUTPUT2               (1 << 3)
// bit[2]
#define COM_MCR_AUXILIARY_OUTPUT1               (1 << 2)
// bit[1]
#define COM_MCR_RTS                             (1 << 1)
// bit[0]
#define COM_MCR_DTR                             (1 << 0)


/* LSR registers */
// bit[7]
#define COM_LSR_ERROR_IN_RECEIVE_FIFO           (1 << 7)
// bit[6]
#define COM_LSR_EMPTY_DATA_HOLDING              (1 << 6)
// bit[5]
#define COM_LSR_EMPTY_TRANSMITTER_HOLDING       (1 << 5)
// bit[4]
#define COM_LSR_BREAK_INTERRUPT                 (1 << 4)
// bit[3]
#define COM_LSR_FRAMING_ERROR                   (1 << 3)
// bit[2]
#define COM_LSR_PARITY_ERROR                    (1 << 2)
// bit[1]
#define COM_LSR_OVERRUN_ERROR                   (1 << 1)
// bit[0]
#define COM_LSR_DATA_READY                      (1 << 0)


/* MSR registers */
// bit[7]
#define COM_MSR_CARRIER_DETECT                  (1 << 7)
// bit[6]
#define COM_MSR_RING_INDICATOR                  (1 << 6)
// bit[5]
#define COM_MSR_DATA_SET_READy                  (1 << 5)
// bit[4]
#define COM_MSR_CLEAR_TO_SEND                   (1 << 4)
// bit[3]
#define COM_MSR_DELTA_DATA_CARRIER_DETECT       (1 << 3)
// bit[2]
#define COM_MSR_TRAILING_EDGE_RING_INDICATOR    (1 << 2)
// bit[1]
#define COM_MSR_DELTA_DATA_SET_READY            (1 << 1)
// bit[0]
#define COM_MSR_DELTA_CLEAR_TO_SEND             (1 << 0)


/**
 * \brief Intel 8250 serial device init.
 *
 * \param port_base[in] port address
 * \param baudrate[in]  baud_rate <= 115200
 * \param parity[in]    1: odd parity  0:even parity
 *
 * \retval none
 */
void serial_init(uint16_t port_base, uint32_t baudrate, uint8_t parity);

void x86_serial_send(uint16_t port, char c);
void x86_serial_send_str(uint16_t port, char *str);
void x86_serial_send_bin(uint16_t port, char *src, uint64_t len);
char x86_serial_receive(uint16_t port);
void X86_uart_enable(uint16_t port_base);
void X86_uart_disable(uint16_t port_base);

#endif /* X86_DRIVER_X86_ISA_SERIAL_H_ */
