/******************************************************************************/
/* updater.c                                                                  */
/******************************************************************************/


#include "data_types.h"
#include "At25df\at25df.h"
#include "Fmem\fmem.h"
#include "Iap\iap.h"
#include "Updater\updater.h"



static const __FMEM_SETT fw_mem = {FMEM_PAGE_SIZE, FMEM_SECTOR_SIZE, FW_START_ADDR, FW_MEM_SIZE};
static __TOOLS_UPDATER tools = {0, 0, 0, 0, FALSE_T};
static __FLASH_UPDATER flash_data = {0, 0, 0};

#pragma data_alignment = 4
static uint8_t upd_buffer[UPD_BUFF_SIZE];
static uint32_t upd_buff_len = 0;

#pragma data_alignment = 4
static __FW_SETT_UPDATER fw_sett = {0, 0};



/*
 * Init updater
 *
 */
void init_updater() {
    __FMEM_DATA data;

    data.addr = FW_STAFF_MEM_ADDR;
    data.pBuff = (uint8_t *)&flash_data;
    data.len = sizeof(__FLASH_UPDATER);

    read_data_fmem(&fw_mem, &data);
}


/*
 * Reset updater state
 *
 */
void reset_update_state() {
    tools.flg_update_end = FALSE_T;
    tools.addr = FW_START_ADDRESS;
    tools.crc = 0UL;
    tools.data_counter = 0UL;
    tools.read_data_counter = 0UL;

    fw_sett.fw_size = 0UL;
    fw_sett.fw_crc = 0UL;
}


/*
 * Test demand of update
 *
 */
bool_t is_need_update() {
    if (flash_data.flg_ok == FLG_NEED_UPDATE) {
        if (flash_data.fw_size && flash_data.fw_size < FW_MEM_SIZE) {
            reset_update_state();

            while (!tools.flg_update_end) {
                if (read_block_from_flash_upd()) {
                    for (uint32_t i = 0; i < upd_buff_len; i++) {
                        tools.crc += upd_buffer[i];
                    }
                }
            }

            if (!(tools.crc + flash_data.row_fw_crc)) {
                return TRUE_T;
            }
        }

        // erase flag - fw is fake
        erase_flg_need_update();
    }

    return FALSE_T;
}


/*
 * Test fin update
 *
 */
bool_t is_update_end() {
    return tools.flg_update_end;
}


/*
 * Run main firmware
 *
 */
void run_main_firmware() {
    ((pFW_ENTRY)(FW_START_ADDRESS))();
}


/*
 * Erase fw area in mcu flash
 *
 */
bool_t erase_main_fw_area_upd() {
    const uint32_t end_sector = determ_sector_num_iap(flash_data.fw_size + FW_START_ADDRESS - 16UL);
    uint32_t error;

    error = prepare_sector_iap(FW_START_SECT, end_sector);
    if (error != IAP_CMD_SUCCESS) {
        return FALSE_T;
    }

    error = erase_sector_iap(FW_START_SECT, end_sector);
    if (error != IAP_CMD_SUCCESS) {
        return FALSE_T;
    }

    error = blank_check_sector_iap(FW_START_SECT, end_sector);
    return (error != IAP_CMD_SUCCESS) ? FALSE_T : TRUE_T;
}


/*
 * Read data from ext flash
 *
 */
bool_t read_block_from_flash_upd() {
    __FMEM_DATA data;

    upd_buff_len = ((flash_data.fw_size - tools.data_counter) > UPD_BUFF_SIZE)
        ? UPD_BUFF_SIZE : (flash_data.fw_size - tools.data_counter);

    data.addr = tools.read_data_counter;
    data.pBuff = upd_buffer;
    data.len = upd_buff_len;

    tools.addr = FW_START_ADDRESS + tools.data_counter;
    tools.data_counter += upd_buff_len;

    // Read
    if (read_data_fmem(&fw_mem, &data) != eMEM_OK) {
        tools.flg_update_end = TRUE_T;
        return FALSE_T;
    }

    // Inc counter
    tools.read_data_counter += upd_buff_len;

    // Testing by end
    if (flash_data.fw_size == tools.data_counter) {
        tools.flg_update_end = TRUE_T;
    }

    return TRUE_T;
}


/*
 * Write data to mcu flash
 *
 */
bool_t write_block_in_main_fw_upd() {
    if (tools.flg_update_end == TRUE_T) {
        if (upd_buff_len < 16) {
            return FALSE_T;
        }

        upd_buff_len -= 16;
        tools.data_counter -= 16;

        // read fw_crc and fw_size
        fw_sett.fw_size = *((uint32_t *)(upd_buffer + upd_buff_len));
        fw_sett.fw_crc = *((uint32_t *)(upd_buffer + upd_buff_len + 4));
    }

    if (upd_buff_len > 0) {
        uint32_t error;
        const uint32_t start_sector = determ_sector_num_iap(tools.addr);

        // calc crc
        for (uint32_t i = 0; i < upd_buff_len; i++) {
            tools.crc += (uint32_t)upd_buffer[i];
        }

        // Write to mcu flash
        error = prepare_sector_iap(start_sector, start_sector);
        if (error != IAP_CMD_SUCCESS) {
            return FALSE_T;
        }

        error = copy_ram_to_flash_iap(tools.addr, (uint32_t)upd_buffer, upd_buff_len);
        if (error != IAP_CMD_SUCCESS) {
            return FALSE_T;
        }

        error = compare_iap(tools.addr, (uint32_t)upd_buffer, upd_buff_len);
        if (error != IAP_CMD_SUCCESS) {
            return FALSE_T;
        }
    }
    return TRUE_T;
}


/*
 * Finalize update-process
 *
 */
bool_t finalize_update() {
    // Validating crc
    tools.crc = 0UL - tools.crc;

    if ((fw_sett.fw_size == tools.data_counter) && (fw_sett.fw_crc == tools.crc)) {
        return safety_write_flash_iap((uint8_t *)&fw_sett, SHIFT_FW_SIZE_ADDR, sizeof(__FW_SETT_UPDATER));
    }

    return FALSE_T;
}


/*
 * Erase flg in ext flash
 *
 */
void erase_flg_need_update() {
    const uint32_t flg = 0;
    __FMEM_DATA data;

    data.addr = FW_STAFF_MEM_ADDR;
    data.pBuff = (uint8_t *)&flg;
    data.len = sizeof(flg);

    change_data_fmem(&fw_mem, &data);
}

