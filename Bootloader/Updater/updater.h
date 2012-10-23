/******************************************************************************/
/* updater.h                                                                  */
/******************************************************************************/


#ifndef __UPDATER_H
#define __UPDATER_H

#include "Iap\bl_iap.h"
#include "Updater\updater_def.h"



#define FLG_NEED_UPDATE     (0xAA55AA55)
#define FW_START_ADDRESS    IAP_MAIN_SECT_ADDR
#define FW_START_SECT       IAP_FW_START_SECT
#define UPD_BUFF_SIZE       (512)


void init_updater();
void reset_updater();
void erase_flg_need_update();
void run_main_firmware();
void read_flash_updater();
bool_t is_need_update();
bool_t is_update_end();
bool_t erase_fw_updater();
bool_t write_fw_updater();
bool_t finalize_fw_update();




#endif