/******************************************************************************/
/*  Project "Bootloader"                                                      */
/*  Soft: S. Maslyakov, rusoil.9@gmail.com                                    */
/*                                                                            */
/******************************************************************************/

/*
 * $Id$
 */


#include "data_types.h"
#include "Core\core.h"
#include "Uart\v_printf.h"
#include "At25df\at25df.h"
#include "Ringbuff\ring_buffer.h"
#include "Sdp\sdp.h"
#include "Tasks\tasks.h"
#include "Bootloader\bootloader.h"


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

    DEBUG_PRINTF(device_vers_tag);
    DEBUG_PRINTF("FlashMem: %s %s\n\r",
        get_family_desc_at25df(),
        get_density_desc_at25df());


    DEBUG_PRINTF("\r\n\r\n");

    while (1) {
        if (pointerRingBuff->size(pointerRingBuff) > 0) {
            sdp_insert_char(pointerRingBuff->get(pointerRingBuff));
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