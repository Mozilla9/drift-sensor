/******************************************************************************/
/*  Project "Compass" drift sensor                                            */
/*  Soft: S. Maslyakov, rusoil.9@gmail.com                                    */
/*                                                                            */
/*  Revision:     1.1.0                                                       */
/*  Date:         2012/10/08 01:03:33                                         */
/******************************************************************************/

#include "data_types.h"
#include "Core\core.h"
#include "At25df\at25df.h"
#include "Uart\v_printf.h"
#include "Lis3dh\lis3dh.h"
#include "Compass\compass.h"
#include "Ringbuff\ring_buffer.h"
#include "Drift\drift.h"
#include "Microrl\handlers.h"
#include "Microrl\microrl.h"
#include "Sett\settings.h"
#include "Tasks\tasks.h"


#define device_vers_tag    "\n\rDriftSensor v.13.3\n\r"


/******************************************************************************/
/*                                                                            */
/*      main                                                                  */
/*                                                                            */
/******************************************************************************/
void main()
{
    __disable_interrupt();

    init_core();
    init_device();
    init_wdt();

    __enable_interrupt();

    microrl_init (pointerMicrorl, &serprintf);
    microrl_set_execute_callback (pointerMicrorl, execute);
    microrl_set_complete_callback (pointerMicrorl, complet);
    microrl_set_sigint_callback (pointerMicrorl, sigint);

    init_app_settings();
    enable_default_lis3dh();
    init_tasks();
    init_reco_drift();
    
    DEBUG_PRINTF(device_vers_tag);
    DEBUG_PRINTF("flash mem: %s %s %s\n\r",
        get_flash_status_at25df() ? "on" : "off",
        get_family_desc_at25df(),
        get_density_desc_at25df());
        
    DEBUG_PRINTF("\r\n\r\n");

    while (1) {
        if (is_ring_buff_empty(pointerRingBuff) == FALSE_T) {
            microrl_insert_char (pointerMicrorl, get_from_ring_buff(pointerRingBuff));
        }

        run_tasks();

        clear_wdt();
    }
}
/******************************************************************************/
/*                                                                            */
/*      The End                                                               */
/*                                                                            */
/******************************************************************************/
