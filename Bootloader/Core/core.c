/*
 * $Id$
 */


#include "data_types.h"
#include "Core\core.h"


static uint32_t g_istate;



/*
 * Init mam
 *
 */
static void init_mam() {
    MAMCR_bit.MODECTRL = 0; // Dis MAM
    MAMTIM_bit.CYCLES = 3;
    MAMCR_bit.MODECTRL = 2; // En MAM
}


/*
 * Init PLL - external
 *
 */
void init_pll() {
    // Dis PLL
    PLLCON_bit.PLLE = 0;
    PLLCON_bit.PLLC = 0;
    PLLFEED_bit.FEED = 0xAA;
    PLLFEED_bit.FEED = 0x55;

    // En main oscill
    //SCS_bit.MCIPWR = 0;      //
    SCS_bit.OSCRANGE = 0;      // 1-20 Mhz
    SCS_bit.OSCEN = 1;         // Ext oscill en

    while(!SCS_bit.OSCSTAT);   // Waiting

    // Set suply for PLL
    CLKSRCSEL_bit.CLKSRC = 1;  // Main oscill

    // Configure PLL
    PLLCFG_bit.MSEL = PLL_MValue;
    PLLCFG_bit.NSEL = PLL_NValue;
    PLLFEED_bit.FEED = 0xAA;
    PLLFEED_bit.FEED = 0x55;

    // En PLL, but dont connect to CPU
    PLLCON_bit.PLLE = 1;
    PLLFEED_bit.FEED = 0xAA;
    PLLFEED_bit.FEED = 0x55;

    // Set dividers for CPU and USB
    CCLKCFG_bit.CCLKSEL = CCLK_DIV;
    USBCLKCFG_bit.USBSEL = USB_CLK_DIV;

    // Configure PCLK
    //PCLKSEL0 = 0x00000000;     // PCLK = CCKL/4
    PCLKSEL0 = 0xAAAAAAAA;     // PCLK = CCKL/2 for all modules
    PCLKSEL1 = 0xAAAAAAAA;

    while(!PLLSTAT_bit.PLOCK);

    // Connect and en PLL
    PLLCON_bit.PLLE = 1;
    PLLCON_bit.PLLC = 1;
    PLLFEED_bit.FEED = 0xAA;
    PLLFEED_bit.FEED = 0x55;

    while(!PLLSTAT_bit.PLOCK);   // Waiting capture pll
}


/*
 * Init general pinouts
 *
 */
static void init_gpio() {
    SCS_bit.GPIOM = 1;

    // Reset all GPIO pins to default: primary function
    PINSEL0 = 0x00000000;
    PINSEL1 = 0x00000000;
    PINSEL2 = 0x00000000;
    PINSEL3 = 0x00000000;
    PINSEL4 = 0x00000000;
    PINSEL5 = 0x00000000;
    PINSEL6 = 0x00000000;
    PINSEL7 = 0x00000000;
    PINSEL8 = 0x00000000;
    PINSEL9 = 0x00000000;
    PINSEL10 = 0x00000000;

    IO0DIR = 0x00000000;
    IO1DIR = 0x00000000;
    IO0SET = 0x00000000;
    IO1SET = 0x00000000;

    FIO0DIR = 0x00000000;
    FIO1DIR = 0x00000000;
    FIO2DIR = 0x00000000;
    FIO3DIR = 0x00000000;
    FIO4DIR = 0x00000000;

    FIO0CLR = 0xFFFFFFFF;
    FIO1CLR = 0xFFFFFFFF;
    FIO2CLR = 0xFFFFFFFF;
    FIO3CLR = 0xFFFFFFFF;
    FIO4CLR = 0xFFFFFFFF;
}


/*
 * Init WatchDog
 *
 */
void init_wdt() {
    WDTC = WDT_VALUE;         // 5 sec
    WDCLKSEL_bit.WDSEL = 1;   // Suply of PCLK
    WDMOD = 3;                // wdt enable

    // Feed WDT
    WDFEED = 0xAA;
    WDFEED = 0x55;
}


/*
 * Critical section - enter
 *
 */
__arm void enter_cs() {
    g_istate = __get_interrupt_state();
    __disable_interrupt();
}


/*
 * Critical section - exit
 *
 */
__arm void exit_cs() {
    __set_interrupt_state(g_istate);
}


/*
 * Clear WDT
 *
  */
__arm void clear_wdt() {
    enter_cs();

    // feed WDT
    WDFEED = 0xAA;
    WDFEED = 0x55;

    exit_cs();
}


/*
 * Unsafe clear WDT
 *
  */
__arm void unsafe_clear_wdt() {
    // feed WDT
    WDFEED = 0xAA;
    WDFEED = 0x55;
}


/*
 * Reset device by wdt
 *
 */
__arm void reset_device() {
    // error feed WDT
    WDFEED = 0x55;
    WDFEED = 0xAA;
}


/*
 * Init TIMER0
 *
 */
void init_timer0() {
    PCONP_bit.PCTIM0 = 1;

    // Reset and halt
    T0TCR_bit.CE = 0;
    T0TCR_bit.CR = 1;

    // Configure prescaler
    T0PR = 0x00;
    T0PC = 0x00;

    // Set mode
    T0CTCR_bit.CTM = 0;   // timer mode
    T0MCR_bit.MR0I = 1;   // int by compare
    T0MCR_bit.MR0R = 1;   // clear TC in compare
    T0MCR_bit.MR0S = 0;   // continuous count

    // Interval
    T0MR0 = TIMER0_VALUE;

    // Run
    T0TCR_bit.CE = 1;
    T0TCR_bit.CR = 0;
}


/*
 * Init TIMER1
 *
 */
void init_timer1() {
    PCONP_bit.PCTIM1 = 1;

    // Reset and halt
    T1TCR_bit.CE = 0;
    T1TCR_bit.CR = 1;

    // Configure prescaler
    T1PR = 0x00;
    T1PC = 0x00;

    // Set mode
    T1CTCR_bit.CTM = 0;   // timer mode
    T1MCR_bit.MR0I = 1;   // int by compare
    T1MCR_bit.MR0R = 1;   // clear TC in compare
    T1MCR_bit.MR0S = 0;   // continuous count

    // Interval
    T1MR0 = TIMER1_VALUE;

    // Run
    T1TCR_bit.CE = 1;
    T1TCR_bit.CR = 0;
}


/*
 * Init Core
 *
 */
void init_core() {
    init_gpio();
    init_pll();
    init_mam();
}
