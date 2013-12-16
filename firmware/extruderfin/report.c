/*
 * report.c - contains all reporting statements
 * This file works with any processor on Kinen fins (generic)
 * This file is part of the TinyG project
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

#include "extruderfin.h"
#include "config.h"
#include "report.h"
#include "json_parser.h"
#include "text_parser.h"
#include "sensor_thermo.h"
#include "heater.h"
#include "pid.h"
#include "util.h"

/*** Strings and string arrays in program memory ***/
static const char initialized[] PROGMEM = "\nDevice Initialized\n"; 

static const char msg_scode0[] PROGMEM = "";
static const char msg_scode1[] PROGMEM = "  Taking Reading";
static const char msg_scode2[] PROGMEM = "  Bad Reading";
static const char msg_scode3[] PROGMEM = "  Disconnected";
static const char msg_scode4[] PROGMEM = "  No Power";
static const char const *msg_scode[] PROGMEM = { msg_scode0, msg_scode1, msg_scode2, msg_scode3, msg_scode4 };

static const char msg_hstate0[] PROGMEM = "  OK";
static const char msg_hstate1[] PROGMEM = "  Shutdown";
static const char msg_hstate2[] PROGMEM = "  Heating";
static const char msg_hstate3[] PROGMEM = "  REGULATED";
static const char msg_hstate4[] PROGMEM = "  Cooling";
static const char const *msg_hstate[] PROGMEM = { msg_hstate0, msg_hstate1, msg_hstate2, msg_hstate3, msg_hstate4};

/**** Exception Messages ************************************************************
 * rpt_exception() - generate an exception message - always in JSON format
 * rpt_er()		   - send a bogus exception report for testing purposes (it's not real)
 *
 * WARNING: Do not call this function from MED or HI interrupts (LO is OK) or there is 
 *			a potential for deadlock in the TX buffer.
 */
void rpt_exception(uint8_t status)
{
	printf_P(PSTR("{\"er\":{\"fb\":%0.2f,\"st\":%d,\"msg\":\"%s\"}}\n"),
		FIRMWARE_BUILD, status, get_status_message(status));
}

/**** Application Messages *********************************************************
 * rpt_print_initializing_message()	   - initializing configs from hard-coded profile
 * rpt_print_loading_configs_message() - loading configs from EEPROM
 * rpt_print_system_ready_message()    - system ready message
 *
 *	These messages are always in JSON format to allow UIs to sync
 */

void _startup_helper(stat_t status, const char_t *msg )
{
#ifndef __SUPPRESS_STARTUP_MESSAGES
//	js.json_footer_depth = 0;
	cmd_reset_list();
	cmd_add_object((const char_t *)"fv");		// firmware version
	cmd_add_object((const char_t *)"fb");		// firmware build
	cmd_add_object((const char_t *)"hp");		// hardware platform
	cmd_add_object((const char_t *)"hv");		// hardware version
//	cmd_add_object((const char_t *)"id");		// hardware ID
	cmd_add_string((const char_t *)"msg", pstr2str(msg));	// startup message
	json_print_response(status);
#endif
}

void rpt_print_initializing_message(void)
{
//	_startup_helper(STAT_INITIALIZING, PSTR(INIT_MESSAGE));
}

void rpt_print_loading_configs_message(void)
{
//	_startup_helper(STAT_INITIALIZING, PSTR("Loading configs from EEPROM"));
}

void rpt_print_system_ready_message(void)
{
#ifndef __SUPPRESS_STARTUP_MESSAGES
	printf_P(PSTR("Device Initialized %1.0f\n"),(double)42);
	_startup_helper(STAT_OK, PSTR("SYSTEM READY"));
//	if (cfg.comm_mode == TEXT_MODE) { text_response(STAT_OK, (char_t *)"");}// prompt
#endif
}

void rpt_print_status()
{
	printf_P(PSTR("Temp:%1.3f  "), 		sensor.temperature);
	printf_P(PSTR("PWM:%1.3f  "),		pid.output);
//	printf_P(PSTR("s[0]:%1.3f  "), 		sensor.sample[0]);
	printf_P(PSTR("StdDev:%1.3f  "),	sensor.std_dev);
//	printf_P(PSTR("Samples:%1.3f  "),	sensor.samples);
	printf_P(PSTR("Err:%1.3f  "),		pid.error);
	printf_P(PSTR("I:%1.3f  "),			pid.integral);
	printf_P(PSTR("D:%1.3f  "),			pid.derivative);
//	printf_P(PSTR("Hy:%1.3f  "),		heater.hysteresis);

	printf_P((PGM_P)pgm_read_word(&msg_hstate[heater.state]));
//	printf_P((PGM_P)pgm_read_word(&msg_scode[sensor.code]));
	printf_P(PSTR("\n")); 
}
