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


void sdp_insert_char(const uint8_t _char);

uint8_t erase_pg_flash();
uint8_t erase_sett_flash();
uint8_t write_pg_flash(const uint8_t * pData, const uint8_t len);
uint8_t write_sett_flash(const uint8_t * pData, const uint8_t len);
uint8_t invoke_code(const uint8_t * pData, const uint8_t len);
uint8_t invoke_user_cmd(const uint8_t * pData, const uint8_t len);


#endif