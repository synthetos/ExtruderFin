/*
 * report.c - contains all reporting statements
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

#include <stdio.h>
//#include <stdlib.h>
//#include <stdbool.h>
//#include <string.h>
#include <avr/pgmspace.h>

#include "report.h"
#include "sensor.h"
#include "heater.h"
//#include "tempfin.h"
//#include "xio/xio.h"

/*** Strings and string arrays in program memory ***/
static const char initialized[] PROGMEM = "\nDevice Initialized\n"; 

static const char msg_scode0[] PROGMEM = "";
static const char msg_scode1[] PROGMEM = "  Taking Reading";
static const char msg_scode2[] PROGMEM = "  Bad Reading";
static const char msg_scode3[] PROGMEM = "  Disconnected";
static const char msg_scode4[] PROGMEM = "  No Power";
static PGM_P const msg_scode[] PROGMEM = { msg_scode0, msg_scode1, msg_scode2, msg_scode3, msg_scode4 };

static const char msg_hstate0[] PROGMEM = "  OK";
static const char msg_hstate1[] PROGMEM = "  Shutdown";
static const char msg_hstate2[] PROGMEM = "  Heating";
static const char msg_hstate3[] PROGMEM = "  REGULATED";
static PGM_P const msg_hstate[] PROGMEM = { msg_hstate0, msg_hstate1, msg_hstate2, msg_hstate3 };

/*** Display routines ***/
void rpt_initialized()
{
	printf_P(PSTR("\nDevice Initialized %1.0f\n"),42);
}

void rpt_readout()
{
	printf_P(PSTR("Temp:%1.3f  "), 		sensor.temperature);
	printf_P(PSTR("PWM:%1.3f  "),		pid.output);
//	printf_P(PSTR("s[0]:%1.3f  "), 		sensor.sample[0]);
	printf_P(PSTR("StdDev:%1.3f  "),	sensor.std_dev);
//	printf_P(PSTR("Samples:%1.3f  "),	sensor.samples);
	printf_P(PSTR("Err:%1.3f  "),		pid.error);
	printf_P(PSTR("I:%1.3f  "),			pid.integral);
//	printf_P(PSTR("D:%1.3f  "),			pid.derivative);
//	printf_P(PSTR("Hy:%1.3f  "),		heater.hysteresis);

	printf_P((PGM_P)pgm_read_word(&msg_hstate[heater.state]));
//	printf_P((PGM_P)pgm_read_word(&msg_scode[sensor.code]));
	printf_P(PSTR("\n")); 
}
