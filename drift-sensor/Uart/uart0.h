/*
 * $Id$
 */


#ifndef __UART0_H
#define __UART0_H

#include "Uart\uart_def.h"


void waite_tx_all_uart0();
void init_uart0(const uint32_t _baudrate, const uint8_t _key);
__irq __arm void __interrupt_handler_uart0();


#endif