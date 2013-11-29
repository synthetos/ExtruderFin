/*
 * xio_spi.h	- General purpose SPI device driver for xmega family
 * 				- works with avr-gcc stdio library
 * Part of Kinen project
 *
 * Copyright (c) 2012 - 2013 Alden S. Hart Jr.
 *
 * This file ("the software") is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2 as published by the
 * Free Software Foundation. You should have received a copy of the GNU General Public
 * License, version 2 along with the software.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, you may use this file as part of a software library without
 * restriction. Specifically, if other files instantiate templates or use macros or
 * inline functions from this file, or you compile this file and link it with  other
 * files to produce an executable, this file does not by itself cause the resulting
 * executable to be covered by the GNU General Public License. This exception does not
 * however invalidate any other reasons why the executable file might be covered by the
 * GNU General Public License.
 *
 * THE SOFTWARE IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL, BUT WITHOUT ANY
 * WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef xio_spi_h
#define xio_spi_h

/******************************************************************************
 * SPI DEVICE CONFIGS AND STRUCTURES
 ******************************************************************************/

//#define SPI_MODE		(1<<SPIE | 1<<SPE)						// mode 0 operation / slave
#define SPI_MODE		(1<<SPIE | 1<<SPE | 1<<CPOL | 1<<CPHA)	// mode 3 operation / slave
#define SPI_OUTBITS		(1<<DDB4)			// Set SCK, MOSI, SS to input, MISO to output
#define SPI_XIO_FLAGS 	(XIO_LINEMODE)

// Buffer structs must be the same as xioBuf, except that the buf array size is defined.
#define SPI_RX_BUFFER_SIZE 32
#define SPI_TX_BUFFER_SIZE 32

typedef struct xioSpiRX {
	buffer_t size;							// initialize to SPI_RX_BUFFER_SIZE-1
	volatile buffer_t rd;					// read index
	volatile buffer_t wr;					// write index
	char buf[SPI_RX_BUFFER_SIZE];
} xioSpiRX_t;

typedef struct xioSpiTX {
	buffer_t size;
	volatile buffer_t rd;
	volatile buffer_t wr;
	char buf[SPI_TX_BUFFER_SIZE];
} xioSpiTX_t;

/******************************************************************************
 * SPI FUNCTION PROTOTYPES AND ALIASES
 ******************************************************************************/

xioDev_t *xio_init_spi(uint8_t dev);
FILE *xio_open_spi(const uint8_t dev, const char *addr, const flags_t flags);
//int xio_gets_spi(xioDev_t *d, char *buf, const int size);
//int xio_getc_spi(FILE *stream);
//int xio_putc_spi(const char c, FILE *stream);

#endif // xio_spi_h
