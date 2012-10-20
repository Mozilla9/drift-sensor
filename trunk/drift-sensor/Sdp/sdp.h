/******************************************************************************/
/* sdp.h                                                                      */
/******************************************************************************/


#ifndef __SDP_H
#define __SDP_H

#include "Sdp\sdp_def.h"



typedef struct
{
    uint8_t buff[SDP_BUFF_LEN];
    uint16_t len;
} __sdp_data;


#endif