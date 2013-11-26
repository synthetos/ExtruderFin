/*
 * main.c - Kinen temperature controller example
 *
 * Part of Kinen project
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

#include "extruderfin.h"			// #1 There are some dependencies
//#include "config.h"				// #2
#include "controller.h"
#include "hardware.h"
#include "heater.h"
#include "sensor.h"
#include "report.h"
#include "config.h"
#include "json_parser.h"
#include "util.h"
#include "xio.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

void _init() __attribute__ ((weak));
void _init() {;}

#ifdef __cplusplus
}
#endif // __cplusplus

static void _application_init(void);
static void _unit_tests(void);

static void _controller_run(void);
static uint8_t _dispatch(void);
//static void _unit_tests(void);

// Had to move the struct definitions and declarations to .h file for reporting purposes

/****************************************************************************
 **** CODE ******************************************************************
 ****************************************************************************/

void init( void )
{
	#ifdef __ARM
	SystemInit();

	// Disable watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;

	// Initialize C library
	__libc_init_array();
	#endif
}

int main(void)
{
	// system initialization
//	init();						// do this first

	// application initialization
	_application_init();
	_unit_tests();					// run any unit tests that are enabled
//	run_canned_startup();			// run any pre-loaded commands
//	canned_startup();
	
	// main loop
	for (;;) {
		controller_run( );			// single pass through the controller
	}
	return 0;
/*
	// application level inits
//	heater_init();				// setup the heater module and subordinate functions
//	sensor_init();
//	sei(); 						// enable interrupts
//	rpt_initialized();			// send initalization string

//	_unit_tests();				// run any unit tests that are enabled

	while (true) {				// main loop
		_controller();
	}
	return (false);				// never returns
*/
}

static void _application_init(void)
{
	// There are a lot of dependencies in the order of these inits.
	// Don't change the ordering unless you understand this.

	cli();

	// hardware and low-level drivers
	sys_init();					// do this first
//	hardware_init();				// system hardware setup 			- must be first

//	rtc_init();						// real time counter

//	xio_init();					// do this second
	xio_init();						// xmega io subsystem
//	switch_init();					// switches

	adc_init(ADC_CHANNEL);		// init system devices
	pwm_init();
	tick_init();
	led_init();

	// application level inits
	heater_init();				// setup the heater module and subordinate functions
	sensor_init();
	sei(); 						// enable interrupts
	rpt_initialized();			// send initalization string
/*
	// application sub-systems
	controller_init(STD_IN, STD_OUT, STD_ERR);// must be first app init; reqs xio_init()
	config_init();					// config records from eeprom 		- must be next app init
	network_init();					// reset std devices if required	- must follow config_init()
	planner_init();					// motion planning subsystem
	canonical_machine_init();		// canonical machine				- must follow config_init()

	// now bring up the interrupts and get started
	PMIC_SetVectorLocationToApplication();// as opposed to boot ROM
	PMIC_EnableHighLevel();			// all levels are used, so don't bother to abstract them
	PMIC_EnableMediumLevel();
	PMIC_EnableLowLevel();
	sei();							 // enable global interrupts
	rpt_print_system_ready_message();// (LAST) announce system is ready
*/
}



/******************************************************************************
 * STARTUP TESTS
 ******************************************************************************/

/*
 * canned_startup() - run a string on startup
 *
 *	Pre-load the USB RX (input) buffer with some test strings that will be called 
 *	on startup. Be mindful of the char limit on the read buffer (RX_BUFFER_SIZE).
 *	It's best to create a test file for really complicated things.
 */
void canned_startup()	// uncomment __CANNED_STARTUP in tempfin1.h if you need this run
{
#ifdef __CANNED_STARTUP
	xio_queue_RX_string(XIO_DEV_USART, "{\"fv\":\"\"}\n");
#endif
}

/******************************************************************************
 * TEMPERATURE FIN UNIT TESTS
 ******************************************************************************/

static void _unit_tests(void)
{
	XIO_UNIT_TESTS				// uncomment __XIO_UNIT_TESTs in xio.h to enable these unit tests
	TF1_UNIT_TESTS				// uncomment __TF1_UNIT_TESTs in tempfin1.h to enable unit tests
//	heater_on(160);				// turn heater on for testing
}


#ifdef __TF1_UNIT_TESTS

/*
#define SETPOINT 200

static void _pid_test(void);
static void _pwm_test(void);

void tf1_unit_tests()
{
	_pid_test();
	_pwm_test();
}

static void _pid_test()
{
	pid_init();
	pid_calculate(SETPOINT, 0);
	pid_calculate(SETPOINT, SETPOINT-150);
	pid_calculate(SETPOINT, SETPOINT-100);
	pid_calculate(SETPOINT, SETPOINT-66);
	pid_calculate(SETPOINT, SETPOINT-50);
	pid_calculate(SETPOINT, SETPOINT-25);
	pid_calculate(SETPOINT, SETPOINT-20);
	pid_calculate(SETPOINT, SETPOINT-15);
	pid_calculate(SETPOINT, SETPOINT-10);
	pid_calculate(SETPOINT, SETPOINT-5);
	pid_calculate(SETPOINT, SETPOINT-3);
	pid_calculate(SETPOINT, SETPOINT-2);
	pid_calculate(SETPOINT, SETPOINT-1);
	pid_calculate(SETPOINT, SETPOINT);
	pid_calculate(SETPOINT, SETPOINT+1);
	pid_calculate(SETPOINT, SETPOINT+5);
	pid_calculate(SETPOINT, SETPOINT+10);
	pid_calculate(SETPOINT, SETPOINT+20);
	pid_calculate(SETPOINT, SETPOINT+25);
	pid_calculate(SETPOINT, SETPOINT+50);
}

static void _pwm_test()
{
	pwm_set_freq(50000);
	pwm_set_freq(10000);
	pwm_set_freq(5000);
	pwm_set_freq(2500);
	pwm_set_freq(1000);
	pwm_set_freq(500);
	pwm_set_freq(250);
	pwm_set_freq(100);

	pwm_set_freq(1000);
	pwm_set_duty(1000);
	pwm_set_duty(100);
	pwm_set_duty(99);
	pwm_set_duty(75);
	pwm_set_duty(50);
	pwm_set_duty(20);
	pwm_set_duty(10);
	pwm_set_duty(5);
	pwm_set_duty(2);
	pwm_set_duty(1);
	pwm_set_duty(0.1);

	pwm_set_freq(5000);
	pwm_set_duty(1000);
	pwm_set_duty(100);
	pwm_set_duty(99);
	pwm_set_duty(75);
	pwm_set_duty(50);
	pwm_set_duty(20);
	pwm_set_duty(10);
	pwm_set_duty(5);
	pwm_set_duty(2);
	pwm_set_duty(1);
	pwm_set_duty(0.1);

// exception cases
}
*/

#endif // __UNIT_TESTS

