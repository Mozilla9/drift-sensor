/*
 * $Id$
 */

#ifndef __CAN_HANDLERS_H
#define __CAN_HANDLERS_H


#pragma pack(1)
typedef struct
{
    uint32_t  pgn;          // param id
    uint8_t   pos;          // pos in bits
    uint8_t   len;          // len in bits
    const int8_t * pLabel;  // text name
    ulong64_t data;         // data
} __can_param_t;
#pragma pack()


#pragma pack(1)
typedef union
{
    ulong64_t  data64;
    uint8_t   data8[8];
    uint16_t  data16[4];
    uint32_t  data32[2];
} __UNION_UINT64;
#pragma pack()




void poll_can_msg();
void init_can_j1939();
__can_param_t * get_next_can_data(const uint16_t idx);



#endif