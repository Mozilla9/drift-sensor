/******************************************************************************/
/* settings.c                                                                 */
/******************************************************************************/

#include "data_types.h"
#include "Core\core.h"
#include "Uart\v_printf.h"
#include "At25df\at25df.h"
#include "Sett\settings.h"


static const __FMEM_SETT app_sett = {FMEM_PAGE_SIZE, FMEM_SECTOR_SIZE, GSETT_START_ADDR, GSETT_MEM_SIZE};


/*
 * Init sett, test for def sett
 *
 */
void init_app_settings() {
    __FMEM_DATA data;
    uint32_t def_flag;

    // read main flag
    def_flag = 0;
    data.addr = DEF_SETT_FLAG_ADDR;
    data.pBuff = (uint8_t *) (&def_flag);
    data.len = sizeof(def_flag);
    read_data_fmem(&app_sett, &data);

    if (def_flag == DEF_SETT_FLAG) {
        // trace func
        data.addr = DEV_EN_TRACE_ADDR;
        read_app_settings(&data);
        set_trace(def_flag ? &serprintf : 0);
    }
}


/*
 * Read sett
 *
 */
uint32_t read_app_settings(const __FMEM_DATA * const pReadData) {
    return read_data_fmem(&app_sett, pReadData);
}
