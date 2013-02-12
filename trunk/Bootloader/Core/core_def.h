/*
 * $Id$
 */

#ifndef __CORE_DEF_H
#define __CORE_DEF_H


/******************************** Macros for core *****************************/
#define   FOSC              (12000000)      // Oscill 12 Mhz

#define   PLL_MValue        (11)            // Fcclk = 57.6Mhz, Fpllclk = 288Mhz, and USB = 48Mhz
#define   PLL_NValue        (0)
#define   CCLK_DIV          (4)
#define   USB_CLK_DIV       (5)

#define   FCCLK             (57600000)      // Freq CPU in Hz
#define   FCCLK_kHz         (FCCLK/1000)    // Freq CPU in kHz
#define   FPLLCLK           (288000000)     // Freq at PLL
#define   FPCLK             (FCCLK/2)       // Freq peripherals

#define   WDT_VALUE         (0x02255100)    // Timeout = Tfpclk * WDT_VALUE * 4


/************************ Constants for timers ********************************/
#define   TIMER0_TICK       (1000L)                   // Tick per sec
#define   TIMER0_VALUE      (FPCLK/TIMER0_TICK)       // Interval = 1 msec

#define   TIMER1_TICK       (1000000L)                // Tick per sec
#define   TIMER1_VALUE      (FPCLK/TIMER1_TICK)       // Interval = 1 usec


/****************************** Macros for IRQ ********************************/
#define   I_Bit             (0x80)
#define   F_Bit             (0x40)

#define   SYS32Mode         (0x1F)
#define   IRQ32Mode         (0x12)
#define   FIQ32Mode         (0x11)

#define   HIGHEST_PRIORITY  (0x01)
#define   LOWEST_PRIORITY   (0x0F)

#define   WDT_INT           (0)
#define   SWI_INT           (1)
#define   ARM_CORE0_INT     (2)
#define   ARM_CORE1_INT     (3)
#define   TIMER0_INT        (4)
#define   TIMER1_INT        (5)
#define   UART0_INT         (6)
#define   UART1_INT         (7)
#define   PWM0_1_INT        (8)
#define   I2C0_INT          (9)
#define   SPI0_INT          (10)      /* SPI and SSP0 share VIC slot */
#define   SSP0_INT          (10)
#define   SSP1_INT          (11)
#define   PLL_INT           (12)
#define   RTC_INT           (13)
#define   EINT0_INT         (14)
#define   EINT1_INT         (15)
#define   EINT2_INT         (16)
#define   EINT3_INT         (17)
#define   ADC0_INT          (18)
#define   I2C1_INT          (19)
#define   BOD_INT           (20)
#define   EMAC_INT          (21)
#define   USB_INT           (22)
#define   CAN_INT           (23)
#define   MCI_INT           (24)
#define   GPDMA_INT         (25)
#define   TIMER2_INT        (26)
#define   TIMER3_INT        (27)
#define   UART2_INT         (28)
#define   UART3_INT         (29)
#define   I2C2_INT          (30)
#define   I2S_INT           (31)

#define   VIC_SIZE          (32)

#define   VIC_BASE_ADDR     ((uint32_t)&VICIRQSTATUS)

#define   VECT_ADDR_INDEX   (0x100)
#define   VECT_CNTL_INDEX   (0x200)


#endif