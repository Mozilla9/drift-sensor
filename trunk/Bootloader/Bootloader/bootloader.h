/******************************************************************************/
/* bootloader.h                                                               */
/******************************************************************************/


#ifndef __BOOTLOADER_H
#define __BOOTLOADER_H


#define device_vers_tag    "\n\rBootloader v.1.1 12/11/2012 01:03:33\n\r"


void init_device();
uint32_t get_sys_tick();
uint32_t get_time();



#endif