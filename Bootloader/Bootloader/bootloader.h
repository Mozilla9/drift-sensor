/******************************************************************************/
/* bootloader.h                                                               */
/******************************************************************************/


#ifndef __BOOTLOADER_H
#define __BOOTLOADER_H


#define device_vers_tag    "\n\rBootloader v.1.0 09/11/2012 03:43:33\n\r"


void init_device();
uint32_t get_sys_tick();
uint32_t get_time();



#endif