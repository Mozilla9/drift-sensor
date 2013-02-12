/*
 * $Id$
 */


#include "data_types.h"
#include "Core\core.h"
#include "Uart\uart0.h"


/*
 * Lpc23xx implementation for UART0
 *
 */
__arm uint32_t uart_putchar(const uint8_t c) {
    static uint32_t count = 16;

    if ((U0LSR & LSR_THRE) || (U0LSR & LSR_TEMT)) {
        count = 16;
    }

    if (count) {
        U0THR = c;
        count--;
        return 0;
    }
    return 1;
}