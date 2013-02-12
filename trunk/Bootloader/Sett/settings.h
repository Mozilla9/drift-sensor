/*
 * $Id$
 */

#ifndef __SETTINGS_H
#define __SETTINGS_H


#include "Fmem\fmem.h"


#define DEF_SETT_FLAG       0xAA55AA55


void init_app_settings();
uint32_t read_app_settings(const __FMEM_DATA * const pReadData);


#endif