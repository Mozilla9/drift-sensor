/*
 * $Id$
 */


#include "data_types.h"
#include "Core\core.h"
#include "Spi\spi.h"
#include "At25df\at25df.h"


static bool_t m_is_flash_ok;
static uint8_t m_family_code;
static uint8_t m_density_code;



/*
 * Select chip
 *
 */
static void select_chip() {
    FIO0CLR_bit.P0_16 = 1;
}


/*
 * Lock chip
 *
 */
static void lock_chip() {
    FIO0SET_bit.P0_16 = 1;
}


/*
 * Read status register
 *
 */
static uint16_t read_status_reg() {
    // Test flash memory is Ok
    if (m_is_flash_ok != TRUE_T) {
        return 0x00FC;
    }

    select_chip();

    SPI_BUFF = eAT25DF_READ_SREG;
    while (!(SPI_FREE));

    // Read status reg (16-bit)
    uint16_t sreg;

    SPI_BUFF = AT25DF_SPI_READ_CLK;
    while (!(SPI_FREE));
    sreg = SPI_BUFF;
    sreg <<= 8;

    SPI_BUFF = AT25DF_SPI_READ_CLK;
    while (!(SPI_FREE));
    sreg |= SPI_BUFF;

    lock_chip();

    return ((sreg >> 8) | (sreg << 8));
}


/*
 * Read id
 *
 */
static void read_id(uint8_t * const _pId) {
    select_chip();

    SPI_BUFF = eAT25DF_READ_ID;
    while (!(SPI_FREE));

    // Read ID
    SPI_BUFF = AT25DF_SPI_READ_CLK;
    while (!(SPI_FREE));
    _pId[0] = SPI_BUFF;

    SPI_BUFF = AT25DF_SPI_READ_CLK;
    while (!(SPI_FREE));
    _pId[1] = SPI_BUFF;

    SPI_BUFF = AT25DF_SPI_READ_CLK;
    while (!(SPI_FREE));
    _pId[2] = SPI_BUFF;

    lock_chip();
}


/*
 * Write enable
 *
 */
static void write_enable() {
    select_chip();

    SPI_BUFF = eAT25DF_WRITE_EN;
    while (!(SPI_FREE));

    lock_chip();
}


/*
 * Write disable
 *
 */
/*static void write_disable() {
    select_chip();

    SPI_BUFF = eAT25DF_WRITE_DIS;
    while (!(SPI_FREE));

    lock_chip();
}*/


/*
 * Test at busy
 *
 */
static bool_t is_busy() {
    const uint16_t statusReg = read_status_reg();

    return (statusReg & AT25DF_BUSY) ? TRUE_T : FALSE_T;
}


/*
 * Test at write enable
 *
 */
static bool_t is_write_enable() {
    const uint16_t statusReg = read_status_reg();

    return (statusReg & AT25DF_WEL) ? TRUE_T : FALSE_T;
}


/*
 * Protect sector
 *
 */
/*static uint32_t protect_sector(const __AT25DF_ADDRESS _addr) {

    // Test write enable bit
    if (is_write_enable() == FALSE_T) {
        write_enable();

        if (is_write_enable() == FALSE_T) {
            return eMEM_WRITE_EN_ERROR;
        }
    }

    select_chip();

    // Write cmd
    SPI_BUFF = eAT25DF_PROTECT_SECTOR;

    // Write address
    for (uint32_t i = 0, k = 2; i < 3;) {
        if (SPI_FREE) {
            SPI_BUFF = _addr.addr_array[k--];
            i++;
        }
    }
    while (!(SPI_FREE));

    lock_chip();

    return eMEM_OK;
}*/


/*
 * Unprotect sector
 *
 */
static uint32_t unprotect_sector(const __AT25DF_ADDRESS _addr) {

    // Test write enable bit
    if (is_write_enable() == FALSE_T) {
        write_enable();

        if (is_write_enable() == FALSE_T) {
            return eMEM_WRITE_EN_ERROR;
        }
    }

    select_chip();

    // Write cmd
    SPI_BUFF = eAT25DF_UNPROTECT_SECTOR;

    // Write address
    for (uint32_t i = 0, k = 2; i < 3;) {
        if (SPI_FREE) {
            SPI_BUFF = _addr.addr_array[k--];
            i++;
        }
    }
    while (!(SPI_FREE));

    lock_chip();

    return eMEM_OK;
}


