/******************************************************************************/
/* settings.h                                                                 */
/******************************************************************************/

#ifndef __SETTINGS_H
#define __SETTINGS_H


#include "Fmem\fmem.h"


#define DEF_SETT_FLAG       0xAA55AA55


void init_app_settings();
void write_app_settings(const __FMEM_DATA * const pWriteData);
uint32_t read_app_settings(const __FMEM_DATA * const pReadData);


#endif