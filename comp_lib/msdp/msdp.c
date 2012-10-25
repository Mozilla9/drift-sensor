/******************************************************************************/
/* msdp.c - serial download protocol (master)                                 */
/******************************************************************************/

#include "data_types.h"
#include "msdp_def.h"
#include "msdp.h"



/*
 * Calc crc
 *
 */
static uint8_t calc_crc_sdp(const uint8_t * pData, uint8_t len) {
    uint8_t crc = 0;

    do {
        crc += *pData++;
    } while (--len);

    return 0 - crc;
}


/*
 * nibble to hex
 * Mode : 'X' - 'A' - 'F',
 *        'x' - 'a' - 'f'
 *
 */
static uint8_t nibble_to_hex(const uint8_t nibble, const uint8_t _mode) {
    if (nibble >= 0 && nibble <= 9) {
        return nibble + 0x30;
    } else if (nibble >= 0xA && nibble <= 0xF) {
        return (_mode == 'x') ? nibble + 0x57 : nibble + 0x37;
    } else {
        return 0;
    }
}


/*
 * Erase pg-flash
 *
 */
void erase_pg_flash_msdp(__msdp_out_data * const pOutData) {
    static const uint8_t erase_cmd_c[] = {SDP_HEADER_x07, SDP_HEADER_x0E, 0x01, 0x34, 0x33, 0x99};

    pOutData->len = 0;
    while (pOutData->len < sizeof(erase_cmd_c)) {
        pOutData->pBuff[pOutData->len] = erase_cmd_c[pOutData->len];
        pOutData->len++;
    }
}


/*
 * Erase sett-flash
 *
 */
void erase_sett_flash_msdp(__msdp_out_data * const pOutData) {
    static const uint8_t erase_cmd_a[] = {SDP_HEADER_x07, SDP_HEADER_x0E, 0x01, 0x34, 0x31, 0x9B};

    pOutData->len = 0;
    while (pOutData->len < sizeof(erase_cmd_a)) {
        pOutData->pBuff[pOutData->len] = erase_cmd_a[pOutData->len];
        pOutData->len++;
    }
}


/*
 * Write data in pg-flash
 *
 */
void write_pg_flash_msdp(__msdp_out_data * const pOutData, const __msdp_wr_data * const pWrData) {
    pOutData->len = 0;

    if (pWrData->len > 0 && pWrData->len < 27) {
        // header
        pOutData->pBuff[pOutData->len++] = SDP_HEADER_x07;
        pOutData->pBuff[pOutData->len++] = SDP_HEADER_x0E;

        // data len
        pOutData->pBuff[pOutData->len++] = pWrData->len + 4;  // cmd(1byte) + addr(3bytes) + data

        // cmd 'W'
        pOutData->pBuff[pOutData->len++] = 0x35;
        pOutData->pBuff[pOutData->len++] = 0x37;

        // addr
        uint32_t addr = pWrData->addr;
        for (uint16_t i = 0; i < 6; i++) {
            pOutData->pBuff[pOutData->len++] = nibble_to_hex(addr & 0xF, 'X');
            addr >>= 4;
        }

        // data
        for (uint16_t i = 0; i < pWrData->len; i++) {
            pOutData->pBuff[pOutData->len++] = nibble_to_hex(pWrData->pBuff[i] & 0xF, 'X');
            pOutData->pBuff[pOutData->len++] = nibble_to_hex(pWrData->pBuff[i] >> 4, 'X');
        }

        // crc
        pOutData->pBuff[pOutData->len] = calc_crc_sdp(pOutData->pBuff + 3, pOutData->len - 3);
        pOutData->len++;
    }
}


/*
 * Write data in sett-flash
 *
 */
void write_sett_flash_msdp(__msdp_out_data * const pOutData, const __msdp_wr_data * const pWrData) {
    pOutData->len = 0;

    if (pWrData->len > 0 && pWrData->len < 27) {
        // header
        pOutData->pBuff[pOutData->len++] = SDP_HEADER_x07;
        pOutData->pBuff[pOutData->len++] = SDP_HEADER_x0E;

        // data len
        pOutData->pBuff[pOutData->len++] = pWrData->len + 4;  // cmd(1byte) + addr(3bytes) + data

        // cmd 'E'
        pOutData->pBuff[pOutData->len++] = 0x34;
        pOutData->pBuff[pOutData->len++] = 0x35;

        // addr
        uint32_t addr = pWrData->addr;
        for (uint16_t i = 0; i < 6; i++) {
            pOutData->pBuff[pOutData->len++] = nibble_to_hex(addr & 0xF, 'X');
            addr >>= 4;
        }

        // data
        for (uint16_t i = 0; i < pWrData->len; i++) {
            pOutData->pBuff[pOutData->len++] = nibble_to_hex(pWrData->pBuff[i] & 0xF, 'X');
            pOutData->pBuff[pOutData->len++] = nibble_to_hex(pWrData->pBuff[i] >> 4, 'X');
        }

        // crc
        pOutData->pBuff[pOutData->len] = calc_crc_sdp(pOutData->pBuff + 3, pOutData->len - 3);
        pOutData->len++;
    }
}


/*
 * Invoke code
 *
 */
void invoke_code_msdp(__msdp_out_data * const pOutData, const uint32_t address) {
    pOutData->len = 0;

    // header
    pOutData->pBuff[pOutData->len++] = SDP_HEADER_x07;
    pOutData->pBuff[pOutData->len++] = SDP_HEADER_x0E;

    // data len
    pOutData->pBuff[pOutData->len++] = 4;  // cmd(1byte) + addr(3bytes)

    // cmd 'U'
    pOutData->pBuff[pOutData->len++] = 0x35;
    pOutData->pBuff[pOutData->len++] = 0x35;

    // addr
    uint32_t addr = address;
    for (uint16_t i = 0; i < 6; i++) {
        pOutData->pBuff[pOutData->len++] = nibble_to_hex(addr & 0xF, 'X');
        addr >>= 4;
    }

    // crc
    pOutData->pBuff[pOutData->len] = calc_crc_sdp(pOutData->pBuff + 3, pOutData->len - 3);
    pOutData->len++;
}


/*
 * Invoke user cmd
 *
 */
void invoke_user_cmd_msdp(__msdp_out_data * const pOutData, const __msdp_wr_data * const pWrData) {
    pOutData->len = 0;

    if (pWrData->len > 0 && pWrData->len < 30) {
        // header
        pOutData->pBuff[pOutData->len++] = SDP_HEADER_x07;
        pOutData->pBuff[pOutData->len++] = SDP_HEADER_x0E;

        // data len
        pOutData->pBuff[pOutData->len++] = pWrData->len + 1;  // cmd(1byte) + data

        // cmd 'Z'
        pOutData->pBuff[pOutData->len++] = nibble_to_hex(SDP_CMD_Z & 0xF, 'X');
        pOutData->pBuff[pOutData->len++] = nibble_to_hex(SDP_CMD_Z >> 4, 'X');

        // data
        for (uint16_t i = 0; i < pWrData->len; i++) {
            pOutData->pBuff[pOutData->len++] = nibble_to_hex(pWrData->pBuff[i] & 0xF, 'X');
            pOutData->pBuff[pOutData->len++] = nibble_to_hex(pWrData->pBuff[i] >> 4, 'X');
        }

        // crc
        pOutData->pBuff[pOutData->len] = calc_crc_sdp(pOutData->pBuff + 3, pOutData->len - 3);
        pOutData->len++;
    }
}

