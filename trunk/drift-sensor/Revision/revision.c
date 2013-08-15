/*
 * $Id$
*/


#include "data_types.h"
#include "Uart\v_printf.h"
#include "svnrev.h"
#include "revision.h"



static const int8_t * const pName = "DriftSensor";

static const int8_t * const pRevision = SVN_REVSTR;
static const int8_t * const pDate = SVN_REVDATE;
static const int8_t * const pTime = __TIME__;



/*
 *
 *
 */
const int8_t * get_project_name() {
    return pName;
}


/*
 *
 *
 */
const int8_t * get_revision() {
    return pRevision;
}


/*
 *
 *
 */
const int8_t * get_rev_date() {
    return pName;
}


/*
 *
 *
 */
const int8_t * get_rev_time() {
    return pName;
}


/*
 *
 *
 */
void get_formatted_rev(int8_t * const str) {
    strcpy(str, pName);
    strcat(str, " rev. ");
    strcat(str, pRevision);
    strcat(str, " ");
    strcat(str, pDate);
    strcat(str, " ");
    strcat(str, pTime);
}


/*
 *
 *
 */
void print_revision() {
    int8_t buff[64];

    get_formatted_rev(buff);
    DEBUG_PRINTF(buff);
    DEBUG_PRINTF("\r\n");
}

