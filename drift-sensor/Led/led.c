/*
 * $Id$
 */


#include "data_types.h"
#include "Core\core.h"
#include "Led\led_def.h"
#include "Led\led.h"



/*
 * Init led pins
 *
 */
void init_led() {

    PINSEL3_bit.P1_17 = 0;         // LED_2 GPIO

    FIO1DIR_bit.P1_17 = 1;         // output

    PINMODE3_bit.P1_17 = 2;        // 0=PULL_UP, 1=RESERVED, 2=PULL_DIS, 3=PULL_DOWN

    led1_off();
}


/*
 * led1 on
 *
 */
void led1_on() {
    LED_1_ON;
}


/*
 * led1 off
 *
 */
void led1_off() {
    LED_1_OFF;
}


/*
 * get led1 state
 *
 */
uint16_t get_led1_state() {
    return LED_1_STATE ? 1 : 0;
}
