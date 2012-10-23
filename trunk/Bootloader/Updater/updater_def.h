/******************************************************************************/
/* updater_def.h                                                              */
/******************************************************************************/


#ifndef __UPDATER_DEF_H
#define __UPDATER_DEF_H



/************************** Prototypes & struct for UPDATER *******************/
typedef void (* pFW_ENTRY)();


typedef struct
{
    uint32_t flg_ok;
    uint32_t fw_size;
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



#endif