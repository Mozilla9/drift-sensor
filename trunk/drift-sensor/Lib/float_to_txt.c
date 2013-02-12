/*
 * $Id$
 */

#include "data_types.h"
#include "Lib\float_to_txt.h"



/*
 * Split float to components (mantissa and exp)
 *
 */
void split_float_to_comp(const float32_t _ff, uint32_t components[]) {
    const uint32_t mull = 10000;
    const uint32_t sign = _ff >= 0.0 ? 0 : 1;
    const float32_t ff = _ff >= 0.0 ? _ff : (-1.0 * _ff);
    const uint32_t mnt = (uint32_t) ff;
    const uint32_t exp = (uint32_t)((ff - mnt) * mull);

    components[0] = sign;
    components[1] = mnt;
    components[2] = exp;
}
