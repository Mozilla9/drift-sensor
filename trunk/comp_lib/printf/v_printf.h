/******************************************************************************/
/* v_printf.h                                                                 */
/******************************************************************************/


#ifndef __V_PRINTF_H
#define __V_PRINTF_H


uint32_t serprintf(const int8_t * format, ...);
void set_trace_func(serprint_func f);
serprint_func get_trace();
void set_trace(serprint_func func);


#endif