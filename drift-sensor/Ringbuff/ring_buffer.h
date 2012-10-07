/******************************************************************************/
/* ring_buff.h                                                                */
/******************************************************************************/

#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H


#define RING_BUFF_LEN    100


typedef struct
{
    uint8_t buff[RING_BUFF_LEN];
    volatile uint16_t tile;
    uint16_t head;
} __ring_buff;


void init_ring_buff(__ring_buff * const _pBuff);
void put_in_ring_buff(__ring_buff * const _pBuff, const uint8_t _byte);
uint8_t get_from_ring_buff(__ring_buff * const _pBuff);
bool_t is_ring_buff_empty(__ring_buff * const _pBuff);
uint16_t size_ring_buff(__ring_buff * const _pBuff);


extern __ring_buff * const pointerRingBuff;


#endif