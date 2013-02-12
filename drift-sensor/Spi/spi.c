/*
 * $Id$
 */


#include "data_types.h"
#include "Spi\spi.h"



/*
 * Init SPI
 *
 */
void init_spi() {
    uint32_t temp;

    PCONP_bit.PCSPI = 1;

    PINSEL0_bit.P0_15 = 3;            // SCK
    PINSEL1_bit.P0_17 = 3;            // MISO
    PINSEL1_bit.P0_18 = 3;            // MOSI

    FIO0DIR_bit.P0_15 = 1;            // SCK as output
    FIO0DIR_bit.P0_17 = 0;            // MISO as input
    FIO0DIR_bit.P0_18 = 1;            // MOSI

    PINMODE0_bit.P0_15 = 0;           // 0=PULL_UP, 1=RESERVED, 2=PULL_DIS, 3=PULL_DOWN
    PINMODE1_bit.P0_17 = 2;           //
    PINMODE1_bit.P0_18 = 0;           //

    S0SPCR_bit.BITENABLE = 0;         // 8 data bits
    S0SPCR_bit.CPHA = 0;              // rising front
    S0SPCR_bit.CPOL = 0;              // positive inmpulse polarity
    S0SPCR_bit.MSTR = 1;              // master mode
    S0SPCR_bit.LSBF = 0;              // MSB first
    S0SPCR_bit.SPIE = 0;              // int dis
    S0SPCR_bit.BITS = 8;

    S0SPCCR_bit.COUNTER = 8;          // Freq SPI = PCLK / 8
    S0SPINT_bit.SPIINT = 0;           // Clear int

    temp = S0SPSR;                    // Clear status reg
    temp = S0SPDR;
}
