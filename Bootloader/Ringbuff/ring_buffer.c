/******************************************************************************/
/* ring_buffer.c                                                              */
/******************************************************************************/

#include "data_types.h"
#include "Core\core.h"
#include "Ringbuff\ring_buffer.h"


/*
 * Inc tile.
 *
 */
static void inc_tile(__ring_buff * const _pBuff) {
    if (_pBuff->tile > _pBuff->head) {
        if (_pBuff->tile == _pBuff->buff_size - 1) {
            if (_pBuff->head > 0) {
                _pBuff->tile = 0;
            }
        }
        else {
            _pBuff->tile++;
        }
    }
    else if (_pBuff->tile < _pBuff->head) {
        if (_pBuff->tile != _pBuff->head - 1) {
            _pBuff->tile++;
        }
    }
    else {
        _pBuff->tile++;
    }
}


/*
 * Inc head.
 *
 */
static void inc_head(__ring_buff * const _pBuff) {
    if (_pBuff->head == _pBuff->buff_size - 1) {
        _pBuff->head = 0;
    }
    else {
        _pBuff->head++;
    }

    if (_pBuff->tile == _pBuff->head) {
        _pBuff->tile = 0;
        _pBuff->head = 0;
    }
}


/*
 * Put byte in ring buff.
 *
 */
void put_in_ring_buff(__ring_buff * const _pBuff, const uint8_t _byte) {
    _pBuff->pBuff[_pBuff->tile] = _byte;
    inc_tile(_pBuff);
}


/*
 * Get byte from ring buff.
 *
 */
uint8_t get_from_ring_buff(__ring_buff * const _pBuff) {
    uint8_t _byte = _pBuff->pBuff[_pBuff->head];

    enter_cs();
    inc_head(_pBuff);
    exit_cs();

    return _byte;
}


/*
 * Test by empty ring buff.
 *
 */
bool_t is_ring_buff_empty(__ring_buff * const _pBuff) {
    enter_cs();
    const bool_t decision = _pBuff->head == _pBuff->tile ? TRUE_T : FALSE_T;
    exit_cs();

    return decision;
}


/*
 * Get size ring buff.
 *
 */
uint16_t size_ring_buff(__ring_buff * const _pBuff) {
    if (_pBuff->head > _pBuff->tile) {
        return _pBuff->buff_size - _pBuff->head + _pBuff->tile;
    }
    else if (_pBuff->head < _pBuff->tile) {
        return _pBuff->tile - _pBuff->head;
    }
    else {
        return 0;
    }
}
