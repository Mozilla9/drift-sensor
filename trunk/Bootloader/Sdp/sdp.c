/*
 * Serial download protocol
 *
 * $Id$
 */


#include "data_types.h"
#include "sdp.h"


extern uint32_t serprintf(const int8_t * format, ...);
extern uint8_t erase_pg_flash();
extern uint8_t erase_sett_flash();
extern uint8_t write_pg_flash(const uint8_t * pData, const uint8_t len);
extern uint8_t write_sett_flash(const uint8_t * pData, const uint8_t len);
extern uint8_t invoke_code(const uint8_t * pData, const uint8_t len);
extern uint8_t invoke_user_cmd(const uint8_t * pData, const uint8_t len);


static __sdp_data sdp;


/*
 * Send answer
 *
 */
static void send_answer(const uint8_t answ) {
    serprintf("%c", answ);
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
 * nibble to bin
 *
 */
static uint8_t nibble_to_bin(const uint8_t nibble) {
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
 * Parsing data-packet
 *
 */
static void parsing_packet_sdp() {
    uint8_t res = SDP_NACK_x07;
    const uint8_t data_len = sdp.buff[0] * 2;

    // test len
    if (sdp.len > 3 && !(sdp.len % 2) && data_len == (sdp.len - 2)) {
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
                    
                case SDP_CMD_Z:
                    res = invoke_user_cmd(sdp.buff + 3, data_len - 2);
                    break;

                default:
                    break;
            }
        }
    }

    send_answer(res);
}


/*
 * Insert char and process
 *
 */
void sdp_insert_char(const uint8_t _char) {
    static uint16_t expected_len;
    static uint16_t state = SDP_STATE_WAIT_HDR;

    if (sdp.len < SDP_BUFF_LEN) {
        sdp.buff[sdp.len++] = _char;
    } else {
        sdp.len = 0;
        state = SDP_STATE_WAIT_HDR;
    }

    if (sdp.len) {
        if (sdp.len > 1 && sdp.buff[sdp.len - 2] == SDP_HEADER_x07
            && sdp.buff[sdp.len - 1] == SDP_HEADER_x0E) {
            sdp.len = 0;
            state = SDP_STATE_WAIT_LEN;
            return;
        }

        switch (state) {
            case SDP_STATE_WAIT_HDR:
                if (sdp.len > 1) {
                    sdp.buff[0] = _char;
                    sdp.len = 1;
                }
                break;

            case SDP_STATE_WAIT_LEN:
                expected_len = (2 * _char) + 2;
                state = SDP_STATE_DATA_RCV;
                break;

            case SDP_STATE_DATA_RCV:
                if (sdp.len == expected_len) {
                    parsing_packet_sdp();

                    sdp.len = 0;
                    state = SDP_STATE_WAIT_HDR;
                }
                break;

            default:
                sdp.len = 0;
                state = SDP_STATE_WAIT_HDR;
                break;
        }
    }
}
