/*
 * $Id$
 */

#ifndef __UART_DEF_H
#define __UART_DEF_H


/****************************** UART constants ********************************/
#define IER_RBR               (0x01)
#define IER_THRE              (0x02)
#define IER_RLS               (0x04)
#define IER_MODEM             (0x08)
#define IER_CTS               (0x80)
#define IER_ABEO              (0x100)
#define IER_ABTO              (0x200)

#define IIR_RLS               (0x03)
#define IIR_RDA               (0x02)
#define IIR_CTI               (0x06)
#define IIR_THRE              (0x01)
#define IIR_MODEM             (0x00)

#define LSR_RDR               (0x01)
#define LSR_OE                (0x02)
#define LSR_PE                (0x04)
#define LSR_FE                (0x08)
#define LSR_BI                (0x10)
#define LSR_THRE              (0x20)
#define LSR_TEMT              (0x40)
#define LSR_RXFE              (0x80)

#define MCR_DTR               (0x01)
#define MCR_RTS               (0x02)
#define MCR_LOOPB             (0x10)
#define MCR_RTSEN             (0x40)
#define MCR_CTSEN             (0x80)

#define MSR_DCTS              (0x01)
#define MSR_DDSR              (0x02)
#define MSR_TRI               (0x04)
#define MSR_DDCD              (0x08)
#define MSR_CTS               (0x10)
#define MSR_DSR               (0x20)
#define MSR_RI                (0x40)
#define MSR_DCD               (0x80)

#define INT_UART0TX_ON    (U0IER_bit.THREIE = 1)
#define INT_UART0TX_OFF   (U0IER_bit.THREIE = 0)
#define INT_UART0RX_ON    (U0IER_bit.RDAIE = 1)
#define INT_UART0RX_OFF   (U0IER_bit.RDAIE = 0)
#define INT_UART0_EN      (U0IER = 0x00000007)
#define INT_UART0_OFF     (U0IER = 0UL)

#define INT_UART1TX_ON    (U1IER_bit.THREIE = 1)
#define INT_UART1TX_OFF   (U1IER_bit.THREIE = 0)
#define INT_UART1RX_ON    (U1IER_bit.RDAIE = 1)
#define INT_UART1RX_OFF   (U1IER_bit.RDAIE = 0)
#define INT_UART1_EN      (U1IER = 0x0000000F)
#define INT_UART1_OFF     (U1IER = 0UL)

#define INT_UART2TX_ON    (U2IER_bit.THREIE = 1)
#define INT_UART2TX_OFF   (U2IER_bit.THREIE = 0)
#define INT_UART2RX_ON    (U2IER_bit.RDAIE = 1)
#define INT_UART2RX_OFF   (U2IER_bit.RDAIE = 0)
#define INT_UART2_EN      (U2IER = 0x00000007)
#define INT_UART2_OFF     (U2IER = 0UL)

#define INT_UART3TX_ON    (U3IER_bit.THREIE = 1)
#define INT_UART3TX_OFF   (U3IER_bit.THREIE = 0)
#define INT_UART3RX_ON    (U3IER_bit.RDAIE = 1)
#define INT_UART3RX_OFF   (U3IER_bit.RDAIE = 0)
#define INT_UART3_EN      (U3IER = 0x00000007)
#define INT_UART3_OFF     (U3IER = 0UL)

#define UART0_FIFO_TRLEVEL    (4)
#define UART1_FIFO_TRLEVEL    (4)
#define UART2_FIFO_TRLEVEL    (4)

#define UART_WORD_8           (3)
#define UART_WORD_7           (2)
#define UART_WORD_6           (1)
#define UART_WORD_5           (0)

#define UART_STOPB_2          (1)
#define UART_STOPB_1          (0)

#define UART_PARITY_EN        (1)
#define UART_PARITY_DIS       (0)

#define UART_ODD_PARITY       (0)
#define UART_EVEN_PARITY      (1)
#define UART_MARK_PARITY      (2)
#define UART_SPACE_PARITY     (3)


/*************************** Prototypes struct for UART ***********************/
typedef struct
{
    uint8_t wordLen   : 2;
    uint8_t stopBit   : 1;
    uint8_t parityEn  : 1;
    uint8_t paritySel : 2;
    uint8_t dummy     : 2;
} __UART_KEY;


typedef union
UART_LINE_KEY
{
    uint8_t key;
    __UART_KEY bitKey;
} __UART_LINE_KEY;



#endif