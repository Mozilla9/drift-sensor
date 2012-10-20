/******************************************************************************/
/* sdp.c - serial download protocol                                           */
/******************************************************************************/


#include "data_types.h"
#include "Core\core.h"
#include "At25df\at25df.h"
#include "Lib\bin_to_bcd.h"
#include "Fmem\fmem.h"
#include "Sdp\sdp.h"


static __sdp_data sdp;
static const __FMEM_SETT pg_mem = {FMEM_PAGE_SIZE, FMEM_SECTOR_SIZE, FW_START_ADDR, FW_MEM_SIZE};


/*
 * Send answer
 *
 */
static void send_answer(const uint8_t answ) {

}


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
 * Erase pg_flash
 *
 */
static uint8_t erase_pg_flash() {
    return erase_memory_fmem(&pg_mem) ? SDP_NACK_x07 : SDP_ACK_x06;
}


/*
 * Erase sett_flash
 *
 */
static uint8_t erase_sett_flash() {
    return SDP_ACK_x06;
}


/*
 * Write pg_flash
 *
 */
static uint8_t write_pg_flash(const uint8_t * pData, const uint8_t len) {
    __FMEM_DATA data;
    uint8_t buff[26];

    // vaildating
    if (len < 8 || len > (SDP_PDATA_MAX_LEN - 2) || len % 2) {
        return SDP_NACK_x07;
    }

    // extract addr
    data.addr = 0;
    for (uint16_t i = 0; i < 6; i++) {
        data.addr |= nibble_to_bin(pData[i]);
        data.addr <<= 4;
    }

    // extract data
    data.len = (len - 6) / 2;
    data.pBuff = buff;
    for (uint16_t i = 0, k = 6; i < data.len; i++) {
        buff[i] = nibble_to_bin(k++);
        buff[i] <<= 4;
        buff[i] |= nibble_to_bin(k++);
    }

    return write_data_fmem(&pg_mem, &data) ? SDP_NACK_x07 : SDP_ACK_x06;
}


/*
 * Write sett_flash
 *
 */
static uint8_t write_sett_flash(const uint8_t * pData, const uint8_t len) {
    return SDP_ACK_x06;
}


/*
 * Invoke code
 *
 */
static uint8_t invoke_code(const uint8_t * pData, const uint8_t len) {
    return SDP_ACK_x06;
}


/*
 * Parsing data-packet
 *
 */
void parsing_packet_sdp() {
    uint8_t res = SDP_NACK_x07;
    const uint8_t data_len = sdp.buff[0] << 1;

    // test expected len
    if (sdp.len > 5 && !(sdp.len % 2) && data_len == (sdp.len - 2)) {
        // test crc
        if (calc_crc_sdp(sdp.buff + 1, data_len) == sdp.buff[sdp.len - 1]) {
            const uint8_t cmd = (nibble_to_bin(sdp.buff[1]) << 4 | nibble_to_bin(sdp.buff[2]));

            switch (cmd) {
                case SDP_CMD_C:
                    res = erase_pg_flash();
                    break;

                case SDP_CMD_A:
                    res = erase_sett_flash();
                    break;

                case SDP_CMD_W:
                    res = write_pg_flash(sdp.buff + 3, data_len - 2);
                    break;

                case SDP_CMD_E:
                    res = write_sett_flash(sdp.buff + 3, data_len - 2);
                    break;

                case SDP_CMD_U:
                    res = invoke_code(sdp.buff + 3, data_len - 2);
                    break;

                default:
                    break;
            }
        }
    }

    send_answer(res);
}



