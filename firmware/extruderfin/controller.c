/*
 * controller.c - extruderfin controller and top level parser
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
#include "extruderfin.h"
#include "config.h"
#include "controller.h"
#include "json_parser.h"
#include "text_parser.h"
#include "sensor.h"
#include "heater.h"
#include "xio.h"

#include <util/delay.h>

/***********************************************************************************
 **** STRUCTURE ALLOCATIONS *********************************************************
 ***********************************************************************************/

controller_t cs;		// controller state structure

/***********************************************************************************
 **** STATICS AND LOCALS ***********************************************************
 ***********************************************************************************/

static void _controller_HSM(void);
static stat_t _command_dispatch(void);

//static stat_t _shutdown_idler(void);
//static stat_t _normal_idler(void);
//static stat_t _limit_switch_handler(void);
//static stat_t _system_assertions(void);
//static stat_t _sync_to_planner(void);
//static stat_t _sync_to_tx_buffer(void);

/***********************************************************************************
 **** CODE *************************************************************************
 ***********************************************************************************/
/*
 * controller_init()
 */
void controller_init(uint8_t std_in, uint8_t std_out, uint8_t std_err)
{
	cs.magic_start = MAGICNUM;
	cs.magic_end = MAGICNUM;

	// identification attributes	
	cs.fw_build = FIRMWARE_BUILD;
	cs.fw_version = FIRMWARE_VERSION;
	cs.hw_platform = HARDWARE_PLATFORM;				// NB: HW version is set from EEPROM
	cs.hw_version = HARDWARE_VERSION;

	// controller variables
	cs.linelen = 0;									// initialize index for read_line()
	cs.state = CONTROLLER_STARTUP;					// ready to run startup lines
	cs.hard_reset_requested = false;
	cs.bootloader_requested = false;

	// serial IO settings
	xio_set_stdin(std_in);
	xio_set_stdout(std_out);
	xio_set_stderr(std_err);
	cs.active_src = std_in;
	cs.default_src = std_in;
//	tg_set_primary_source(cs.default_src);
}

/*
static stat_t _spew_ASCII(void)
{
	printf("0123456789abcdefghijklmnopqrstuvwxyz\n");
	_delay_ms (100);
	return (STAT_EAGAIN);
}
*/
/* 
 * controller_run() - MAIN LOOP - top-level controller
 *
 * The order of the dispatched tasks is very important. Tasks are ordered by increasing 
 * dependency (blocking hierarchy). Tasks that are dependent on completion of lower-level 
 * tasks must be later in the list than the task(s) they are dependent upon. 
 *
 * Tasks must be written as continuations as they will be called repeatedly, and are 
 * called even if they are not currently active. 
 *
 * The DISPATCH macro calls the function and returns to the controller parent if not 
 * finished (STAT_EAGAIN), preventing later routines from running (they remain blocked). 
 * Any other condition - OK or ERR - drops through and runs the next routine in the list.
 *
 * A routine that had no action (i.e. is OFF or idle) should return STAT_NOOP.
 */

void controller_run()
{
	while (true) {
		_controller_HSM();
	}
}

#define	DISPATCH(func) if (func == STAT_EAGAIN) return;
static void _controller_HSM()
{
//	DISPATCH(tick_callback());			// regular interval timer clock handler (ticks)
//	DISPATCH(_spew_ASCII());			// read and execute next incoming command
//	DISPATCH(sensor_callback());
//	DISPATCH(heater_callback());
	DISPATCH(_command_dispatch());		// read and execute next incoming command
}

//static uint8_t _dispatch()
static stat_t _command_dispatch(void)
{
	stat_t status;

//	char c;
//	if ((c = xio_getc()) != _FDEV_ERR) {
//		xio_putc(c);
//	}
//	return (STAT_OK);

	// read input line or return if not a completed line
	// xio_gets() is a non-blocking workalike of fgets()
	while (true) {
		if ((status = xio_gets(cs.active_src, cs.in_buf, sizeof(cs.in_buf))) == STAT_OK) {
			cs.bufp = cs.in_buf;
			break;
		}
	// handle end-of-file from file devices
//		if (status == STAT_EOF) {						// EOF can come from file devices only
//			if (cfg.comm_mode == TEXT_MODE) {
//				fprintf_P(stderr, PSTR("End of command file\n"));
//			} else {
//				rpt_exception(STAT_EOF);				// not really an exception
//			}
//			tg_reset_source();							// reset to default source
//		}
//		return (status);								// Note: STAT_EAGAIN, errors, etc. will drop through
	}
	cs.linelen = strlen(cs.in_buf)+1;					// linelen only tracks primary input
//	strncpy(cs.saved_buf, cs.bufp, SAVED_BUFFER_LEN-1);	// save input buffer for reporting

	// dispatch the new text line
	switch (toupper(*cs.bufp)) {						// first char

//		case '!': { cm_request_feedhold(); break; }		// include for AVR diagnostics and ARM serial
//		case '%': { cm_request_queue_flush(); break; }
//		case '~': { cm_request_cycle_start(); break; }

		case NUL: { 									// blank line (just a CR)
			if (cfg.comm_mode != JSON_MODE) {
				text_response(STAT_OK);
			}
			break;
		}
		case '$': case '?': case 'H': { 				// Text mode input
			cfg.comm_mode = TEXT_MODE;
			text_response(text_parser(cs.bufp));
			break;
		}
		case '{': { 									// JSON input
			cfg.comm_mode = JSON_MODE;
			json_parser(cs.bufp);
			break;
		}
	}
	return (STAT_OK);
}
