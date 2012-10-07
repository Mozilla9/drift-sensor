/******************************************************************************/
/* average.c                                                                  */
/******************************************************************************/

#include "data_types.h"
#include "Core\core.h"
#include "Filter\average.h"



/*
 * Init average filter
 *
 */
void init_average_filter(average_buff * const _pData) {
    _pData->fill_index = 0;
    _pData->wind_size = AVERAGE_DEF_WINDOW_SIZE;
    _pData->average_type = AVERAGE_DEF_TYPE;

    for (uint16_t i = 0; i < AVERAGE_MAX_WINDOW_SIZE; i++) {
        _pData->buff[i] = 0;
    }
}


/*
 * Set window size
 *
 */
void set_wind_size_average_filter(average_buff * const _pData, const uint16_t _wind_size) {
    if (_wind_size > AVERAGE_MAX_WINDOW_SIZE) {
        _pData->wind_size = AVERAGE_DEF_WINDOW_SIZE;
    } else if (_wind_size < 2) {
        _pData->wind_size = AVERAGE_DEF_WINDOW_SIZE;
    } else {
        _pData->wind_size = _wind_size;
    }

    _pData->fill_index = 0;
    for (uint16_t i = 0; i < AVERAGE_MAX_WINDOW_SIZE; i++) {
        _pData->buff[i] = 0;
    }
}


/*
 * Set average type
 *
 */
void set_average_type_filter(average_buff * const _pData, const uint16_t _type) {
    if (_type == AVERAGE_SIMPLE) {
        _pData->average_type = AVERAGE_SIMPLE;
    } else if(_type == AVERAGE_WEIGHT) {
        _pData->average_type = AVERAGE_WEIGHT;
    } else if(_type == AVERAGE_MOD_PIT) {
        _pData->average_type = AVERAGE_MOD_PIT;
    } else {
        _pData->average_type = AVERAGE_DEF_TYPE;
    }
}


/*
 * Add sample in filter
 *
 */
void add_sample_in_filter(average_buff * const _pData, const sint16_t _sample) {
    if (_pData->fill_index < _pData->wind_size) {
        _pData->buff[_pData->fill_index++] = _sample;
    }
    else {
        uint16_t i = 0, k = 1;
        while (k < _pData->wind_size) {
            _pData->buff[i++] = _pData->buff[k++];
        }

        _pData->buff[i] = _sample;
    }
}


/*
 * Get last sample
 *
 */
sint16_t get_last_sample(average_buff * const _pData) {
    return _pData->buff[_pData->fill_index > 0 ?_pData->fill_index - 1 : 0];
}


/*
 * Get average
 *
 */
sint16_t get_average(average_buff * const _pData) {
    sint32_t summ = 0;

    for (uint16_t i = 0; i < _pData->wind_size; i++) {
        summ += _pData->buff[i];
    }

    return summ / _pData->wind_size;
}


/*
 * Get weighted average
 * 30% with koeff 0.5
 * 30% with koeff 0.8
 * 30% with koeff 1.0
 *
 */
sint16_t get_weighted_average(average_buff * const _pData) {
    sint32_t summ = 0;

    for (uint16_t i = 0; i < _pData->wind_size; i++) {
        sint32_t value = _pData->buff[i];
        if (i < (_pData->wind_size / 3)) {
            summ += (value / 2);
        }
        else if (i < ((_pData->wind_size * 2) / 3)) {
            summ += ((value * 4) / 5);
        }
        else {
            summ += value;
        }
    }

    return summ / _pData->wind_size;
}


/*
 * Get pit average
 *
 */
sint16_t get_pit_average(average_buff * const _pData, const sint16_t _med) {
    sint32_t summ = 0;

    for (uint16_t i = 0; i < _pData->wind_size; i++) {
        summ += (_pData->buff[i] > _med ? _pData->buff[i] - _med : _med - _pData->buff[i]);
    }

    return summ / _pData->wind_size;
}
