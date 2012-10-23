/******************************************************************************/
/* fmem.c                                                                     */
/******************************************************************************/


#include "data_types.h"
#include "Core\core.h"
#include "At25df\at25df.h"
#include "Fmem\fmem.h"




/*
 * Convert ext-addr to hw-addr
 *
 */
static uint32_t convert_ext_addr_in_hw_addr(const __FMEM_SETT * const pSett, const uint32_t ext_addr) {
    return (pSett->start_addr + ext_addr);
}


/*
 * Get write dlen
 *
 */
static uint32_t get_write_datalen(const __FMEM_SETT * const pSett, const uint32_t addr) {
    return ((((addr / pSett->page_size) + 1) * pSett->page_size) - addr);
}


/*
 * Test addr range
 *
 */
static bool_t test_addr_range(const __FMEM_SETT * const pSett, const uint32_t addr) {
    return (addr < pSett->allocated_size) ? TRUE_T : FALSE_T;
}


/*
 * Test new sector write
 *
 */
static bool_t test_new_sector_write(const __FMEM_SETT * const pSett, const uint32_t addr) {
    return (addr % pSett->sector_size) ? FALSE_T : TRUE_T;
}


/*
 * Get space to end
 *
 */
uint32_t get_space_to_end_fmem(const __FMEM_SETT * const pSett, const uint32_t addr) {
    return (addr < pSett->allocated_size) ? (pSett->allocated_size - addr) : 0;
}


/*
 * Write data
 *
 */
uint32_t write_data_fmem(const __FMEM_SETT * const pSett, const __FMEM_DATA * const pData) {
    uint32_t count_addr;
    uint32_t count_data;
    uint32_t err;
    __AT25DF_DATA hw_data;

    // Addr rang test
    if (test_addr_range(pSett, pData->addr) == FALSE_T) {
        return eMEM_ADDR_ERROR;
    }

    // Capacity test
    if ((pData->len > get_space_to_end_fmem(pSett, pData->addr)) || (pData->len == 0)) {
        return eMEM_DATA_ERROR;
    }

    // Write cycle
    count_addr = pData->addr;
    count_data = 0;
    while (count_data < pData->len) {
        // Determine data block for write
        hw_data.len = get_write_datalen(pSett, count_addr);
        hw_data.len = ((pData->len - count_data) > hw_data.len)
                              ? hw_data.len : (pData->len - count_data);

        // Prapare param for write
        hw_data.addr.addr32 = convert_ext_addr_in_hw_addr(pSett, count_addr);
        hw_data.pBuff = (pData->pBuff + count_data);

        // Test at new sectror write
        if (test_new_sector_write(pSett, count_addr) == TRUE_T) {
            // Erase sector
            clear_wdt();
            err = block_erase_at25df(hw_data.addr, eAT25DF_BLOCK_4);

            switch (err)
            {
                //--------------------------------------------------------------
                case eMEM_OK:
                break;

                //--------------------------------------------------------------
                case eMEM_BUSY:
                //--------------------------------------------------------------
                case eMEM_NOT_AVAILABLE:
                //--------------------------------------------------------------
                case eMEM_ADDR_ERROR:
                //--------------------------------------------------------------
                case eMEM_DATA_ERROR:
                //--------------------------------------------------------------
                case eMEM_WRITE_EN_ERROR:
                //--------------------------------------------------------------
                case eMEM_INTERNAL_ERROR:
                //--------------------------------------------------------------
                default:
                  return err;
            }
        }

        // Write data
        clear_wdt();
        err = program_page_at25df(&hw_data);

        switch (err)
        {
            //------------------------------------------------------------------
            case eMEM_OK:
            break;

            //------------------------------------------------------------------
            case eMEM_BUSY:
            //------------------------------------------------------------------
            case eMEM_NOT_AVAILABLE:
            //------------------------------------------------------------------
            case eMEM_ADDR_ERROR:
            //------------------------------------------------------------------
            case eMEM_DATA_ERROR:
            //------------------------------------------------------------------
            case eMEM_WRITE_EN_ERROR:
            //------------------------------------------------------------------
            case eMEM_INTERNAL_ERROR:
            //------------------------------------------------------------------
            default:
              return err;
        }

        // Shift counters
        count_addr += hw_data.len;
        count_data += hw_data.len;
    }

    return eMEM_OK;
}


