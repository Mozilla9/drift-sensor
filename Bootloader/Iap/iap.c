/*
 * $Id$
 */


#include "data_types.h"
#include "Core\core.h"
#include "Iap\iap_def.h"
#include "Iap\iap.h"


static const pIAP_ENTRY pIapFunc = (pIAP_ENTRY)IAP_ADR_ARM;
static volatile uint32_t iap_result[3];
static uint32_t iap_command[5];
static uint32_t dw_timeout;


#define TIMEOUT_IAP          (600000L)            // Timeout value( ~40мс )


/*
 * Blank check sector
 *
 */
__arm uint32_t blank_check_sector_iap(const uint32_t start_sect, const uint32_t end_sect) {
    dw_timeout = TIMEOUT_IAP;
    iap_command[0] = IAP_BLANK_CHECK_SECT;
    iap_command[1] = start_sect;
    iap_command[2] = end_sect;

    enter_cs();
    unsafe_clear_wdt();

    pIapFunc(iap_command, iap_result);
    while ((iap_result[0] == IAP_BUSY) && (dw_timeout--));

    exit_cs();

    return iap_result[0];
}


/*
 * Prepare sector
 *
 */
__arm uint32_t prepare_sector_iap(const uint32_t start_sect, const uint32_t end_sect) {
    dw_timeout = TIMEOUT_IAP;
    iap_command[0] = IAP_PREPARE_SECT;
    iap_command[1] = start_sect;
    iap_command[2] = end_sect;

    enter_cs();
    unsafe_clear_wdt();

    pIapFunc(iap_command, iap_result);
    while ((iap_result[0] == IAP_BUSY) && (dw_timeout--));

    exit_cs();

    return iap_result[0];
}


/*
 * Erase sector
 *
 */
__arm uint32_t erase_sector_iap(const uint32_t start_sect, const uint32_t end_sect) {
    dw_timeout = TIMEOUT_IAP;
    iap_command[0] = IAP_ERASE_SECT;
    iap_command[1] = start_sect;
    iap_command[2] = end_sect;
    iap_command[3] = FCCLK_kHz;

    enter_cs();
    unsafe_clear_wdt();

    pIapFunc(iap_command, iap_result);
    while ((iap_result[0] == IAP_BUSY) && (dw_timeout--));

    exit_cs();

    return iap_result[0];
}


/*
 * Copy ram to flash sector
 *
 */
__arm uint32_t copy_ram_to_flash_iap(const uint32_t dst_addr, const uint32_t src_addr, const uint32_t len) {
    dw_timeout = TIMEOUT_IAP;
    iap_command[0] = IAP_COPY_RAM_TO_FLASH;
    iap_command[1] = dst_addr;
    iap_command[2] = src_addr;
    iap_command[3] = (len == 768) ? 1024 : len;
    iap_command[4] = FCCLK_kHz;

    enter_cs();
    unsafe_clear_wdt();

    pIapFunc(iap_command, iap_result);
    while ((iap_result[0] == IAP_BUSY) && (dw_timeout--));

    exit_cs();

    return iap_result[0];
}


/*
 * Compare
 *
 */
__arm uint32_t compare_iap(const uint32_t dst_addr, const uint32_t src_addr, const uint32_t len) {
    dw_timeout = TIMEOUT_IAP;
    iap_command[0] = IAP_COMPARE;
    iap_command[1] = dst_addr;
    iap_command[2] = src_addr;
    iap_command[3] = (len == 768) ? 1024 : len;

    enter_cs();
    unsafe_clear_wdt();

    pIapFunc(iap_command, iap_result);
    while ((iap_result[0] == IAP_BUSY) && (dw_timeout--));

    exit_cs();

    return iap_result[0];
}


/*
 * Read part id mcu
 *
 */
__arm uint32_t read_part_id_iap(uint32_t * dst) {
    dw_timeout = TIMEOUT_IAP;
    iap_command[0] = IAP_READ_PART_ID;

    enter_cs();
    unsafe_clear_wdt();

    pIapFunc(iap_command, iap_result);
    while ((iap_result[0] == IAP_BUSY) && (dw_timeout--));

    exit_cs();

    *dst = iap_result[1];
    return iap_result[0];
}


/*
 * Read sys bootloader vers mcu
 *
 */
__arm uint32_t read_bl_vers_iap(uint32_t * dst) {
    dw_timeout = TIMEOUT_IAP;
    iap_command[0] = IAP_READ_BOOT_VERS;

    enter_cs();
    unsafe_clear_wdt();

    pIapFunc(iap_command, iap_result);
    while ((iap_result[0] == IAP_BUSY) && (dw_timeout--));

    exit_cs();

    *dst = iap_result[1];
    return iap_result[0];
}
