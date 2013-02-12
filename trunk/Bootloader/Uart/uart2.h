/*
 * $Id$
 */


#ifndef __UART2_H
#define __UART2_H

#include "Uart\uart_def.h"


void init_uart2(const uint32_t _baudrate, const uint8_t _key);
__irq __arm void __interrupt_handler_uart2();


#endif