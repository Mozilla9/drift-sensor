/******************************************************************************/
/* msdp.h                                                                     */
/******************************************************************************/

#ifndef __MSDP_H
#define __MSDP_H



typedef struct
{
    const uint8_t * pBuff;
    uint16_t len;
} __msdp_out_data;


typedef struct
{
    uint32_t addr;
    uint8_t * pBuff;
    uint16_t len;
} __msdp_wr_data;


void erase_pg_flash_msdp(__msdp_out_data * const pOutData);
void erase_sett_flash_msdp(__msdp_out_data * const pOutData);
void write_pg_flash_msdp(__msdp_out_data * const pOutData, const __msdp_wr_data * const pWrData);
void write_sett_flash_msdp(__msdp_out_data * const pOutData, const __msdp_wr_data * const pWrData);
void invoke_code_msdp(__msdp_out_data * const pOutData, const uint32_t address);
void invoke_user_cmd_msdp(__msdp_out_data * const pOutData, const __msdp_wr_data * const pWrData);

#endif
