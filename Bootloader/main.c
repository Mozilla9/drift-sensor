/******************************************************************************/
/*  Project "Bootloader"                                                      */
/*  Soft: S. Maslyakov, rusoil.9@gmail.com                                    */
/*                                                                            */
/*  Revision:     0.1.0                                                       */
/*  Date:         2012/10/22 00:06:33                                         */
/******************************************************************************/


#include "data_types.h"
#include "Core\core.h"
#include "Uart\v_printf.h"
#include "At25df\at25df.h"
#include "Ringbuff\ring_buffer.h"
#include "Sdp\sdp.h"
#include "Tasks\tasks.h"
#include "Bootloader\bootloader.h"



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
        if (is_ring_buff_empty(pointerRingBuff) == FALSE_T) {
            sdp_insert_char(get_from_ring_buff(pointerRingBuff));
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