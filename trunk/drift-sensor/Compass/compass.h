/******************************************************************************/
/* compass.h                                                                  */
/******************************************************************************/


#ifndef __COMPASS_H
#define __COMPASS_H


void init_device();
uint32_t get_sys_tick();
uint32_t get_time();

uint32_t get_device_id();
void set_device_id(const uint32_t id);



#endif