/******************************************************************************/
/* at25df_def.h                                                               */
/******************************************************************************/

#ifndef __AT25DF_DEF_H
#define __AT25DF_DEF_H


/********************* Macros & Constants for AT25DF **************************/
#define AT25DF_FLASH_SIZE       (0x00200000)       // AT25DF161A  16-mbit

#define AT25DF_PAGE_SIZE        (256)

#define AT25DF_SECTOR_SIZE_4    (4096)
#define AT25DF_SECTOR_SIZE_32   (32768)
#define AT25DF_SECTOR_SIZE_64   (65536)

#define AT25DF_PAGE_QNT         (AT25DF_FLASH_SIZE / AT25DF_PAGE_SIZE)

#define ATMEL_MANUF_ID          (0x1F)            // Atmel
#define AT25DF_DEV_ID1          (0x47)            // 0_1_0__0_0_1_1_0
#define AT25DF_DEV_ID2          (0x01)            // Product version

#define AT25DF_BUSY             (0x0001)          // Ready/Busy status
#define AT25DF_WEL              (0x0002)          // Write enable latch bit
#define AT25DF_EPE              (0x0010)          // Erase/program error
#define AT25DF_ESS              (0x0200)          // Erase suspend status
#define AT25DF_PSS              (0x0400)          // Program suspend status

#define AT25DF_CHIP_ERASE_PSW   (0xAA55AA55)      // User psw for chip erase

#define AT25DF_SPI_READ_CLK     (0xAA)


/************************ Prototypes & struct for AT25DF **********************/
typedef enum
{
    eATMEL_DATAFLAFH = 1,
    eATMEL_AT25DF_26DF,
    eATMEL_AT25F_25FS,
    eATMEL_QUAD
} __eATMEL_FAMILY_CODE;


typedef enum
{
    eATMEL_01M = 2,
    eATMEL_02M,
    eATMEL_04M,
    eATMEL_08M,
    eATMEL_16M,
    eATMEL_32M,
    eATMEL_64M
} __eATMEL_DENSITY_CODE;


typedef enum
{
    eAT25DF_BLOCK_4 = 0,
    eAT25DF_BLOCK_32,
    eAT25DF_BLOCk_64
} __eAt25DF_BLOCK;


typedef enum
{
    eMEM_OK = 0,
    eMEM_BUSY,
    eMEM_NOT_AVAILABLE,
    eMEM_ADDR_ERROR,
    eMEM_DATA_ERROR,
    eMEM_WRITE_EN_ERROR,
    eMEM_MUTEX_ERROR,
    eMEM_INTERNAL_ERROR
} __eMEM_ERROR_CODE;


typedef enum
{
    eAT25DF_PRGR_PAGE        = 0x02,       // page program ~ 1-3 ms
    eAT25DF_READ_DATA        = 0x03,       // read data bytes
    eAT25DF_WRITE_DIS        = 0x04,       // write disable
    eAT25DF_READ_SREG        = 0x05,       // read status register
    eAT25DF_WRITE_EN         = 0x06,       // write enable
    eAT25DF_BLOCK_4_ERASE    = 0x20,       // erase block 4 kb ~ 50-200 ms
    eAT25DF_PROTECT_SECTOR   = 0x36,       // protect sector (65k b)
    eAT25DF_UNPROTECT_SECTOR = 0x39,       // unprotect sector (65k b)
    eAT25DF_READ_PROT_REG    = 0x3C,       // read sector unprotection register
    eAT25DF_BLOCK_32_ERASE   = 0x52,       // erase block 32 kb ~ 250-600 ms
    eAT25DF_READ_ID          = 0x9F,       // read id
    eAT25DF_CHIP_ERASE       = 0xC7,       // chip erase ~ 25-40 sec
    eAT25DF_BLOCK_64_ERASE   = 0xD8        // erase block 64 kb ~ 400-950 ms
} __eAT25DF_INSTRUCTIONS;


typedef union
AT25DF_ADDRESS
{
    uint32_t addr32;
    uint8_t addr_array[4];
} __AT25DF_ADDRESS;


typedef struct
AT25DF_DATA
{
    __AT25DF_ADDRESS addr;
    uint8_t * pBuff;
    uint32_t len;
} __AT25DF_DATA;


#endif