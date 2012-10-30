/******************************************************************************/
/* bl_iap.h                                                                   */
/******************************************************************************/


#ifndef __BL_IAP_H
#define __BL_IAP_H


#include "Iap\iap_def.h"


#define SAFETY_BUFF_SIZE          (512)                // Buff len for safety-write func
#define IAP_SECT_QNT              (11)                 // Qnt Flash sectors for LPC2364

#define IAP_MAIN_SECTOR           1
#define IAP_RESERV_SECTOR         2
#define IAP_MAIN_SECT_ADDR        SECTOR1
#define IAP_RESERV_SECT_ADDR      SECTOR2

#define IAP_FW_START_SECT         8
#define IAP_FW_END_SECT           10
#define IAP_BL_START_SECT         0
#define IAP_BL_END_SECT           7

#define CHIP_UNLOCK_CODE          123456789    // Code for chip erase
#define IAP_VALID_DATA            0xAA55AA55   // Test value for segment writes
#define SHIFT_FW_SIZE_ADDR        (4084)
#define SHIFT_FW_CRC_ADDR         (4088)
#define SHIFT_FW_VALID_FLG        (4092)


void repair_flash_iap();
bool_t safety_write_flash_iap(const uint8_t * pData, const uint32_t shift, uint32_t len);
uint32_t determ_sector_num_iap(const uint32_t addr);
void clear_mcu_iap(const uint32_t __defend_code);




#endif