/*
 * Read sector protection register
 *
 */
static bool_t is_sector_protect(const __AT25DF_ADDRESS _addr) {

    select_chip();

    // Write cmd
    SPI_BUFF = eAT25DF_READ_PROT_REG;

    // Write address
    for (uint32_t i = 0, k = 2; i < 3;) {
        if (SPI_FREE) {
            SPI_BUFF = _addr.addr_array[k--];
            i++;
        }
    }
    while (!(SPI_FREE));

    // Read reg
    SPI_BUFF = AT25DF_SPI_READ_CLK;
    while (!(SPI_FREE));
    const uint8_t reg = SPI_BUFF;

    lock_chip();

    return (reg == 0) ? FALSE_T : TRUE_T;
}


/*
 * Get status flash
 *
 */
bool_t get_flash_status_at25df() {
    return m_is_flash_ok;
}


/*
 * Get flash family code
 *
 */
uint8_t get_family_code_at25df() {
    return m_family_code;
}


/*
 * Get flash density code
 *
 */
uint8_t get_density_code_at25df() {
    return m_density_code;
}


/*
 * Get flash density description
 *
 */
const int8_t * get_density_desc_at25df() {
    if (m_is_flash_ok) {
        switch (m_density_code) {
            case eATMEL_01M:
                return "1M";
            case eATMEL_02M:
                return "2M";
            case eATMEL_04M:
                return "4M";
            case eATMEL_08M:
                return "8M";
            case eATMEL_16M:
                return "16M";
            case eATMEL_32M:
                return "32M";
            case eATMEL_64M:
                return "64M";
        }
    }
    return "undef";
}


/*
 * Get flash family description
 *
 */
const int8_t * get_family_desc_at25df() {
    if (m_is_flash_ok) {
        switch (m_family_code) {
            case eATMEL_DATAFLAFH:
                return "ATMEL DATAFLASH";
            case eATMEL_AT25DF_26DF:
                return "AT25DF-26DF";
            case eATMEL_AT25F_25FS:
                return "AT25F-25FS";
            case eATMEL_QUAD:
                return "ATMEL QUAD";
        }
    }
    return "undef";
}


/*
 * Init flash chip
 *
 */
void init_at25df() {
    // Conf cs
    PINSEL1_bit.P0_16 = 0;                // CS
    FIO0DIR_bit.P0_16 = 1;                // CS
    PINMODE1_bit.P0_16 = 0;               // 0=PULL_UP

    lock_chip();
    init_spi();

    // Read Manufacturer and Device ID information
    uint8_t id[3];

    read_id(id);
    if (id[0] == ATMEL_MANUF_ID) {
        m_family_code = (id[1] >> 5);
        m_density_code = (id[1] & 0x1F);
        m_is_flash_ok = TRUE_T;
    }
}


/*
 * Read data bytes
 *
 */
uint32_t read_data_bytes_at25df(const __AT25DF_DATA * const _pData) {
    // Test flash memory is Ok
    if (m_is_flash_ok != TRUE_T) {
        return eMEM_NOT_AVAILABLE;
    }

    // Validate addr
    if (_pData->addr.addr32 >= AT25DF_FLASH_SIZE) {
        return eMEM_ADDR_ERROR;
    }

    // Test busy
    while (is_busy() == TRUE_T) {
        clear_wdt();
    }

    select_chip();

    // Write cmd
    SPI_BUFF = eAT25DF_READ_DATA;

    // Write address
    for (uint32_t i = 0, k = 2; i < 3;) {
        if (SPI_FREE) {
            SPI_BUFF = _pData->addr.addr_array[k--];
            i++;
        }
    }
    while (!(SPI_FREE));

    // Read data
    uint32_t ind = 0;
    const uint32_t len = _pData->len - 1;

    SPI_BUFF = AT25DF_SPI_READ_CLK;
    while (ind < len) {
        if (SPI_FREE) {
            _pData->pBuff[ind++] = SPI_BUFF;
            SPI_BUFF = AT25DF_SPI_READ_CLK;
        }
    }
    while (!(SPI_FREE));
    _pData->pBuff[ind] = SPI_BUFF;

    lock_chip();

    return eMEM_OK;
}


/*
 * Program page
 *
 */
