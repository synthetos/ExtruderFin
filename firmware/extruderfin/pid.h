/*
 * heater.h - PID temperature control for extruder fin
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
/* Special thanks to Adam Mayer and the Replicator project for heater guidance
 */
#ifndef PID_H_ONCE
#define PID_H_ONCE

/******************************************************************************
 * PARAMETERS AND SETTINGS
 ******************************************************************************/

/**** PID default parameters ***/

#define PID_DT 				((float)HEATER_SAMPLE_MS / (float)1000)	// PID time constant == heater sampling rate
#define PID_EPSILON 		0.1				// error term precision
#define PID_OUTPUT_MAX 		100				// saturation filter max PWM percent
#define PID_OUTPUT_MIN 		0				// saturation filter min PWM percent
#define PID_INTEGRAL_MAX	100
#define PID_INITIAL_INTEGRAL 0				// initial integral value - set non-zero to speed things along

#define PID_Kp 				2.00			// proportional gain term
#define PID_Ki 				0.001 			// integral gain term
#define PID_Kd 				0.4				// derivative gain term

// some starting values from the example code
//#define PID_Kp 				0.1				// proportional gain term
//#define PID_Ki 				0.005			// integral gain term
//#define PID_Kd 				0.01			// derivative gain term
//#define PID_INITIAL_INTEGRAL 1				// initial integral value to speed things along

// earlier attempt - wicked overshoot
//#define PID_Kp 				5.00			// proportional gain term
//#define PID_Ki 				0.1 			// integral gain term
//#define PID_Kd 				0.5				// derivative gain term

enum tcPIDState {							// PID state machine
	PID_OFF = 0,							// PID is off
	PID_ON
};

/******************************************************************************
 * STRUCTURES 
 ******************************************************************************/
// I prefer these to be static in the .c file but the scope needs to be 
// global to allow the report.c functions to get at the variables

typedef struct PIDstruct {		// PID controller itself
	uint8_t state;				// PID state (actually very simple)
	uint8_t code;				// PID code (more information about PID state)
	float output;				// also used for anti-windup on integral term
	float output_max;			// saturation filter max
	float output_min;			// saturation filter min
	float error;				// current error term
	float prev_error;			// error term from previous pass
	float integral;				// integral term
	float derivative;			// derivative term
//	float dt;					// pid time constant
	float Kp;					// proportional gain
	float Ki;					// integral gain 
	float Kd;					// derivative gain
} PID_t;

// allocations
extern PID_t pid;				// allocate one PID channel...

/******************************************************************************
 * FUNCTION PROTOTYPES
 ******************************************************************************/

void pid_init();
void pid_reset();
float pid_calculate(float setpoint,float temperature);

#ifdef __TEXT_MODE

	void p1_print_kp(cmdObj_t *cmd);
	void p1_print_ki(cmdObj_t *cmd);
	void p1_print_kd(cmdObj_t *cmd);
	void p1_print_smx(cmdObj_t *cmd);
	void p1_print_smn(cmdObj_t *cmd);

#else

	#define p1_print_kp tx_print_stub
	#define p1_print_ki tx_print_stub
	#define p1_print_kd tx_print_stub
	#define p1_print_smx tx_print_stub
	#define p1_print_smn tx_print_stub

#endif // __TEXT_MODE


/******************************************************************************
 * DEFINE UNIT TESTS
 ******************************************************************************/

#ifdef __PID_UNIT_TESTS
void pid_unit_tests(void);
#define	PID_UNIT_TESTS pid_unit_tests();
#else
#define	PID_UNIT_TESTS
#endif

#endif	// Include guard: PID_H_ONCE
