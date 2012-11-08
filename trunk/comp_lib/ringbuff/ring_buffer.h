/******************************************************************************/
/* ring_buff.h                                                                */
/******************************************************************************/

#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H


typedef struct
{
    // data
    uint8_t * pBuff;
    uint16_t buff_size;
    volatile uint16_t tile;
    uint16_t head;

    // interface
    void (* reset)(void * const pRb);
    void (* put)(void * const pRb, const uint8_t _byte);
    uint8_t (* get)(void * const pRb);
    uint16_t (* size)(void * const pRb);
} __ring_buff;


void create_ring_buff(__ring_buff * const _pRb, uint8_t * const _pBuff, const uint16_t _size);


#endif