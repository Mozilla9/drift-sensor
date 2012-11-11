/******************************************************************************/
/* sdp.h                                                                      */
/******************************************************************************/


#ifndef __SDP_H
#define __SDP_H

#include "sdp_def.h"



typedef struct
{
    uint8_t buff[SDP_BUFF_LEN];
    uint16_t len;
} __sdp_data;


void sdp_insert_char(const uint8_t _char);


#endif