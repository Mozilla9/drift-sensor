/*
 * $Id$
*/


#include "data_types.h"
#include "Uart\v_printf.h"
#include "revision.h"



static const int8_t * const pName = "DriftSensor BL";

static const int8_t * const pHiRevision = "1";
static const int8_t * const pLoRevision = "$Revision$";
static const int8_t * const pDate = __DATE__;
static const int8_t * const pTime = __TIME__;
static int8_t rev_buff[9] = {0,0,0,0,0,0,0,0,0};



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
    if (rev_buff[0] == 0) {
        uint16_t i = 0;

        // Hi
        rev_buff[i++] = pHiRevision[0];
        rev_buff[i++] = pHiRevision[1];
        rev_buff[i++] = '.';

        // Lo
        while (pLoRevision[i + 8] != ' ' && pLoRevision[i + 8] != 0) {
            rev_buff[i] = pLoRevision[i + 8];
            i++;
        }
        rev_buff[i] = 0;
    }
    return rev_buff;
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
    strcat(str, get_revision());
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

