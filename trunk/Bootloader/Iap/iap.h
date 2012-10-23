/******************************************************************************/
/* iap.h                                                                      */
/******************************************************************************/


#ifndef __IAP_H
#define __IAP_H



typedef void (* pIAP_ENTRY)(uint32_t [], volatile uint32_t [] );



__arm uint32_t blank_check_sector_iap(const uint32_t start_sect, const uint32_t end_sect);
__arm uint32_t prepare_sector_iap(const uint32_t start_sect, const uint32_t end_sect);
__arm uint32_t erase_sector_iap(const uint32_t start_sect, const uint32_t end_sect);
__arm uint32_t copy_ram_to_flash_iap(const uint32_t dst_addr, const uint32_t src_addr, const uint32_t len);
__arm uint32_t compare_iap(const uint32_t dst_addr, const uint32_t src_addr, const uint32_t len);
__arm uint32_t read_part_id_iap(uint32_t * dst);
__arm uint32_t read_bl_vers_iap(uint32_t * dst);



#endif