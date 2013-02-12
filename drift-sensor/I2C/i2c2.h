/*
 * $Id$
 */


#ifndef __I2C2_H
#define __I2C2_H

#include "I2c\i2c_def.h"

void init_i2c2();
int start_i2c2(void * _pt, i2c_data * _pData);
__irq __arm void __interrupt_handler_i2c2();



#endif