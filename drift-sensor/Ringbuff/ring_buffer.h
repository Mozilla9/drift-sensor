/******************************************************************************/
/* ring_buff.h                                                                */
/******************************************************************************/

#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H


typedef struct
{
    uint8_t * const pBuff;
    volatile uint16_t tile;
    uint16_t head;
    const uint16_t buff_size;
} __ring_buff;


void init_ring_buff(__ring_buff * const _pBuff);
void put_in_ring_buff(__ring_buff * const _pBuff, const uint8_t _byte);
uint8_t get_from_ring_buff(__ring_buff * const _pBuff);
bool_t is_ring_buff_empty(__ring_buff * const _pBuff);
uint16_t size_ring_buff(__ring_buff * const _pBuff);


#endif