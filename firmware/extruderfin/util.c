/*
 * util.c - a random assortment of useful functions
 * This file works with any processor on Kinen fins (generic)
 * This file is part of the TinyG project
 *
 * Copyright (c) 2010 - 2013 Alden S. Hart, Jr.
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
/* util contains a dog's breakfast of supporting functions that are not specific to tinyg: 
 * including:
 *	  - math and min/max utilities and extensions 
 *	  - vector manipulation utilities
 */

#include "extruderfin.h"
#include "util.h"

#ifdef __cplusplus
extern "C"{
#endif


/**** Math and other general purpose functions ****/

/*
 * std_dev() - compute mean and standard deviation in a single pass
 *
 * 	From http://www.strchr.com/standard_deviation_in_one_pass
 *
 *	"A lot of people talk about standard deviation but what is a standard deviant?"
 */

double std_dev(double a[], uint8_t n, double *mean) 
{
	if(n == 0) { return (0);}
	double sum = 0;
	double sq_sum = 0;
	for(uint8_t i=0; i<n; ++i) {
		sum += a[i];
		sq_sum += square(a[i]);
	}
	*mean = sum / n;
	double variance = (sq_sum / n) - square(*mean);
	return sqrt(variance);
}

/* Slightly faster (*) multi-value min and max functions
 * 	min3() - return minimum of 3 numbers
 * 	min4() - return minimum of 4 numbers
 * 	max3() - return maximum of 3 numbers
 * 	max4() - return maximum of 4 numbers
 *
 * Implementation tip: Order the min and max values from most to least likely in the calling args
 *
 * (*) Macro min4 is about 20uSec, inline function version is closer to 10 uSec (Xmega 32 MHz)
 * 	#define min3(a,b,c) (min(min(a,b),c))
 *	#define min4(a,b,c,d) (min(min(a,b),min(c,d)))
 *	#define max3(a,b,c) (max(max(a,b),c))
 *	#define max4(a,b,c,d) (max(max(a,b),max(c,d)))
 */

float min3(float x1, float x2, float x3)
{
	float min = x1;
	if (x2 < min) { min = x2;} 
	if (x3 < min) { return (x3);} 
	return (min);
}

float min4(float x1, float x2, float x3, float x4)
{
	float min = x1;
	if (x2 < min) { min = x2;} 
	if (x3 < min) { min = x3;} 
	if (x4 < min) { return (x4);}
	return (min);
}

float max3(float x1, float x2, float x3)
{
	float max = x1;
	if (x2 > max) { max = x2;} 
	if (x3 > max) { return (x3);} 
	return (max);
}

float max4(float x1, float x2, float x3, float x4)
{
	float max = x1;
	if (x2 > max) { max = x2;} 
	if (x3 > max) { max = x3;} 
	if (x4 > max) { return (x4);}
	return (max);
}

/**** String utilities ****
 * strcpy_U() 	   - strcpy workalike to get around initial NUL for blank string - possibly wrong
 * isnumber() 	   - isdigit that also accepts plus, minus, and decimal point
 * escape_string() - add escapes to a string - currently for quotes only
 */

/*
uint8_t * strcpy_U( uint8_t * dst, const uint8_t * src )
{
	uint16_t index = 0;
	do {
		dst[index] = src[index];	
	} while (src[index++] != 0);
	return dst;
}
*/

uint8_t isnumber(char_t c)
{
	if (c == '.') { return (true); }
	if (c == '-') { return (true); }
	if (c == '+') { return (true); }
	return (isdigit(c));
}

char_t *escape_string(char_t *dst, char_t *src) 
{
	char_t c;
	char_t *start_dst = dst;

	while ((c = *(src++)) != 0) {	// NUL
		if (c == '"') { *(dst++) = '\\'; }
		*(dst++) = c;
	}
	return (start_dst);
}

/*
 * pstr2str() - return an AVR style progmem string as a RAM string. No effect on ARMs
 *
 *	This function deals with FLASH memory string confusion between the AVR serias and ARMs. 
 *	AVRs typically have xxxxx_P() functions which take strings from FLASH as args. 
 *	On ARMs there is no need for this as strings are handled identically in FLASH and RAM. 
 *
 *	This function copies a string from FLASH to a pre-allocated RAM buffer - see main.c for 
 *	allocation and max length. On the ARM it's a pass through that just returns the address 
 *	of the input string
 */
char_t *pstr2str(const char_t *pgm_string)
{
#ifdef __AVR
	strncpy_P(shared_buf, pgm_string, SHARED_BUF_LEN);
	return (shared_buf);
#endif
#ifdef __ARM
	return (pgm_string);
#endif
}

/* 
 * compute_checksum() - calculate the checksum for a string
 * 
 *	Stops calculation on null termination or length value if non-zero.
 *
 * 	This is based on the the Java hashCode function. 
 *	See http://en.wikipedia.org/wiki/Java_hashCode()
 */
#define HASHMASK 9999

uint16_t compute_checksum(char_t const *string, const uint16_t length) 
{
	uint32_t h = 0;
	uint16_t len = strlen(string);
	if (length != 0) len = min(len, length);
    for (uint16_t i=0; i<len; i++) {
		h = 31 * h + string[i];
    }
    return (h % HASHMASK);
}

