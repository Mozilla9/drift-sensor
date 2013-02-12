/*
 * $Id$
 */


#ifndef __SPI_H
#define __SPI_H


#define SPI_BUFF                S0SPDR            // Hw shift-reg SPI-module
#define SPI_FREE                S0SPSR_bit.SPIF   // Macros for testing free SPI



void init_spi();


#endif