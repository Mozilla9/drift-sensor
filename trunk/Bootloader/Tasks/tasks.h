/******************************************************************************/
/* tasks.h                                                                    */
/******************************************************************************/


#ifndef __TASKS_H
#define __TASKS_H


#include "Tasks\tasks_def.h"


void init_tasks();
void run_tasks();
uint32_t get_en_tasks();

void enable_update_task();
void disable_update_task();



#endif