/******************************************************************************/
/* tasks.h                                                                    */
/******************************************************************************/


#ifndef __TASKS_H
#define __TASKS_H

#include "Tasks\tasks_def.h"


void init_tasks();
void run_tasks();

void enable_calibr_task();
void disable_calibr_task();

void enable_acc_task();
void disable_acc_task();

void enable_acc_matrix_task();
void disable_acc_matrix_task();



#endif