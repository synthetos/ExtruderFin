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
#include "xio.h"

/***********************************************************************************
 **** STRUCTURE ALLOCATIONS *********************************************************
 ***********************************************************************************/

controller_t cs;		// controller state structure

/***********************************************************************************
 **** STATICS AND LOCALS ***********************************************************
 ***********************************************************************************/

static void _controller_HSM(void);
//static stat_t _shutdown_idler(void);
//static stat_t _normal_idler(void);
//static stat_t _limit_switch_handler(void);
//static stat_t _system_assertions(void);
//static stat_t _sync_to_planner(void);
//static stat_t _sync_to_tx_buffer(void);
static stat_t _command_dispatch(void);

// prep for export to other modules:
//stat_t hardware_hard_reset_handler(void);
//stat_t hardware_bootloader_handler(void);

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
	cs.fw_build = FIRMWARE_BUILD;
	cs.fw_version = FIRMWARE_VERSION;
	cs.hw_platform = HARDWARE_PLATFORM;				// NB: HW version is set from EEPROM

	cs.linelen = 0;									// initialize index for read_line()
	cs.state = CONTROLLER_STARTUP;					// ready to run startup lines
	cs.hard_reset_requested = false;
	cs.bootloader_requested = false;

	xio_set_stdin(std_in);
	xio_set_stdout(std_out);
	xio_set_stderr(std_err);
	cs.active_src = std_in;
	cs.default_src = std_in;
//	tg_set_primary_source(cs.default_src);
}

/*
 *	_controller()
 *	_dispatch()
 *
 *	The controller/dispatch loop is a set of pre-registered callbacks that (in effect)
 *	provide rudimentry multi-threading. Functions are organized from highest priority 
 *	to lowest priority. Each called function must return a status code (see kinen.h). 
 *	If SC_EAGAIN (02) is returned the loop restarts at the beginning of the list. 
 *	For any other status code exceution continues down the list.
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
	DISPATCH(_command_dispatch());		// read and execute next incoming command
}

//static uint8_t _dispatch()
static stat_t _command_dispatch(void)
{
	ritorno (xio_gets(cs.active_src, cs.buf, sizeof(cs.buf)));// read line or return if not completed
	json_parser(cs.buf);
	return (STAT_OK);

//	if ((status = xio_gets(cs.active_src, cs.buf, sizeof(cs.buf))) != SC_OK) {
//		if (status == SC_EOF) {					// EOF can come from file devices only
//			fprintf_P(stderr, PSTR("End of command file\n"));
//			tg_reset_source();					// reset to default source
//		}
//		// Note that TG_EAGAIN, TG_NOOP etc. will just flow through
//		return (status);
//	}
}
