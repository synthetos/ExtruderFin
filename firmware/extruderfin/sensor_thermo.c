/*
 * sensor.c - TinyG temperature controller - sensor functions
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
#include "controller.h"
#include "text_parser.h"
#include "hardware.h"
#include "sensor_thermo.h"
#include "util.h"

//#include "report.h"
//#include "xio.h"

static inline float _sensor_sample();

/**** Temperature Sensor and Functions ****/
/*
 * sensor_init()	 		- initialize temperature sensor
 * sensor_on()	 			- turn temperature sensor on
 * sensor_off()	 			- turn temperature sensor off
 * sensor_start_reading()	- start a temperature reading
 * sensor_get_temperature()	- return latest temperature reading or LESS _THAN_ZERO
 * sensor_get_state()		- return current sensor state
 * sensor_get_code()		- return latest sensor code
 * sensor_callback() 		- perform sensor sampling / reading
 */
void thermo_init()
{
	memset(&sensor, 0, sizeof(sensor_t));
	sensor.temperature = ABSOLUTE_ZERO;
	sensor.sample_variance_max = SENSOR_SAMPLE_VARIANCE_MAX;
	sensor.reading_variance_max = SENSOR_READING_VARIANCE_MAX;
	sensor.disconnect_temperature = SENSOR_DISCONNECTED_TEMPERATURE;
	sensor.no_power_temperature = SENSOR_NO_POWER_TEMPERATURE;
// note: there are no bits to set to outputs in this initialization
}

void sensor_on()
{
	sensor.state = SENSOR_NO_DATA;
}

void sensor_off()
{
	sensor.state = SENSOR_OFF;
}

void sensor_start_reading() 
{ 
	sensor.sample_idx = 0;
	sensor.code = SENSOR_TAKING_READING;
}

uint8_t sensor_get_state() { return (sensor.state);}
uint8_t sensor_get_code() { return (sensor.code);}

float sensor_get_temperature() 
{ 
	if (sensor.state == SENSOR_HAS_DATA) { 
		return (sensor.temperature);
	} else {
		return (LESS_THAN_ZERO);	// an impossible temperature value
	}
}

/*
 * sensor_callback() - perform tick-timer sensor functions
 *
 *	Sensor_callback() reads in an array of sensor readings then processes the 
 *	array for a clean reading. The function uses the standard deviation of the 
 *	sample set to clean up the reading or to reject the reading as being flawed.
 *
 *	It's set up to collect 9 samples at 10 ms intervals to serve a 100ms heater 
 *	loop. Each sampling interval must be requested explicitly by calling 
 *	sensor_start_sample(). It does not free-run.
 */
stat_t sensor_callback()
{
	// cases where you don't execute the callback:
	if (sensor.state == SENSOR_OFF)	return (STAT_NOOP);
	if (sensor.code != SENSOR_TAKING_READING) return (STAT_NOOP);
	if (sensor.next_sample > SysTickTimer_getValue()) return (STAT_NOOP);

	sensor.next_sample = SysTickTimer_getValue() + SENSOR_SAMPLE_MS;

	// get a sample and return if still in the reading period
	sensor.sample[sensor.sample_idx] = _sensor_sample();
	if ((++sensor.sample_idx) < SENSOR_SAMPLES) { 
		return(STAT_OK);
	}

	// process the array to clean up samples
	float mean;
	sensor.std_dev = std_dev(sensor.sample, SENSOR_SAMPLES, &mean);
	if (sensor.std_dev > sensor.reading_variance_max) {
		sensor.state = SENSOR_ERROR;
		sensor.code = SENSOR_ERROR_BAD_READINGS;
		return(STAT_OK);
	}

	// reject the outlier samples and re-compute the average
	sensor.samples = 0;
	float temperature = 0;
	for (uint8_t i=0; i<SENSOR_SAMPLES; i++) {
		if (fabs(sensor.sample[i] - mean) < (sensor.sample_variance_max * sensor.std_dev)) {
			temperature += sensor.sample[i];
			sensor.samples++;
		}
	}
	// reject the sample set if too may failed
	if (sensor.samples < SENSOR_SAMPLE_THRESHOLD) {	// too many samples rejected. reading failed. Nothing we can do about it
		sensor.state = SENSOR_ERROR;
		sensor.code = SENSOR_ERROR_BAD_READINGS;
		return(STAT_OK);
	}

	// compute the resulting average temp w/o the outliers
	sensor.temperature = temperature / sensor.samples;
	sensor.state = SENSOR_HAS_DATA;
	sensor.code = SENSOR_IDLE;			// we are done. Flip it back to idle

	// process the exception cases
	if (sensor.temperature > SENSOR_DISCONNECTED_TEMPERATURE) {
		sensor.state = SENSOR_ERROR;
		sensor.code = SENSOR_ERROR_DISCONNECTED;
	} else if (sensor.temperature < SENSOR_NO_POWER_TEMPERATURE) {
		sensor.state = SENSOR_ERROR;
		sensor.code = SENSOR_ERROR_NO_POWER;
	}
	return(STAT_OK);
}

/*
 * _sensor_sample() - take a sample and reject samples showing excessive variance
 *
 *	Returns temperature sample if within variance bounds
 *	Returns ABSOLUTE_ZERO if it cannot get a sample within variance
 *	Retries sampling if variance is exceeded - reject spurious readings
 *	To start a new sampling period set 'new_period' true
 *
 * Temperature calculation math
 *
 *	This setup is using B&K TP-29 K-type test probe (Mouser part #615-TP29, $9.50 ea) 
 *	coupled to an Analog Devices AD597 (available from Digikey)
 *
 *	This combination is very linear between 100 - 300 deg-C outputting 7.4 mV per degree
 *	The ADC uses a 5v reference (the 1st major source of error), and 10 bit conversion
 *
 *	The sample value returned by the ADC is computed by ADCvalue = (1024 / Vref)
 *	The temperature derived from this is:
 *
 *		y = mx + b
 *		temp = adc_value * slope + offset
 *
 *		slope = (adc2 - adc1) / (temp2 - temp1)
 *		slope = 0.686645508							// from measurements
 *
 *		b = temp - (adc_value * slope)
 *		b = -4.062500								// from measurements
 *
 *		temp = (adc_value * 1.456355556) - -120.7135972
 */
static inline float _sensor_sample(void)
{
	return (((float)adc_read() * SENSOR_SLOPE) + SENSOR_OFFSET);
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

static const char fmt_st[]  PROGMEM = "[s1st]  sensor state%17d\n";
static const char fmt_tmp[] PROGMEM = "[s1tmp] sensor temperature%14.2f\n";
static const char fmt_svm[] PROGMEM = "[s1svm] sensor sample variance%10.2f\n";
static const char fmt_rvm[] PROGMEM = "[s1rvm] sensor reading variance%9.2f\n";

void s1_print_st(cmdObj_t *cmd) { text_print_ui8(cmd, fmt_st);}
void s1_print_tmp(cmdObj_t *cmd) { text_print_flt(cmd, fmt_tmp);}
void s1_print_svm(cmdObj_t *cmd) { text_print_flt(cmd, fmt_svm);}
void s1_print_rvm(cmdObj_t *cmd) { text_print_flt(cmd, fmt_rvm);}

#endif //__TEXT_MODE 
