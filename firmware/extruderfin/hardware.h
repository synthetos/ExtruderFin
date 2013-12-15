/*
 * hardware.h - system hardware device configuration values 
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
#ifndef HARDWARE_H_ONCE
#define HARDWARE_H_ONCE

/*************************
 * Global System Defines *
 *************************/

// Clock Crystal Config. Pick one:
#define __CLOCK_INTERNAL_8MHZ TRUE	// use internal oscillator
//#define __CLOCK_EXTERNAL_8MHZ	TRUE
//#define __CLOCK_EXTERNAL_16MHZ TRUE	// technically this is overclocking a 3.3v 328

#undef F_CPU							// set for delays
#ifdef __CLOCK_EXTERNAL_16MHZ
#define F_CPU 16000000UL				// should always precede <avr/delay.h>
#else
#define F_CPU 8000000UL					// should always precede <avr/delay.h>
#endif

/*** Power reduction register mappings ***/
// you shouldn't need to change these

#define PRADC_bm 			(1<<PRADC)
#define PRUSART0_bm			(1<<PRUSART0)
#define PRSPI_bm 			(1<<PRSPI)
#define PRTIM1_bm			(1<<PRTIM1)
#define PRTIM0_bm			(1<<PRTIM0)
#define PRTIM2_bm			(1<<PRTIM2)
#define PRTWI_bm			(1<<PRTWI)

/**** Lower-level device mappings and constants (for atmega328P) ****/

#define PWM_PORT			PORTD			// Pulse width modulation port
#define PWM_OUTB			(1<<PIND3)		// OC2B timer output bit
#define PWM_TIMER			TCNT2			// Pulse width modulation timer
#define PWM_NONINVERTED		0xC0			// OC2A non-inverted mode, OC2B non-inverted mode
#define PWM_INVERTED 		0xF0			// OC2A inverted mode, OC2B inverted mode
#define PWM_PRESCALE 		64				// corresponds to TCCR2B |= 0b00000100;
#define PWM_PRESCALE_SET 	4				// 2=8x, 3=32x, 4=64x, 5=128x, 6=256x
#define PWM_MIN_RES 		20				// minimum allowable resolution (20 = 5% duty cycle resolution)
#define PWM_MAX_RES 		255				// maximum supported resolution
#define PWM_F_CPU			F_CPU			// 8 Mhz, nominally (internal RC oscillator)
#define PWM_F_MAX			(F_CPU / PWM_PRESCALE / PWM_MIN_RES)
#define PWM_F_MIN			(F_CPU / PWM_PRESCALE / 256)
#define PWM_FREQUENCY 		1000			// set PWM operating frequency

#define PWM2_PORT			PORTD			// secondary PWM channel (on Timer 0)
#define PWM2_OUT2B			(1<<PIND5)		// OC0B timer output bit

// ADC defines (apply to all channels)

#define ADC0_CHANNEL 		0				// ADC channel 1 / single-ended in this application (write to ADMUX)
#define ADC1_CHANNEL 		1				// ADC channel 1 / single-ended in this application (write to ADMUX)

#define ADC_PORT			PORTC			// Analog to digital converter channels
#define ADC_VREF 			3.30 //was 5.00	// change this if the circuit changes. 3v would be about optimal
#define ADC_ENABLE			(1<<ADEN)		// write this to ADCSRA to enable the ADC
#define ADC_START_CONVERSION (1<<ADSC)		// write to ADCSRA to start conversion
#define ADC_PRESCALE 		6				// 6=64x which is ~125KHz at 8Mhz clock
#define ADC_PRECISION 		1024			// change this if you go to 8 bit precision
#define ADC_REFS			0b01000000		// AVcc external voltage reference (writteen to ADMUX)

// timer deines

#define TICK_TIMER			TCNT0			// Tickclock timer
#define TICK_MODE			0x02			// CTC mode 		(TCCR0A value)
#define TICK_PRESCALER		0x03			// 64x prescaler  (TCCR0B value)
#define TICK_COUNT			125				// gets 8 Mhz/64 to 1000 Hz.

#define LED_PORT			PORTD			// LED port
#define LED_PIN				(1<<PIND2)		// LED indicator

/******************************************************************************
 * STRUCTURES 
 ******************************************************************************/

typedef struct hwSingleton {				// hardware devices that are part of the chip
	uint32_t sys_ticks;						// counts up every 1ms tick 
//	uint8_t adc_channel;					// set during init
	float pwm_freq;							// save it for stopping and starting PWM
	uint16_t nvm_base_addr;					// NVM base address
	uint16_t nvm_profile_base;				// NVM base address of current profile
} hwSingleton_t;

hwSingleton_t hw;							// HW is ALWAYS a singleton. You can't just "make more"

/******************************************************************************
 * FUNCTION PROTOTYPES
 ******************************************************************************/

void hardware_init(void);					// master hardware init

void adc_init(uint8_t channel);
uint16_t adc_read(void);

void pwm_init(void);
void pwm_on(float freq, float duty);
void pwm_off(void);
uint8_t pwm_set_freq(float freq);
uint8_t pwm_set_duty(float duty);

void systick_init(void);
uint32_t SysTickTimer_getValue(void);

void led_init(void);
void led_on(void);
void led_off(void);
void led_toggle(void);

stat_t hw_set_hv(cmdObj_t *cmd);
stat_t hw_get_id(cmdObj_t *cmd);

#ifdef __TEXT_MODE

	void hw_print_fb(cmdObj_t *cmd);
	void hw_print_fv(cmdObj_t *cmd);
	void hw_print_hp(cmdObj_t *cmd);
	void hw_print_hv(cmdObj_t *cmd);
	void hw_print_id(cmdObj_t *cmd);

#else

	#define hw_print_fb tx_print_stub
	#define hw_print_fv tx_print_stub
	#define hw_print_hp tx_print_stub
	#define hw_print_hv tx_print_stub
	#define hw_print_id tx_print_stub

#endif // __TEXT_MODE

#endif	// HARDWARE_H_ONCE
