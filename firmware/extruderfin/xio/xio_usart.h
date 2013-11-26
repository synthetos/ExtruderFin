/*
 * xio_usart.h - Common USART definitions 
 * Part of Kinen project
 *
 * Copyright (c) 2010 - 2013 Alden S. Hart Jr.
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

#ifndef xio_usart_h
#define xio_usart_h

/******************************************************************************
 * USART DEVICE CONFIGS AND STRUCTURES
 ******************************************************************************/

#define USART_BAUD_RATE		115200
#define USART_BAUD_DOUBLER	0									// 0=turns baud doubler off
#define USART_ENABLE_FLAGS	( 1<<RXCIE0 | 1<<TXEN0 | 1<<RXEN0)  // enable recv interrupt, TX and RX
#define USART_XIO_FLAGS 	(XIO_BLOCK |  XIO_ECHO | XIO_XOFF | XIO_LINEMODE )

// Buffer structs must be the same as xioBuf except that the buf array size is defined.
#define USART_RX_BUFFER_SIZE 32
#define USART_TX_BUFFER_SIZE 32

typedef struct xioUsartRX {
	buffer_t size;						// initialize to USART_RX_BUFFER_SIZE-1
	volatile buffer_t rd;				// read index
	volatile buffer_t wr;				// write index
	char buf[USART_RX_BUFFER_SIZE];
} xioUsartRX_t;

typedef struct xioUsartTX {
	buffer_t size;						// initialize to USART_RX_BUFFER_SIZE-1
	volatile buffer_t rd;				// read index
	volatile buffer_t wr;				// write index (written by ISR)
	char buf[USART_TX_BUFFER_SIZE];
} xioUsartTX_t;

/******************************************************************************
 * USART CLASS AND DEVICE FUNCTION PROTOTYPES AND ALIASES
 ******************************************************************************/

xioDev_t *xio_init_usart(uint8_t dev);
FILE *xio_open_usart(const uint8_t dev, const char *addr, const flags_t flags);
void xio_set_baud_usart(xioDev_t *d, const uint32_t baud);
int xio_getc_usart(FILE *stream);
int xio_putc_usart(const char c, FILE *stream);

//int xio_gets_usart(xioDev_t *d, char *buf, const int size);
//void xio_queue_RX_char_usart(const uint8_t dev, const char c);
//void xio_queue_RX_string_usart(const uint8_t dev, const char *buf);

#endif
