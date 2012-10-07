/******************************************************************************/
/* bin_to_bcd.c                                                               */
/******************************************************************************/

#include "data_types.h"
#include "Lib\bin_to_bcd.h"




/*
 * txt_to_bin
 *
 */
sint16_t txt_to_bin(const int8_t * pTxt) {
    const uint16_t mul[6] = {0, 1, 10, 100, 1000, 10000};
    sint16_t sign = 1;
    sint16_t bin = 0;
    uint16_t len = strlen(pTxt);
    
    if (len == 0 || len > 6) {
        return bin;
    }

    if (pTxt[0] == '-') {
        len--;
        pTxt++;
        sign = -1;
    } else if (pTxt[0] == '+') {
        len--;
        pTxt++;
    }

    if (len == 0) {
        return bin;
    }

    while (len) {
        bin += ((*pTxt++ - '0') * mul[len--]);
    }

    return bin * sign;
}
