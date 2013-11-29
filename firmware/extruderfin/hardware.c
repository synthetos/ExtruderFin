/*
 * hardware.c - general hardware support functions
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

#ifdef __AVR
#include <avr/wdt.h>			// used for software reset
#endif

#include "extruderfin.h"
#include "config.h"
#include "controller.h"
#include "hardware.h"
#include "text_parser.h"
#include "sensor.h"
#include "heater.h"

#ifdef __cplusplus
extern "C"{
#endif

/*
 * hardware_init() - lowest level hardware init
 */

void hardware_init() 
{
	PRR = 0xFF;					// turn off all peripherals. Each device needs to enble itself

	DDRB = 0x00;				// initialize all ports as inputs. Each device sets its own outputs
	DDRC = 0x00;
	DDRD = 0x00;
}

// Atmega328P data direction defines: 0=input pin, 1=output pin
// These defines therefore only specify output pins
/*
#define PORTB_DIR	(SPI_MISO)			// setup for on-board SPI to work
#define PORTC_DIR	(0)					// no output bits on C
#define PORTD_DIR	(LED_PIN | PWM_OUTB)// set LED and PWM bits as outputs
*/


/**** ADC - Analog to Digital Converter for thermocouple reader ****/
/*
 * adc_init() - initialize ADC. See tinyg_tc.h for settings used
 * adc_read() - returns a single ADC reading (raw). See __sensor_sample notes for more
 *
 *	There's a weird bug where sometimes the first conversion returns zero. 
 *	I need to fund out why this is happening and stop it.
 *	In the mean time there is a do-while loop in the read function.
 */
void adc_init(uint8_t channel)
{
	PRR &= ~PRADC_bm;					// Enable the ADC in the power reduction register (system.h)
	ADMUX  = (ADC_REFS | channel);		// setup ADC Vref and channel
	ADCSRA = (ADC_ENABLE | ADC_PRESCALE);// Enable ADC (bit 7) & set prescaler

	ADMUX &= 0xF0;						// clobber the channel
	ADMUX |= 0x0F & ADC_CHANNEL;		// set the channel
	DIDR0 = (1<<channel);				// disable digital input
}

uint16_t adc_read()
{
	do {
		ADCSRA |= ADC_START_CONVERSION; // start the conversion
		while (ADCSRA && (1<<ADIF) == 0);// wait about 100 uSec
		ADCSRA |= (1<<ADIF);			// clear the conversion flag
	} while (ADC == 0);
	return (ADC);
}

/**** PWM - Pulse Width Modulation Functions ****/
/*
 * pwm_init() - initialize RTC timers and data
 *
 * 	Configure timer 2 for extruder heater PWM
 *	Mode: 8 bit Fast PWM Fast w/OCR2A setting PWM freq (TOP value)
 *		  and OCR2B setting the duty cycle as a fraction of OCR2A seeting
 */
void pwm_init(void)
{
	DDRD |= PWM_OUTB;					// set PWM bit to output
	PRR &= ~PRTIM2_bm;					// Enable Timer2 in the power reduction register (system.h)
	TCCR2A  = PWM_INVERTED;				// alternative is PWM_NONINVERTED
	TCCR2A |= 0b00000011;				// Waveform generation set to MODE 7 - here...
	TCCR2B  = 0b00001000;				// ...continued here
	TCCR2B |= PWM_PRESCALE_SET;			// set clock and prescaler
	TIMSK1 = 0; 						// disable PWM interrupts
	OCR2A = 0;							// clear PWM frequency (TOP value)
	OCR2B = 0;							// clear PWM duty cycle as % of TOP value
	hw.pwm_freq = 0;
}

void pwm_on(double freq, double duty)
{
	pwm_init();
	pwm_set_freq(freq);
	pwm_set_duty(duty);
}

void pwm_off(void)
{
	pwm_on(0,0);
}

/*
 * pwm_set_freq() - set PWM channel frequency
 *
 *	At current settings the range is from about 500 Hz to about 6000 Hz  
 */
uint8_t pwm_set_freq(double freq)
{
	hw.pwm_freq = F_CPU / PWM_PRESCALE / freq;
	if (hw.pwm_freq < PWM_MIN_RES) { 
		OCR2A = PWM_MIN_RES;
	} else if (hw.pwm_freq >= PWM_MAX_RES) { 
		OCR2A = PWM_MAX_RES;
	} else { 
		OCR2A = (uint8_t)hw.pwm_freq;
	}
	return (STAT_OK);
}

/*
 * pwm_set_duty() - set PWM channel duty cycle 
 *
 *	Setting duty cycle between 0 and 100 enables PWM channel
 *	Setting duty cycle to 0 disables the PWM channel with output low
 *	Setting duty cycle to 100 disables the PWM channel with output high
 *
 *	The frequency must have been set previously.
 *
 *	Since I can't seem to get the output pin to work in non-inverted mode
 *	it's done in software in this routine.
 */
