/*
 * $Id$
 */


#ifndef __LIS3DH_H
#define __LIS3DH_H

#include "Lis3dh\lis3dh_def.h"

void init_lis3dh();
void enable_default_lis3dh();

void read_acc_lis3dh();
vector * get_acc_lis3dh();

bool_t get_int1_lis3dh();
bool_t get_int2_lis3dh();

void set_sa0_lis3dh();
void clr_sa0_lis3dh();

void set_cs_lis3dh();
void clr_cs_lis3dh();



#endif