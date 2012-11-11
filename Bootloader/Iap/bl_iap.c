/******************************************************************************/
/* bl_iap.c                                                                   */
/******************************************************************************/


#include "data_types.h"
#include "Iap\iap.h"
#include "Iap\bl_iap.h"



#pragma data_alignment = 4
static uint8_t safety_buff[SAFETY_BUFF_SIZE];

static const uint32_t sector_addr[IAP_SECT_QNT] =
{
    SECTOR0,  SECTOR1,  SECTOR2,  SECTOR3,  SECTOR4,  SECTOR5,  SECTOR6,
    SECTOR7,  SECTOR8,  SECTOR9,  SECTOR10 /*, SECTOR11, SECTOR12, SECTOR13,
    SECTOR14, SECTOR15, SECTOR16, SECTOR17, SECTOR18, SECTOR19, SECTOR20,
    SECTOR21, SECTOR22, SECTOR23, SECTOR24, SECTOR25, SECTOR26, SECTOR27*/
};


/*
 *
 *
 */
uint32_t determ_sector_num_iap(const uint32_t addr) {
    uint32_t ind = IAP_SECT_QNT - 1;

    while (ind) {
        if (addr >= sector_addr[ind]) {
            break;
        }
        ind--;
    }
    return ind;
}


/*
 *
 *
 */
bool_t safety_write_flash_iap(const uint8_t * pData, const uint32_t shift, uint32_t len) {
    uint32_t req_package_num = shift / SAFETY_BUFF_SIZE;
    uint32_t new_shift = shift - (req_package_num * SAFETY_BUFF_SIZE);
    uint32_t ind = 0;
    uint32_t sect_ind = 0;
    uint8_t * pSect;
    uint32_t err_code;

    // Earse swap sector
    err_code = prepare_sector_iap(IAP_RESERV_SECTOR, IAP_RESERV_SECTOR);
    if (err_code != IAP_CMD_SUCCESS) {
        return FALSE_T;
    }

    err_code = erase_sector_iap(IAP_RESERV_SECTOR, IAP_RESERV_SECTOR);
    if (err_code != IAP_CMD_SUCCESS) {
        return FALSE_T;
    }

    // Copy by snippets
    pSect = (uint8_t *)sector_addr[IAP_MAIN_SECTOR];

    do {
        do {
             safety_buff[sect_ind++] = *pSect++;
        }
        while (sect_ind < SAFETY_BUFF_SIZE);
        sect_ind = 0;

        // Testing snippet
        if (req_package_num == ind) {
            do {
                safety_buff[new_shift++] = *pData++;
                if (!--len) {
                    break;
                }
            }
            while (new_shift < SAFETY_BUFF_SIZE);
            new_shift = 0;

            if (len) {
                req_package_num++;
            }
            else {
                req_package_num = 0;
            }
        }

        // Prepare swap sector
        err_code = prepare_sector_iap(IAP_RESERV_SECTOR, IAP_RESERV_SECTOR);
        if (err_code != IAP_CMD_SUCCESS) {
            return FALSE_T;
        }

        // Write to swap
        err_code = copy_ram_to_flash_iap(sector_addr[IAP_RESERV_SECTOR] + (SAFETY_BUFF_SIZE * ind),
                                         (uint32_t)safety_buff,
                                         SAFETY_BUFF_SIZE);
        if (err_code != IAP_CMD_SUCCESS) {
            return FALSE_T;
        }
    }
    while (ind++ < ((4096 / SAFETY_BUFF_SIZE) - 1));

    // Testing by succesfully
    if (*((uint32_t *)(sector_addr[IAP_RESERV_SECTOR] + SHIFT_FW_VALID_FLG)) != IAP_VALID_DATA) {
        return FALSE_T;
    }

    // Erase main sector
    err_code = prepare_sector_iap(IAP_MAIN_SECTOR, IAP_MAIN_SECTOR);
    if (err_code != IAP_CMD_SUCCESS) {
        return FALSE_T;
    };

    err_code = erase_sector_iap(IAP_MAIN_SECTOR, IAP_MAIN_SECTOR);
    if (err_code != IAP_CMD_SUCCESS) {
        return FALSE_T;
    };

    // Transfer from swap to main
    ind = 0;
    pSect = (uint8_t *)sector_addr[IAP_RESERV_SECTOR];
    sect_ind = 0;
    do {
        do {
            safety_buff[sect_ind++] = *pSect++;
        }
        while (sect_ind < SAFETY_BUFF_SIZE);
        sect_ind = 0;

        err_code = prepare_sector_iap(IAP_MAIN_SECTOR, IAP_MAIN_SECTOR);
        if (err_code != IAP_CMD_SUCCESS) {
            return FALSE_T;
        };

        err_code = copy_ram_to_flash_iap(sector_addr[IAP_MAIN_SECTOR] + (SAFETY_BUFF_SIZE * ind),
                                         (uint32_t)safety_buff,
                                         SAFETY_BUFF_SIZE);
        if (err_code != IAP_CMD_SUCCESS) {
            return FALSE_T;
        }
    }
    while (ind++ < ((4096 / SAFETY_BUFF_SIZE) - 1));

    // Testing by succesfully
    if (*((uint32_t *)(sector_addr[IAP_MAIN_SECTOR] + SHIFT_FW_VALID_FLG)) != IAP_VALID_DATA) {
        return FALSE_T;
    }

    // Erase swap
    err_code = prepare_sector_iap(IAP_RESERV_SECTOR, IAP_RESERV_SECTOR);
    if (err_code != IAP_CMD_SUCCESS) {
        return FALSE_T;
    }

    err_code = erase_sector_iap(IAP_RESERV_SECTOR, IAP_RESERV_SECTOR);
    return (err_code != IAP_CMD_SUCCESS) ? FALSE_T : TRUE_T;
}