/*
 * Change data - single page
 *
 */
uint32_t change_data_fmem(const __FMEM_SETT * const pSett, const __FMEM_DATA * const pData) {
    __AT25DF_DATA hw_data;

    // Addr rang test
    if (test_addr_range(pSett, pData->addr) == FALSE_T) {
        return eMEM_ADDR_ERROR;
    }

    // Capacity test
    if ((pData->len > get_space_to_end_fmem(pSett, pData->addr)) || (pData->len == 0)) {
        return eMEM_DATA_ERROR;
    }

    // One page test
    if (pData->len > get_write_datalen(pSett, pData->addr)) {
        return eMEM_DATA_ERROR;
    }

    // Change data - switch 1 -> 0
    hw_data.addr.addr32 = convert_ext_addr_in_hw_addr(pSett, pData->addr);
    hw_data.pBuff = pData->pBuff;
    hw_data.len = pData->len;

    // Write data
    clear_wdt();
    return program_page_at25df(&hw_data);
}


/*
 * Read data
 *
 */
uint32_t read_data_fmem(const __FMEM_SETT * const pSett, const __FMEM_DATA * const pData) {
    __AT25DF_DATA hw_data;

    // Addr rang test
    if (test_addr_range(pSett, pData->addr) == FALSE_T) {
        return eMEM_ADDR_ERROR;
    }

    // Capacity test
    if ((pData->len > get_space_to_end_fmem(pSett, pData->addr)) || (pData->len == 0)) {
        return eMEM_DATA_ERROR;
    }

    // Read data
    hw_data.addr.addr32 = convert_ext_addr_in_hw_addr(pSett, pData->addr);
    hw_data.pBuff = pData->pBuff;
    hw_data.len = pData->len;

    return read_data_bytes_at25df(&hw_data);
}


/*
 * Erase memory
 *
 */
uint32_t erase_memory_fmem(const __FMEM_SETT * const pSett) {
    __AT25DF_ADDRESS hw_addr;
    __eAt25DF_BLOCK block;
    uint32_t err;
    uint32_t int_addr;

    const bool_t aligment64k = (pSett->start_addr % AT25DF_SECTOR_SIZE_64)
                                                             ? FALSE_T : TRUE_T;
    const bool_t aligment32k = (pSett->start_addr % AT25DF_SECTOR_SIZE_32)
                                                             ? FALSE_T : TRUE_T;

    err = eMEM_NOT_AVAILABLE;
    int_addr = 0;
    while (test_addr_range(pSett, int_addr) == TRUE_T) {
        // Convert virtual addr in hw-addr
        hw_addr.addr32 = convert_ext_addr_in_hw_addr(pSett, int_addr);

        // Choice block size
        const uint32_t size_mud_mem = pSett->allocated_size - int_addr;

        if ((size_mud_mem >= AT25DF_SECTOR_SIZE_64) && (aligment64k == TRUE_T)) {
            block = eAT25DF_BLOCk_64;
            int_addr += AT25DF_SECTOR_SIZE_64;
        }
        else if ((size_mud_mem >= AT25DF_SECTOR_SIZE_32) && (aligment32k == TRUE_T)) {
            block = eAT25DF_BLOCK_32;
            int_addr += AT25DF_SECTOR_SIZE_32;
        }
        else {
            block = eAT25DF_BLOCK_4;
            int_addr += AT25DF_SECTOR_SIZE_4;
        }

        // Erase
        clear_wdt();
        err = block_erase_at25df(hw_addr, block);
        if (err != eMEM_OK) {
            break;
        }
    }

    return err;
}

