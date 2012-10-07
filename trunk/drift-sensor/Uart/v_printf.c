/* printf.c: Output format for a controller environment.
 *
 * This implements a limited subset of printf(), with only the
 * format features typically needed in an embedded environment.
 *
 * It's compatible with the AVR, with the format string read from
 * program space / flash, and all parameters from data space / RAM.
 * See 'S' for an optional exception for constant strings.
 *
 * Before "fixing"/extending this implementation, please consider
 * the target use on very small machine.
 *
 * This implementation is only printf().  There is no option for
 * sprintf() or fprintf().  It queues the characters directly to the
 * output device with serial_putch(), with the only additional space
 * used a small buffer for building %d/%x output.
 *
 * Originally written 2010-2011 by Donald Becker and William Carlson
 * for the QAR EV motor controller project.  Released under GPLv2.1
 * Contact the authors for use under other terms.
 */

#include <stdarg.h>
#include "data_types.h"
#include "Core\core.h"
#include "Uart\uart0.h"
#include "Uart\v_printf.h"


static int8_t buf[12];
static serprint_func pTrace = 0;


/*
 * Lpc23xx implementation for UART0
 *
 */
__arm static uint32_t uart_putchar(const uint8_t c) {
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


__arm static void inline serial_putch(uint8_t c) {
    while(uart_putchar(c) != 0);    // Returns -1 if full queue.
                                    // We busy-wait.
}


/*
 * Sleazy versions of number to string conversions.  These convert VAL
 * to a string in the local buffer with the specified number of
 * digits. Leading zeros are included, overflow is truncated and the
 * string is not terminated.
 *
 */
__arm static void u32_to_uart(uint32_t val, sint32_t digits) {
    int8_t * str = buf + sizeof(buf) - 1;
    *str-- = 0;

    do {
        *str-- = (uint8_t)(val % 10) + '0';
        val = val / 10;
    }
    while (--digits > 0 || val > 0);

    do {
        serial_putch(*++str);
    }
    while (str < buf + sizeof(buf) - 1);
}


/*
 * Same thing in hex.
 *
 */
__arm static void uint_to_hex_uart(unsigned val, uint8_t digits) {
    uint8_t nibble;
    int8_t * str = buf + sizeof(buf) - 1;
    *str-- = 0;

    while (digits-- > 0) {
        nibble = val & 0x000f;

        if (nibble >= 10)
            nibble = (nibble - 10) + 'A';
        else
            nibble = nibble + '0';

        *str-- = nibble;
        val = val >> 4;
    }
    ++str;

    do {
        serial_putch(*str);
    }
    while (*++str);
}



/*
 * Printf() implementation for serial uart
 *
 *
 */
__arm uint32_t serprintf(const int8_t * format, ...) {
    va_list args;
    uint8_t c, j = 0;
    va_start(args, format);

    while ((c = *format++)) {
        if (j) {
            switch(c) {
                case 's': {
                    uint8_t * str = va_arg(args, char *);
                    do {
                        j = *str++;
                        if (j == 0) break;      // Clears precision
                        serial_putch(j);
                    }
                    while (1);
                    break;
                }

                case 'l':
                    j = sizeof(long);
                    continue;

                case 'u':
                    u32_to_uart(j == sizeof(long) ? va_arg(args, long) :
                                va_arg(args, int), j);
                    break;

                case 'd': {
                    int32_t val = (j == sizeof(long) ? va_arg(args, long) :
                                   va_arg(args, int));
                    if (val < 0) {
                        serial_putch('-');
                        val = -val;
                    }
                    u32_to_uart(val, j);
                    break;
                }

                case 'c':
                    serial_putch(va_arg(args, int));
                    break;

                case 'x':
                case 'X': {
                    if (j == 1) {               // Do "%#x" rather than "%x"
                        //serial_putch('0');
                        //serial_putch(c);        // Match case 0x or 0X
                    }
                    uint_to_hex_uart(j == sizeof(long) 
                        ? va_arg(args, long) : va_arg(args, int), j);
                    break;
                }

                default:
                    serial_putch(c);
                    break;
            }

            j = 0;
        }
        else {
            if (c == '%') {
                const int8_t d = *format;

                if ('0' <= d && d <= '9') {
                    format++;
                    j = d - '0';
                }
                else {
                    j = 1;
                }
            }
            else {
                // Automatic CR+LF expansion saves space.
                if (c == '\n')
                    serial_putch('\r');
                serial_putch(c);
            }
        }
    }
    va_end(args);
    return 0;                    // Incorrect, but this is a sleazy version.
}



/*
 * Enable trace function
 *
 */
void set_trace_func(serprint_func f) {
    enter_cs();
    pTrace = f;
    exit_cs();
}


/*
 * Get trace func
 *
 */
serprint_func get_trace() {
    return pTrace;
}

