/*
 * $Id$
*/


#ifndef __REVISION_H
#define __REVISION_H



#define STR_SHORT_REVISION      "TS024"

const int8_t * get_project_name();
const int8_t * get_revision();
const int8_t * get_rev_date();
const int8_t * get_rev_time();
void get_formatted_rev(int8_t * const str);
void print_revision();


#endif