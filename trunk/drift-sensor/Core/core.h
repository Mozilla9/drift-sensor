/******************************************************************************/
/* core.h                                                                     */
/******************************************************************************/


#ifndef __CORE_H
#define __CORE_H


#include "Core\core_def.h"



void init_wdt();
void init_timer0();
void init_timer1();
void init_core();
__arm void enter_cs();
__arm void exit_cs();
__arm void clear_wdt();
__arm void reset_device();


#endif