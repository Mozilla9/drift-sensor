/*
 * $Id$
 */


#ifndef __HANDLERS_H
#define __HANDLERS_H



int32_t execute (int32_t argc, const int8_t * const * argv);
int8_t ** complet (int32_t argc, const int8_t * const * argv);
void sigint ();


#endif