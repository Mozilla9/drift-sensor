/*
 * $Id$
 */

#ifndef __AT25DF_H
#define __AT25DF_H


#include "At25df\at25df_def.h"


void init_at25df();
bool_t get_flash_status_at25df();
uint8_t get_family_code_at25df();
uint8_t get_density_code_at25df();
const int8_t * get_family_desc_at25df();
const int8_t * get_density_desc_at25df();
uint32_t read_data_bytes_at25df(const __AT25DF_DATA * const _pData);
uint32_t program_page_at25df(const __AT25DF_DATA * const _pData);
uint32_t chip_erase_at25df(const uint32_t _password);
uint32_t block_erase_at25df(const __AT25DF_ADDRESS _addr, const __eAt25DF_BLOCK _block);



#endif