/*
 *
 *
 */
void repair_flash_iap() {
    uint8_t * pSect;
    uint32_t ind = 0;
    uint32_t sect_ind = 0;
    uint32_t err_code;

    // Erase main sector
    err_code = prepare_sector_iap(IAP_MAIN_SECTOR, IAP_MAIN_SECTOR);
    if (err_code != IAP_CMD_SUCCESS) {
        return;
    }

    err_code = erase_sector_iap(IAP_MAIN_SECTOR, IAP_MAIN_SECTOR);
    if (err_code != IAP_CMD_SUCCESS) {
        return;
    }

    // Copy swap to main
    pSect = (uint8_t *)sector_addr[IAP_RESERV_SECTOR];

    do {
        do {
            safety_buff[sect_ind++] = *pSect++;
        }
        while (sect_ind < SAFETY_BUFF_SIZE);
        sect_ind = 0;

        err_code = copy_ram_to_flash_iap(sector_addr[IAP_MAIN_SECTOR] + (SAFETY_BUFF_SIZE * ind),
                                         (uint32_t)safety_buff,
                                         SAFETY_BUFF_SIZE);
        if (err_code != IAP_CMD_SUCCESS) {
            return;
        }
    }
    while (ind++ < ((4096 / SAFETY_BUFF_SIZE) - 1));

    // Testing by succesfully
    if (*((uint32_t *)(sector_addr[IAP_MAIN_SECTOR] + SHIFT_FW_VALID_FLG)) != IAP_VALID_DATA) {
        return;
    }

    // Erase swap
    err_code = prepare_sector_iap(IAP_RESERV_SECTOR, IAP_RESERV_SECTOR);
    if (err_code != IAP_CMD_SUCCESS) {
        return;
    }

    erase_sector_iap(IAP_RESERV_SECTOR, IAP_RESERV_SECTOR);
}


/*
 * Erase main fw area in mcu flash
 *
 */
static bool_t erase_fw_area_iap() {
    uint32_t error;

    error = prepare_sector_iap(IAP_FW_START_SECT, IAP_FW_END_SECT);
    if (error != IAP_CMD_SUCCESS) {
        return FALSE_T;
    }

    error = erase_sector_iap(IAP_FW_START_SECT, IAP_FW_END_SECT);
    if (error != IAP_CMD_SUCCESS) {
        return FALSE_T;
    }

    error = blank_check_sector_iap(IAP_FW_START_SECT, IAP_FW_END_SECT);
    return (error != IAP_CMD_SUCCESS) ? FALSE_T : TRUE_T;
}


/*
 * Erase bl area in mcu flash
 *
 */
static bool_t erase_bl_area_iap() {
    uint32_t error;

    error = prepare_sector_iap(IAP_BL_START_SECT, IAP_BL_END_SECT);
    if (error != IAP_CMD_SUCCESS) {
        return FALSE_T;
    }

    error = erase_sector_iap(IAP_BL_START_SECT, IAP_BL_END_SECT);
    if (error != IAP_CMD_SUCCESS) {
        return FALSE_T;
    }

    error = blank_check_sector_iap(IAP_BL_START_SECT, IAP_BL_END_SECT);
    return (error != IAP_CMD_SUCCESS) ? FALSE_T : TRUE_T;
}


/*
 * Clear mcu chip
 *
 */
void clear_mcu_iap(const uint32_t __defend_code) {
    if (__defend_code == CHIP_UNLOCK_CODE) {
        if (erase_fw_area_iap()) {
            erase_bl_area_iap();
        }
    }
}