uint8_t pwm_set_duty(double duty)
{
	if (duty < 0.01) {				// anything approaching 0% 
		OCR2B = 255;
	} else if (duty > 99.9) { 		// anything approaching 100%
		OCR2B = 0;
	} else {
		OCR2B = (uint8_t)(OCR2A * (1-(duty/100)));
	}
	OCR2A = (uint8_t)hw.pwm_freq;
	return (STAT_OK);
}

/*
 * systick_init() 	  - initialize RIT timers and data
 * SysTickTimer_getValue() - this is a hack to get around some compatibility problems
 */
void systick_init(void)
{
	PRR &= ~PRTIM0_bm;				// Enable Timer0 in the power reduction register (hardware.h)
	TCCR0A = TICK_MODE;				// mode_settings
	TCCR0B = TICK_PRESCALER;		// 1024 ~= 7800 Hz
	OCR0A = TICK_COUNT;
	TIMSK0 = (1<<OCIE0A);			// enable compare interrupts
	hw.sys_ticks = 0;
}

ISR(TIMER0_COMPA_vect)
{
	hw.sys_ticks++;
}

#ifdef __AVR
uint32_t SysTickTimer_getValue()
{
	return (hw.sys_ticks);	// system ticks by 1 ms
}
#endif // __AVR

#ifdef __ARM
uint32_t SysTickTimer_getValue()
{
	return (SysTickTimer.getValue());
}
#endif // __ARM

#ifdef __cplusplus
}
#endif

/**** LED Functions ****
 * led_init()
 * led_on()
 * led_off()
 * led_toggle()
 */
void led_init()
{
	DDRD |= PWM_OUTB;			// set PWM bit to output
	led_off();					// put off the red light [~Sting, 1978]
}

void led_on(void) 
{
	LED_PORT &= ~(LED_PIN);
}

void led_off(void) 
{
	LED_PORT |= LED_PIN;
}

void led_toggle(void) 
{
	if (LED_PORT && LED_PIN) {
		led_on();
	} else {
		led_off();
	}
}


/***** END OF SYSTEM FUNCTIONS *****/


/***********************************************************************************
 * CONFIGURATION AND INTERFACE FUNCTIONS
 * Functions to get and set variables from the cfgArray table
 ***********************************************************************************/

/*
 * hw_get_id() - get device ID (signature)
 */
/*
stat_t hw_get_id(cmdObj_t *cmd) 
{
	char_t tmp[SYS_ID_LEN];
	_get_id(tmp);
	cmd->objtype = TYPE_STRING;
	ritorno(cmd_copy_string(cmd, tmp));
	return (STAT_OK);
}
*/
/*
 * hw_run_boot() - invoke boot form the cfgArray
 */
stat_t hw_run_boot(cmdObj_t *cmd)
{
//	hw_request_bootloader();
	return(STAT_OK);
}

/*
 * hw_set_hv() - set hardware version number
 */
stat_t hw_set_hv(cmdObj_t *cmd) 
{
	if (cmd->value > HARDWARE_VERSION_MAX) { return (STAT_INPUT_VALUE_UNSUPPORTED);}
	set_flt(cmd);					// record the hardware version
//	_port_bindings(cmd->value);		// reset port bindings
//	switch_init();					// re-initialize the GPIO ports
//+++++	gpio_init();				// re-initialize the GPIO ports
	return (STAT_OK);
}

/***********************************************************************************
 * TEXT MODE SUPPORT
 * Functions to print variables from the cfgArray table
 ***********************************************************************************/

#ifdef __TEXT_MODE

static const char fmt_fb[] PROGMEM = "[fb]  firmware build%18.2f\n";
static const char fmt_fv[] PROGMEM = "[fv]  firmware version%16.2f\n";
static const char fmt_hp[] PROGMEM = "[hp]  hardware platform%15.2f\n";
static const char fmt_hv[] PROGMEM = "[hv]  hardware version%16.2f\n";
//static const char fmt_id[] PROGMEM = "[id]  TinyG ID%30s\n";

void hw_print_fb(cmdObj_t *cmd) { text_print_flt(cmd, fmt_fb);}
void hw_print_fv(cmdObj_t *cmd) { text_print_flt(cmd, fmt_fv);}
void hw_print_hp(cmdObj_t *cmd) { text_print_flt(cmd, fmt_hp);}
void hw_print_hv(cmdObj_t *cmd) { text_print_flt(cmd, fmt_hv);}
//void hw_print_id(cmdObj_t *cmd) { text_print_str(cmd, fmt_id);}

#endif //__TEXT_MODE 
#ifdef __cplusplus
}
#endif
