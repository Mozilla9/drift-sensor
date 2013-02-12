/*
 * $Id$
 */


#ifndef __I2C0_H
#define __I2C0_H

#include "I2c\i2c_def.h"

void init_i2c0();
int start_i2c0(void * _pt, i2c_data * _pData);
__irq __arm void __interrupt_handler_i2c0();



#endif