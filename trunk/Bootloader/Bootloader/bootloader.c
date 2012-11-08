/******************************************************************************/
/* bootloader.c                                                               */
/******************************************************************************/


#include "data_types.h"
#include "Core\core.h"
#include "Uart\uart0.h"
#include "Led\led.h"
#include "Uart\v_printf.h"
#include "At25df\at25df.h"
#include "Ringbuff\ring_buffer.h"
#include "Updater\updater.h"
#include "Tasks\tasks.h"
#include "Bootloader\bootloader.h"


//__root static const uint32_t CRP_MODE   @ ".CRP_CODE" = CRP3;
__root static const uint32_t FW_CFG[]   @ ".FW_CFG" =
{
    0x0000CA70,     // main fw len
    0xffb81cc0,     // main fw crc
    IAP_VALID_DATA
};


static volatile uint32_t sys_tick = 0;

static uint8_t ring_buff[64];
static __ring_buff r_buff;
__ring_buff * const pointerRingBuff = &r_buff;



/*
 * Get sys tick
 *
 */
uint32_t get_sys_tick() {
    return sys_tick;
}


/*
 * Get time
 *
 */
uint32_t get_time() {
    return sys_tick / TIMER0_TICK;
}


/*
 * Timer0 interrupt handler
 *
 */
static __irq __arm void __interrupt_handler_timer0() {
    sys_tick++;

    T0IR = 0x01;
    VICADDRESS = 0;
}


/*
 * Init interrupt
 *
 */
static void init_irq() {
    uint32_t *vect_addr, *vect_cntl;

    // VIC in user mode
    VICPROTECTION_bit.VIC_ACCESS = 0;
    VICINTENCLEAR = 0xffffffff;
    VICADDRESS = 0L;
    VICINTSELECT = 0L;

    for (uint32_t i = 0; i < VIC_SIZE; i++) {
        vect_addr = (uint32_t *)(VIC_BASE_ADDR + VECT_ADDR_INDEX + i*4);
        vect_cntl = (uint32_t *)(VIC_BASE_ADDR + VECT_CNTL_INDEX + i*4);
        *vect_addr = 0x0;
        *vect_cntl = 0xF;
    }

    VICINTSELECT_bit.TIMER0 = 0;
    VICVECTPRIORITY4_bit.PRIORITY = 13;

    VICINTSELECT_bit.UART0 = 0;
    VICVECTPRIORITY6_bit.PRIORITY = 9;

    VICVECTADDR4 = (uint32_t)__interrupt_handler_timer0;
    VICVECTADDR6 = (uint32_t)__interrupt_handler_uart0;

    VICINTENABLE_bit.UART0 = 1;
    VICINTENABLE_bit.TIMER0 = 1;
}


void init_device() {
    PCONP_bit.PCUART0 = 0;
    PCONP_bit.PCPWM1 = 0;
    PCONP_bit.PCI2C0 = 0;
    PCONP_bit.PCSSP1 = 0;
    PCONP_bit.PCAN1 = 0;
    PCONP_bit.PCAN2 = 0;
    PCONP_bit.PCI2C1 = 0;
    PCONP_bit.PCSSP0 = 0;
    PCONP_bit.PCTIM2 = 0;
    PCONP_bit.PCTIM3 = 0;
    PCONP_bit.PCUART2 = 0;
    PCONP_bit.PCI2C2 = 0;
    PCONP_bit.PCI2S = 0;
    PCONP_bit.PCGPDMA = 0;
    PCONP_bit.PCENET = 0;

    init_timer0();
    init_at25df();
    init_led();
    init_updater();
    init_tasks();
    create_ring_buff(pointerRingBuff, ring_buff, sizeof(ring_buff));

    init_uart0(57600, 3);
    INT_UART0RX_ON;

    // debug
    set_trace(serprintf);

    init_irq();

    // Testing main pin (for locked mode)
    if (FIO2PIN_bit.P2_10 == 0) {
        return;
    }

    // Fork
    if (is_need_update() == TRUE_T) {
        enable_update_task();
    } else {
        if (*((uint32_t *)(IAP_MAIN_SECT_ADDR + SHIFT_FW_VALID_FLG)) != IAP_VALID_DATA) {
            repair_flash_iap();
        }

        uint32_t success_count = 0;
        const uint32_t fw_size = *((uint32_t *)(IAP_MAIN_SECT_ADDR + SHIFT_FW_SIZE_ADDR));
        const uint32_t fw_crc = *((uint32_t *)(IAP_MAIN_SECT_ADDR + SHIFT_FW_CRC_ADDR));

        for (uint32_t i = 0; i < 3; i++) {
            uint8_t * pFw = (uint8_t *)FW_START_ADDRESS;
            uint32_t crc = 0;

            for (uint32_t k = 0; k < fw_size; k++) {
                crc += (uint32_t)(*pFw++);
            }

            crc = 0UL - crc;
            if (fw_crc == crc) {
                success_count++;
                break;
            }
        }

        if (success_count > 0 && fw_crc) {
            run_main_firmware();
        }
    }
}
