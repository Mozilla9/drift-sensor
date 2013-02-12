/*
 * $Id$
 */


#ifndef __COMPASS_H
#define __COMPASS_H



#define PROTO_TYPE_CONSOLE   0
#define PROTO_TYPE_SDP       1


void init_device();
uint32_t get_sys_tick();
uint32_t get_time();

uint32_t get_device_id();
void set_device_id(const uint32_t id);

uint32_t get_proto_type();
void set_proto_type(const uint32_t _proto);


#endif