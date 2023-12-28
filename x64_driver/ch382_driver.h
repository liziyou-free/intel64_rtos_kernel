/*
 * ch382_driver.h
 *
 *  Created on: 2023年12月18日
 *      Author: freedomli
 */

#ifndef X64_DRIVER_CH382_DRIVER_H_
#define X64_DRIVER_CH382_DRIVER_H_

#define CH382_VENDOR_ID    0x1C00
#define CH382_DEVICE_ID    0x3253

#define CH382_CRYSTAL      22118400  /* 22.1184MHz */

/* Register Define */
#define RBR_REG_OFFSET     0xC0
#define THR_REG_OFFSET     0xC0
#define DLL_REG_OFFSET     0xC0      /* DLAB == 1 */

#define IER_REG_OFFSET     0xC1
# define IER_RESET_BIT     (1 << 7)
# define IER_LOWPOWER_BIT  (1 << 6)
# define IER_CK2X_BIT      (1 << 5)
# define IER_MODEM_BIT     (1 << 3)
# define IER_LINES_BIT     (1 << 2)
# define IER_THRE_BIT      (1 << 1)
# define IER_RECV_BIT      (1 << 0)


#define DLM_REG_OFFSET     0xC1      /* operating conditions :DLAB == 1 */

#define IIR_REG_OFFSET     0xC2
# define IIR_MS_INT        0x00      /* Modem status interrupt */
# define IIR_THE_INT       0x02      /* Transfer hold empty interrupt */
# define IIR_RAD_INT       0x04      /* Received available data interrupt */
# define IIR_RLS_INT       0x06      /* Receive line status interrupt*/
# define IIR_TP_INT        0x0C      /* Timeout pending interrupt */


#define FCR_REG_OFFSET     0xC2
# define FCR_RECVTG1_BIT   (1 << 7)
# define FCR_RECVTG0_BIT   (1 << 6)
# define FCR_TRIG16_BIT    (1 << 5)
# define FCR_TFIFORST_BIT  (1 << 2)
# define FCR_RFIFORST_BIT  (1 << 1)
# define FCR_FIFOEN_BIT    (1 << 0)


#define LCR_REG_OFFSET     0xC3
# define LCR_DLAB_BIT      (1 << 7)
# define LCR_BREAKEN_BIT   (1 << 6)
# define LCR_ODD_PARITY    (0 << 4)
# define LCR_EVEN_PARITY   (1 << 4)
# define LCR_PARITY_EN     (1 << 3)
# define LCR_ONE_STOP      (0 << 2)
# define LCR_TWO_STOP      (1 << 2)
# define LCR_FIVE_BITS     (0 << 0)
# define LCR_SIX_BITS      (1 << 0)
# define LCR_SEVEN_BITS    (2 << 0)
# define LCR_EIGHT_BITS    (3 << 0)


#define MCR_REG_OFFSET     0xC4
# define MCR_HALF_BIT      (1 << 7)
# define MCR_AFE_BIT       (1 << 5)
# define MCR_LOOP_BIT      (1 << 4)
# define MCR_OUT2_BIT      (1 << 3)
# define MCR_OUT1_BIT      (1 << 2)
# define MCR_RTS_BIT       (1 << 1)
# define MCR_DTR_BIT       (1 << 0)

#define LSR_REG_OFFSET     0xC5
# define LSR_RFIFOERR_BIT  (1 << 7)
# define LSR_TEMT_BIT      (1 << 6)
# define LSR_THRE_BIT      (1 << 5)
# define LSR_BREAKINT_BIT  (1 << 4)
# define LSR_FRAMEERR_BIT  (1 << 3)
# define LSR_PARERR_BIT    (1 << 2)
# define LSR_OVERR_BIT     (1 << 1)
# define LSR_DATARDY_BIT   (1 << 0)

#define MSR_REG_OFFSET     0xC6
#define SCR_REG_OFFSET     0xC7


void ch382_device_init ();
void ch382_serial_send(uint16_t port, char c);
void ch382_serial_send_str(uint16_t port, char *str);
char ch382_serial_receive(uint16_t port);


#endif /* X64_DRIVER_CH382_DRIVER_H_ */


