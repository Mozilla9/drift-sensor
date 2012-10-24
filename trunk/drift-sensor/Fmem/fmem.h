/******************************************************************************/
/* fmem.h                                                                     */
/******************************************************************************/


#ifndef __FMEM_H
#define __FMEM_H


/********************* Macros & Constants for FMEM ****************************/
#define FMEM_PAGE_SIZE    AT25DF_PAGE_SIZE
#define FMEM_SECTOR_SIZE  AT25DF_SECTOR_SIZE_4
#define FMEM_ALLOC_FLAG   (0xAA55AA55)


/************************** Prototypes & struct for FMEM **********************/
typedef struct
FMEM_SETT
{
    const uint32_t page_size;
    const uint32_t sector_size;
    const uint32_t start_addr;
    const uint32_t allocated_size;
} __FMEM_SETT;


typedef struct
FMEM_DATA
{
    uint32_t addr;
    uint8_t * pBuff;
    uint32_t len;
} __FMEM_DATA;


/******************************** Api func ************************************/
uint32_t write_data_fmem(const __FMEM_SETT * const pSett, const __FMEM_DATA * const pData);
uint32_t change_data_fmem(const __FMEM_SETT * const pSett, const __FMEM_DATA * const pData);
uint32_t read_data_fmem(const __FMEM_SETT * const pSett, const __FMEM_DATA * const pData);
uint32_t erase_memory_fmem(const __FMEM_SETT * const pSett);
uint32_t get_space_to_end_fmem(const __FMEM_SETT * const pSett, const uint32_t addr);


#endif