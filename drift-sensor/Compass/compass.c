/******************************************************************************/
/* compass.c                                                                  */
/******************************************************************************/


#include "data_types.h"
#include "Core\core.h"
#include "Uart\uart0.h"
#include "Led\led.h"
#include "Uart\v_printf.h"
#include "I2C\i2c0.h"
#include "At25df\at25df.h"
#include "Ringbuff\ring_buffer.h"
#include "Lis3dh\lis3dh.h"
#include "Compass\compass.h"


static volatile uint32_t sys_tick = 0;
static uint32_t device_id = 0;



/*
 * Get device id
 *
 */
uint32_t get_device_id() {
    return device_id;
}


/*
 * Set device id
 *
 */
void set_device_id(const uint32_t id) {
    device_id = id;
}


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

    VICINTSELECT_bit.I2C0 = 0;
    VICVECTPRIORITY9_bit.PRIORITY = 9;

    VICVECTADDR4 = (uint32_t)__interrupt_handler_timer0;
    VICVECTADDR6 = (uint32_t)__interrupt_handler_uart0;
    VICVECTADDR9 = (uint32_t)__interrupt_handler_i2c0;

    VICINTENABLE_bit.UART0 = 1;
    VICINTENABLE_bit.TIMER0 = 1;
    VICINTENABLE_bit.TIMER1 = 1;
    VICINTENABLE_bit.I2C0 = 1;
}


/*
 * Init device
 *
 */
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
    init_lis3dh();
    init_ring_buff(pointerRingBuff);

    init_uart0(57600, 3);
    INT_UART0RX_ON;

    init_irq();
}
