/******************************************************************************/
/* ring_buffer.c                                                              */
/******************************************************************************/

#include "data_types.h"
#include "ring_buffer.h"


extern void enter_cs();
extern void exit_cs();



/*
 * Inc tile.
 *
 */
static void inc_tile(__ring_buff * const pRb) {
    if (pRb->tile > pRb->head) {
        if (pRb->tile == pRb->buff_size - 1) {
            if (pRb->head > 0) {
                pRb->tile = 0;
            }
        }
        else {
            pRb->tile++;
        }
    }
    else if (pRb->tile < pRb->head) {
        if (pRb->tile != pRb->head - 1) {
            pRb->tile++;
        }
    }
    else {
        pRb->tile++;
    }
}


/*
 * Inc head.
 *
 */
static void inc_head(__ring_buff * const pRb) {
    if (pRb->head == pRb->buff_size - 1) {
        pRb->head = 0;
    }
    else {
        pRb->head++;
    }

    if (pRb->tile == pRb->head) {
        pRb->tile = 0;
        pRb->head = 0;
    }
}


/*
 * Put byte in ring buff.
 *
 */
static void __put(void * const _pRb, const uint8_t _byte) {
    __ring_buff * const pRb = (__ring_buff *)_pRb;

    pRb->pBuff[pRb->tile] = _byte;
    inc_tile(pRb);
}


/*
 * Get byte from ring buff.
 *
 */
static uint8_t __get(void * const _pRb) {
    __ring_buff * const pRb = (__ring_buff *)_pRb;
    const uint8_t _byte = pRb->pBuff[pRb->head];

    enter_cs();
    inc_head(pRb);
    exit_cs();

    return _byte;
}


/*
 * Get size ring buff.
 *
 */
static uint16_t __size(void * const _pRb) {
    __ring_buff * const pRb = (__ring_buff *)_pRb;

    if (pRb->head > pRb->tile) {
        return pRb->buff_size - pRb->head + pRb->tile;
    }
    else if (pRb->head < pRb->tile) {
        return pRb->tile - pRb->head;
    }
    else {
        return 0;
    }
}


/*
 * Get size ring buff.
 *
 */
static void __reset(void * const _pRb) {
    __ring_buff * const pRb = (__ring_buff *)_pRb;

    pRb->tile = 0;
    pRb->head = 0;
}


/*
 * Create ring buffer
 *
 */
void create_ring_buff(__ring_buff * const _pRb, uint8_t * const _pBuff, const uint16_t _size) {
    _pRb->pBuff = _pBuff;
    _pRb->buff_size = _size;
    _pRb->tile = 0;
    _pRb->head = 0;

    _pRb->reset = __reset;
    _pRb->put = __put;
    _pRb->get = __get;
    _pRb->size = __size;
}
