/******************************************************************************/
/*  Project "Compass" drift sensor                                            */
/*  Soft: S. Maslyakov, rusoil.9@gmail.com                                    */
/*                                                                            */
/*  Revision:     13.4.0                                                      */
/*  Date:         2012/10/28 22:13:33                                         */
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
#include "Sdp\sdp.h"
#include "Sett\settings.h"
#include "Tasks\tasks.h"


#define device_vers_tag    "\n\rDriftSensor v.13.4\n\r"

extern __ring_buff * const pointerRingBuff;


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

    DEBUG_PRINTF(device_vers_tag);
    DEBUG_PRINTF("FlashMem: %s %s\n\r",
        get_family_desc_at25df(),
        get_density_desc_at25df());

    enable_default_lis3dh();
    init_tasks();
    init_reco_drift();

    DEBUG_PRINTF("\r\n\r\n");

    while (1) {
        if (is_ring_buff_empty(pointerRingBuff) == FALSE_T) {
            const uint8_t data = get_from_ring_buff(pointerRingBuff);
            
            if (!get_proto_type()) {
                microrl_insert_char (pointerMicrorl, data);
            } else {
                sdp_insert_char(data);
            }
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
