/******************************************************************************/
/* updater.h                                                                  */
/******************************************************************************/


#ifndef __UPDATER_H
#define __UPDATER_H

#include "Iap\bl_iap.h"



#define FLG_NEED_UPDATE     (0xAA55AA55)
#define FW_START_ADDRESS    IAP_FW_START_ADDR
#define FW_START_SECT       IAP_FW_START_SECT
#define UPD_BUFF_SIZE       (512)


typedef void (* pFW_ENTRY)();


typedef struct
{
    uint32_t flg_ok;
    uint32_t fw_size;
    uint32_t row_fw_crc;
} __FLASH_UPDATER;


typedef struct
{
    uint32_t addr;
    uint32_t crc;
    uint32_t data_counter;
    uint32_t read_data_counter;
    bool_t flg_update_end;
} __TOOLS_UPDATER;


typedef struct
{
    uint32_t fw_size;
    uint32_t fw_crc;
} __FW_SETT_UPDATER;



void init_updater();
void reset_update_state();
void run_main_firmware();
void erase_flg_need_update();
bool_t read_block_from_flash_upd();
bool_t is_need_update();
bool_t is_update_end();
bool_t erase_main_fw_area_upd();
bool_t write_block_in_main_fw_upd();
bool_t finalize_update();




#endif