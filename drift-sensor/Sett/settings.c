/******************************************************************************/
/* settings.c                                                                 */
/******************************************************************************/

#include "data_types.h"
#include "Core\core.h"
#include "At25df\at25df.h"
#include "Fmem\fmem.h"
#include "Fmem\fmem_map.h"
#include "Sett\settings.h"


static const __FMEM_SETT app_sett = {FMEM_PAGE_SIZE, FMEM_SECTOR_SIZE, GSETT_START_ADDR, GSETT_MEM_SIZE};
static const __FMEM_SETT app_swap_sett = {FMEM_PAGE_SIZE, FMEM_SECTOR_SIZE, GSETT_SWAP_START_ADDR, GSETT_MEM_SIZE};


/*
 * Init sett, test for def sett
 *
 */
void load_app_settings() {


}


/*
 * Load def sett
 *
 */
void sett_def_app_settings() {


}