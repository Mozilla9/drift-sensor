/******************************************************************************/
/* i2c0.h                                                                     */
/******************************************************************************/


#ifndef __I2C1_H
#define __I2C1_H

#include "I2c\i2c_def.h"

void init_i2c1();
int start_i2c1(void * _pt, i2c_data * _pData);
__irq __arm void __interrupt_handler_i2c1();



#endif