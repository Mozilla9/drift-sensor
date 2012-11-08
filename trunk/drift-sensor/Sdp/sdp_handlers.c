/******************************************************************************/
/* sdp_handlers.c                                                             */
/******************************************************************************/


#include "data_types.h"
#include "Core\core.h"
#include "Uart\v_printf.h"
#include "At25df\at25df.h"
#include "Lib\bin_to_bcd.h"
#include "Fmem\fmem.h"
#include "Compass\compass.h"
#include "Sdp\sdp.h"



static const __FMEM_SETT pg_mem = {FMEM_PAGE_SIZE, FMEM_SECTOR_SIZE, FW_START_ADDR, FW_MEM_SIZE};


#define CMD_SWITCH_PROTO    1
#define CMD_RESET_DEVICE    2


/*
 * Erase pg_flash
 *
 */
uint8_t erase_pg_flash() {
    return erase_memory_fmem(&pg_mem) ? SDP_NACK_x07 : SDP_ACK_x06;
}


/*
 * Erase sett_flash
 *
 */
uint8_t erase_sett_flash() {
    return SDP_ACK_x06;
}


/*
 * Write pg_flash
 *
 */
uint8_t write_pg_flash(const uint8_t * pData, const uint8_t len) {
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
uint8_t write_sett_flash(const uint8_t * pData, const uint8_t len) {
    return SDP_ACK_x06;
}


/*
 * Invoke code
 *
 */
uint8_t invoke_code(const uint8_t * pData, const uint8_t len) {
    return SDP_ACK_x06;
}


/*
 * Invoke user cmd
 *
 */
uint8_t invoke_user_cmd(const uint8_t * pData, const uint8_t len) {
    uint8_t user_cmd;

    user_cmd = nibble_to_bin(pData[0]);
    user_cmd <<= 4;
    user_cmd |= nibble_to_bin(pData[1]);

    switch (user_cmd) {
        case CMD_SWITCH_PROTO:
            set_proto_type(PROTO_TYPE_CONSOLE);
            break;

        case CMD_RESET_DEVICE:
            reset_device();
            break;

        default:
            break;
    }

    return SDP_ACK_x06;
}

