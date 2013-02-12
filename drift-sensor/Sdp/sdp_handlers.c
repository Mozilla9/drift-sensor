/*
 * $Id$
 */


#include "data_types.h"
#include "Core\core.h"
#include "Uart\uart0.h"
#include "Uart\v_printf.h"
#include "At25df\at25df.h"
#include "Lib\bin_to_bcd.h"
#include "Fmem\fmem.h"
#include "Sett\settings.h"
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
    DEBUG_PRINTF("erase_pg_flash()\r\n");
    return erase_memory_fmem(&pg_mem) ? SDP_NACK_x07 : SDP_ACK_x06;
}


/*
 * Erase sett_flash
 *
 */
uint8_t erase_sett_flash() {
    DEBUG_PRINTF("erase_sett_flash()\r\n");
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
        data.addr <<= 4;
        data.addr |= nibble_to_bin(pData[i]);
    }
    DEBUG_PRINTF("write_pg_flash()\r\n");
    DEBUG_PRINTF("pg_addr: 0x%8x\r\n", data.addr);

    // extract data
    DEBUG_PRINTF("pg_data: ");
    data.len = (len - 6) / 2;
    data.pBuff = buff;
    for (uint16_t i = 0, k = 6; i < data.len; i++) {
        buff[i] = nibble_to_bin(pData[k++]);
        buff[i] <<= 4;
        buff[i] |= nibble_to_bin(pData[k++]);
        DEBUG_PRINTF("%2x ", buff[i]);
    }
    DEBUG_PRINTF("\r\n");

    return write_data_fmem(&pg_mem, &data) ? SDP_NACK_x07 : SDP_ACK_x06;
}


/*
 * Write sett_flash
 *
 */
uint8_t write_sett_flash(const uint8_t * pData, const uint8_t len) {
    __FMEM_DATA data;
    uint8_t buff[26];

    // vaildating
    if (len < 8 || len > (SDP_PDATA_MAX_LEN - 2) || len % 2) {
        return SDP_NACK_x07;
    }

    // extract addr
    data.addr = 0;
    for (uint16_t i = 0; i < 6; i++) {
        data.addr <<= 4;
        data.addr |= nibble_to_bin(pData[i]);
    }
    DEBUG_PRINTF("write_sett_flash()\r\n");
    DEBUG_PRINTF("st_addr: 0x%8x\r\n", data.addr);

    // extract data
    DEBUG_PRINTF("st_data: ");
    data.len = (len - 6) / 2;
    data.pBuff = buff;
    for (uint16_t i = 0, k = 6; i < data.len; i++) {
        buff[i] = nibble_to_bin(pData[k++]);
        buff[i] <<= 4;
        buff[i] |= nibble_to_bin(pData[k++]);
        DEBUG_PRINTF("%2x ", buff[i]);
    }
    DEBUG_PRINTF("\r\n");

    write_app_settings(&data);
    return SDP_ACK_x06;
}


/*
 * Invoke code
 *
 */
uint8_t invoke_code(const uint8_t * pData, const uint8_t len) {
    uint32_t addr;

    // vaildating
    if (len < 8) {
        return SDP_NACK_x07;
    }

    // extract addr
    addr = 0;
    for (uint16_t i = 0; i < 8; i++) {
        addr <<= 4;
        addr |= nibble_to_bin(pData[i]);
    }
    DEBUG_PRINTF("invoke_code: 0x%8x\r\n", addr);
    waite_tx_all_uart0();
    ((void (*) ())addr)();

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

    DEBUG_PRINTF("user_cmd: 0x%2x\r\n", user_cmd);

    switch (user_cmd) {
        case CMD_SWITCH_PROTO:
            set_proto_type(PROTO_TYPE_CONSOLE);
            break;

        case CMD_RESET_DEVICE:
            waite_tx_all_uart0();
            reset_device();
            break;

        default:
            break;
    }

    return SDP_ACK_x06;
}

