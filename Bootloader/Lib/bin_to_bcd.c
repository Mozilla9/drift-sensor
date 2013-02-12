/*
 * $Id$
 */

#include "data_types.h"
#include "Lib\bin_to_bcd.h"




/*
 * txt to bin
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


/*
 * nibble to bin
 *
 */
uint8_t nibble_to_bin(const uint8_t nibble) {
    if (nibble >= '0' && nibble <= '9') {
        return nibble - 0x30;
    } else if (nibble >= 'A' && nibble <= 'F') {
        return nibble - 0x37;
    } else if (nibble >= 'a' && nibble <= 'f') {
        return nibble - 0x57;
    } else {
        return 0;
    }
}


/*
 * hex to bin
 *
 */
uint16_t hex_to_bin(const int8_t * pHex) {
    uint16_t bin = 0;
    uint8_t tbin = 0;
    uint16_t len = strlen(pHex);

    if (len == 0 || len > 4) {
        return bin;
    }

    while (len) {
        tbin = nibble_to_bin(*pHex++);

        if (tbin != 0) {
            bin <<= 4;
            bin |= tbin;
        }
        len--;
    }

    return bin;
}
