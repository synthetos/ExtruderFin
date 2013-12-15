/*
 * test.c - extruderfin canned tests
 * This file works with ATMEGA328's on Kinen fins
 * This file is part of the TinyG project
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


#include "extruderfin.h"	// #1
#include "config.h"			// #2
#include "test.h"
#include "xio.h"

// regression test files

#ifdef __CANNED_TESTS
//#include "tests/test_001_smoke.h" 			// basic functionality
#endif

/*
 * run_test() - system tests from FLASH invoked by $test=n command
 *
 * 	By convention the character array containing the test must have the same 
 *	name as the file name.
 */
uint8_t run_test(cmdObj_t *cmd)
{
/*
	switch ((uint8_t)cmd->value) {
		case 0: { return (STAT_OK);}
#ifdef __CANNED_TESTS
		case 1: { xio_open(XIO_DEV_PGM, PGMFILE(&test_smoke),PGM_FLAGS); break;}
		default: {
			fprintf_P(stderr,PSTR("Test #%d not found\n"),(uint8_t)cmd->value);
			return (STAT_ERROR);
		}
	}
	tg_set_primary_source(XIO_DEV_PGM);
*/
	return (STAT_OK);
}

/*
 * run_canned_startup() - run a string on startup
 *
 *	Pre-load the USB RX (input) buffer with some test strings that will be called 
 *	on startup. Be mindful of the char limit on the read buffer (RX_BUFFER_SIZE).
 *	It's best to create a test file for really complicated things.
 */
void run_canned_startup()	// uncomment __CANNED_STARTUP in extruderfin.h if you need this run
{
#ifdef __CANNED_STARTUP
	xio_queue_RX_string(XIO_DEV_USART, "{\"fv\":\"\"}\n");
#endif
}
