/*
 * pid.c - PID code for extruderfin
 * This file works with any processor on Kinen fins (generic)
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
#include "text_parser.h"
#include "heater.h"
#include "pid.h"
#include "util.h"

// memory allocations
PID_t pid;						// allocate one PID channel...

/**** PID Functions ****/
/*
 * pid_init() - initialize PID with default values
 * pid_reset() - reset PID values to cold start
 * pid_calc() - derived from: http://www.embeddedheaven.com/pid-control-algorithm-c-language.htm
 */
void pid_init() 
{
	memset(&pid, 0, sizeof(struct PIDstruct));
	pid.Kp = PID_Kp;
	pid.Ki = PID_Ki;
	pid.Kd = PID_Kd;
	pid.output_max = PID_OUTPUT_MAX;		// saturation filter max value
	pid.output_min = PID_OUTPUT_MIN;		// saturation filter min value
	pid.state = PID_ON;
}

void pid_reset()
{
	pid.output = 0;
	pid.integral = PID_INITIAL_INTEGRAL;
	pid.prev_error = 0;
}

float pid_calculate(float setpoint,float temperature)
{
	if (pid.state == PID_OFF) { return (pid.output_min);}

	pid.error = setpoint - temperature;		// current error term

	// perform integration only if error is GT epsilon, and with anti-windup
	if ((fabs(pid.error) > PID_EPSILON) && (pid.output < pid.output_max)) {
		pid.integral += (pid.error * PID_DT);
		pid.integral = min(pid.integral, PID_INTEGRAL_MAX);
	}
	// compute derivative and output
	pid.derivative = (pid.error - pid.prev_error) / PID_DT;
	pid.output = pid.Kp * pid.error + pid.Ki * pid.integral + pid.Kd * pid.derivative;

	// fix min amd max outputs (saturation filter)
	if(pid.output > pid.output_max) { pid.output = pid.output_max; } else
	if(pid.output < pid.output_min) { pid.output = pid.output_min; }
	pid.prev_error = pid.error;

	return pid.output;
}

/***** END OF SYSTEM FUNCTIONS *****/

/***********************************************************************************
 * CONFIGURATION AND INTERFACE FUNCTIONS
 * Functions to get and set variables from the cfgArray table
 ***********************************************************************************/

/***********************************************************************************
 * TEXT MODE SUPPORT
 * Functions to print variables from the cfgArray table
 ***********************************************************************************/

#ifdef __TEXT_MODE

static const char fmt_kp[]  PROGMEM = "[p1kp]  PID Kp%26.2f\n";
static const char fmt_ki[]  PROGMEM = "[p1ki]  PID Ki%26.2f\n";
static const char fmt_kd[]  PROGMEM = "[p1kd]  PID Kd%26.2f\n";
static const char fmt_smx[] PROGMEM = "[p1smx] output max%22.2f\n";
static const char fmt_smn[] PROGMEM = "[p1smn] output min%22.2f\n";

void p1_print_kp(cmdObj_t *cmd)  { text_print_flt(cmd, fmt_kp);}
void p1_print_ki(cmdObj_t *cmd)  { text_print_flt(cmd, fmt_ki);}
void p1_print_kd(cmdObj_t *cmd)  { text_print_flt(cmd, fmt_kd);}
void p1_print_smx(cmdObj_t *cmd) { text_print_flt(cmd, fmt_smx);}
void p1_print_smn(cmdObj_t *cmd) { text_print_flt(cmd, fmt_smn);}

#endif //__TEXT_MODE 