uint32_t program_page_at25df(const __AT25DF_DATA * const _pData) {
    // Test flash memory is Ok
    if (m_is_flash_ok != TRUE_T) {
        return eMEM_NOT_AVAILABLE;
    }

    // Validate addr
    if (_pData->addr.addr32 >= AT25DF_FLASH_SIZE) {
        return eMEM_ADDR_ERROR;
    }

    // Validate addr + data_len
    const uint32_t freeSize = AT25DF_PAGE_SIZE - _pData->addr.addr_array[0];
    if (_pData->len > freeSize) {
        return eMEM_DATA_ERROR;
    }

    // Test busy
    while (is_busy() == TRUE_T) {
        clear_wdt();
    }

    // Test sector protect
    if (is_sector_protect(_pData->addr) == TRUE_T) {
        unprotect_sector(_pData->addr);

        if (is_sector_protect(_pData->addr) == TRUE_T) {
            return eMEM_WRITE_EN_ERROR;
        }
    }

    // Test write enable bit
    if (is_write_enable() == FALSE_T) {
        write_enable();

        if (is_write_enable() == FALSE_T) {
            return eMEM_WRITE_EN_ERROR;
        }
    }

    select_chip();

    // Write cmd
    SPI_BUFF = eAT25DF_PRGR_PAGE;

    // Write address
    for (uint32_t i = 0, k = 2; i < 3;) {
        if (SPI_FREE) {
            SPI_BUFF = _pData->addr.addr_array[k--];
            i++;
        }
    }

    // Write data
    for (uint32_t i = 0; i < (_pData->len); ) {
        if (SPI_FREE) {
            SPI_BUFF = _pData->pBuff[i++];
        }
    }
    while (!(SPI_FREE));

    lock_chip();

    return eMEM_OK;
}


/*
 * Block erase
 *
 */
uint32_t block_erase_at25df(const __AT25DF_ADDRESS _addr, const __eAt25DF_BLOCK _block) {
    // Test flash memory is Ok
    if (m_is_flash_ok != TRUE_T) {
        return eMEM_NOT_AVAILABLE;
    }

    // Validate addr
    if (_addr.addr32 >= AT25DF_FLASH_SIZE) {
        return eMEM_ADDR_ERROR;
    }

    // Test busy
    while (is_busy() == TRUE_T) {
        clear_wdt();
    }

    // Test sector protect
    if (is_sector_protect(_addr) == TRUE_T) {
        unprotect_sector(_addr);

        if (is_sector_protect(_addr) == TRUE_T) {
            return eMEM_WRITE_EN_ERROR;
        }
    }

    // Test write enable bit
    if (is_write_enable() == FALSE_T) {
        write_enable();

        if (is_write_enable() == FALSE_T) {
            return eMEM_WRITE_EN_ERROR;
        }
    }

    // Select block
    __eAT25DF_INSTRUCTIONS instr;
    switch (_block) {
        case eAT25DF_BLOCK_4:
          instr = eAT25DF_BLOCK_4_ERASE;
        break;

        case eAT25DF_BLOCK_32:
          instr = eAT25DF_BLOCK_32_ERASE;
        break;

        case eAT25DF_BLOCk_64:
          instr = eAT25DF_BLOCK_64_ERASE;
        break;

        default:
          return eMEM_INTERNAL_ERROR;
    }

    select_chip();

    // Write cmd
    SPI_BUFF = instr;

    // Write address
    for (uint32_t i = 0, k = 2; i < 3;) {
        if (SPI_FREE) {
            SPI_BUFF = _addr.addr_array[k--];
            i++;
        }
    }
    while (!(SPI_FREE));

    lock_chip();

    return eMEM_OK;
}


/*
 * Chip erase
 *
 */
uint32_t chip_erase_at25df(const uint32_t _password) {
    // Test flash memory is Ok
    if (m_is_flash_ok != TRUE_T) {
        return eMEM_NOT_AVAILABLE;
    }

    // Test psw
    if (_password != AT25DF_CHIP_ERASE_PSW) {
        return eMEM_INTERNAL_ERROR;
    }

    // Test busy
    while (is_busy() == TRUE_T) {
        clear_wdt();
    }

    // Need global unprotect

    // Test write enable bit
    if (is_write_enable() == FALSE_T) {
        write_enable();

        if (is_write_enable() == FALSE_T) {
            return eMEM_WRITE_EN_ERROR;
        }
    }

    select_chip();

    // Write cmd
    SPI_BUFF = eAT25DF_CHIP_ERASE;
    while (!(SPI_FREE));

    lock_chip();

    return eMEM_OK;
}
