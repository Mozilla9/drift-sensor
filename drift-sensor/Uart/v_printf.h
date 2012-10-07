/******************************************************************************/
/* v_printf.h                                                                 */
/******************************************************************************/


#ifndef __V_PRINTF_H
#define __V_PRINTF_H


__arm uint32_t serprintf(const int8_t * format, ...);
void set_trace_func(serprint_func f);
serprint_func get_trace();


#endif