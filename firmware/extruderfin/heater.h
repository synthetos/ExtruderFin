/*
 * heater.h - TinyG temperature controller - heater functions
 * Part of TinyG project
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
#ifndef heater_h
#define heater_h

/******************************************************************************
 * PARAMETERS AND SETTINGS
 ******************************************************************************/

/**** Heater default parameters ***/

#define HEATER_SYSTICK_INTERVAL		100		// sample timer in ms
#define HEATER_TICK_SECONDS 		0.1		// 100 ms
#define HEATER_HYSTERESIS 			10		// number of successive readings before declaring heater at-temp or out of regulation
#define HEATER_AMBIENT_TEMPERATURE	40		// detect heater not heating if readings stay below this temp
#define HEATER_OVERHEAT_TEMPERATURE 300		// heater is above max temperature if over this temp. Should shut down
#define HEATER_AMBIENT_TIMEOUT 		90		// time to allow heater to heat above ambinet temperature (seconds)
#define HEATER_REGULATION_RANGE 	3		// +/- degrees to consider heater in regulation
#define HEATER_REGULATION_TIMEOUT 	300		// time to allow heater to come to temp (seconds)
#define HEATER_BAD_READING_MAX 		5		// maximum successive bad readings before shutting down

enum tcHeaterState {						// heater state machine
	HEATER_OFF = 0,							// heater turned OFF or never turned on - transitions to HEATING
	HEATER_SHUTDOWN,						// heater has been shut down - transitions to HEATING
	HEATER_HEATING,							// heating up from OFF or SHUTDOWN - transitions to REGULATED or SHUTDOWN
	HEATER_REGULATED						// heater is at setpoint and in regulation - transitions to OFF or SHUTDOWN
};

enum tcHeaterCode {
	HEATER_OK = 0,							// heater is OK - no errors reported
	HEATER_AMBIENT_TIMED_OUT,				// heater failed to get past ambient temperature
	HEATER_REGULATION_TIMED_OUT,			// heater heated but failed to achieve regulation before timeout
	HEATER_OVERHEATED,						// heater exceeded maximum temperature cutoff value
	HEATER_SENSOR_ERROR						// heater encountered a fatal sensor error
};

/**** PID default parameters ***/

#define PID_DT 				HEATER_TICK_SECONDS	// time constant for PID computation
#define PID_EPSILON 		0.1				// error term precision
#define PID_MAX_OUTPUT 		100				// saturation filter max PWM percent
#define PID_MIN_OUTPUT 		0				// saturation filter min PWM percent

#define PID_Kp 				5.00			// proportional gain term
#define PID_Ki 				0.1 			// integral gain term
#define PID_Kd 				0.5				// derivative gain term
#define PID_INITIAL_INTEGRAL 200			// initial integral value to speed things along

// some starting values from the example code
//#define PID_Kp 0.1						// proportional gain term
//#define PID_Ki 0.005						// integral gain term
//#define PID_Kd 0.01						// derivative gain term

enum tcPIDState {							// PID state machine
	PID_OFF = 0,							// PID is off
	PID_ON
};


/******************************************************************************
 * STRUCTURES 
 ******************************************************************************/
// I prefer these to be static in the .c file but the scope needs to be 
// global to allow the report.c functions to get at the variables


typedef struct HeaterStruct {
	uint32_t next_systick;
	uint8_t state;				// heater state
	uint8_t code;				// heater code (more information about heater state)
	uint8_t	toggle;
	int8_t hysteresis;			// number of successive readings in or out or regulation before changing state
	uint8_t bad_reading_max;	// sets number of successive bad readings before declaring an error
	uint8_t bad_reading_count;	// count of successive bad readings
	double temperature;			// current heater temperature
	double setpoint;			// set point for regulation
	double regulation_range;	// +/- range to consider heater in regulation
	double regulation_timer;	// time taken so far in a HEATING cycle
	double ambient_timeout;		// timeout beyond which regulation has failed (seconds)
	double regulation_timeout;	// timeout beyond which regulation has failed (seconds)
	double ambient_temperature;	// temperature below which it's ambient temperature (heater failed)
	double overheat_temperature;// overheat temperature (cutoff temperature)
} heater_t;

typedef struct PIDstruct {		// PID controller itself
	uint8_t state;				// PID state (actually very simple)
	uint8_t code;				// PID code (more information about PID state)
	double output;				// also used for anti-windup on integral term
	double output_max;			// saturation filter max
	double output_min;			// saturation filter min
	double error;				// current error term
	double prev_error;			// error term from previous pass
	double integral;			// integral term
	double derivative;			// derivative term
//	double dt;					// pid time constant
	double Kp;					// proportional gain
	double Ki;					// integral gain 
	double Kd;					// derivative gain
} PID_t;

// allocations
heater_t heater;				// allocate one heater...
PID_t pid;						// allocate one PID channel...

/******************************************************************************
 * FUNCTION PROTOTYPES
 ******************************************************************************/

void heater_init(void);
void heater_on(double setpoint);
void heater_off(uint8_t state, uint8_t code);
stat_t heater_callback(void);

void pid_init();
void pid_reset();
double pid_calculate(double setpoint,double temperature);

/******************************************************************************
 * DEFINE UNIT TESTS
 ******************************************************************************/

#ifdef __TF1_UNIT_TESTS
void tf1_unit_tests(void);
#define	TF1_UNIT_TESTS tf1_unit_tests();
#else
#define	TF1_UNIT_TESTS
#endif // __UNIT_TESTS

#endif
