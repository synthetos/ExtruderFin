/*
 * extruderfin.h - TinyG extruder functions
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
#ifndef EXTRUDERFIN_H_ONCE
#define EXTRUDERFIN_H_ONCE

// common system includes
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//#include "MotatePins.h"

/****** REVISIONS ******/

#define FIRMWARE_BUILD 		008.03			// More initial codebase update work
#define FIRMWARE_VERSION	0.1				// firmware major version
#define HARDWARE_PLATFORM	1				// board style 
#define HARDWARE_VERSION	1				// board revision number

/****** COMPILE-TIME SETTINGS ******/

#define __TEXT_MODE							// comment out to disable text mode support (saves ~9Kb)
#define __HELP_SCREENS						// comment out to disable help screens 		(saves ~3.5Kb)
#define __CANNED_TESTS 						// comment out to remove $tests 			(saves ~12Kb)
#define __TEST_99 							// comment out to remove diagnostic test 99

/****** DEVELOPMENT SETTINGS ******/

//#define __CANNED_STARTUP					// run any canned startup commands
//#define __DISABLE_PERSISTENCE				// disable EEPROM writes for faster simulation
//#define __SUPPRESS_STARTUP_MESSAGES 		// what it says
//#define __UNIT_TESTS						// master enable for unit tests; USAGE: uncomment test in .h file

void canned_startup(void);

/************************************************************************************
 ***** PLATFORM COMPATIBILITY *******************************************************
 ************************************************************************************/
#undef __AVR
#define __AVR
#undef __AVR_328
#define __AVR_328
//#undef __ARM
//#define __ARM

/*********************
 * AVR Compatibility *
 *********************/
#ifdef __AVR

#include <avr/pgmspace.h>		// defines PROGMEM and PSTR
#include <avr/interrupt.h>

typedef char char_t;			// ARM/C++ version uses uint8_t as char_t

																	// gets rely on cmd->index having been set
#define GET_TABLE_WORD(a)  pgm_read_word(&cfgArray[cmd->index].a)	// get word value from cfgArray
#define GET_TABLE_BYTE(a)  pgm_read_byte(&cfgArray[cmd->index].a)	// get byte value from cfgArray
#define GET_TABLE_FLOAT(a) pgm_read_float(&cfgArray[cmd->index].a)	// get float value from cfgArray
#define GET_TOKEN_BYTE(i,a) (char_t)pgm_read_byte(&cfgArray[i].a)	// get token byte value from cfgArray

// populate the shared buffer with the token string given the index
#define GET_TOKEN_STRING(i,a) strcpy_P(a, (char *)&cfgArray[(index_t)i].token);
//#define GET_TOKEN_STRING(i) strcpy_P(shared_buf, (char *)&cfgArray[(index_t)i].token);

// get text from an array of strings in PGM and convert to RAM string
#define GET_TEXT_ITEM(b,a) strncpy_P(shared_buf,(const char *)pgm_read_word(&b[a]),SHARED_BUF_LEN) 

// get units from array of strings in PGM and convert to RAM string
#define GET_UNITS(a) 	   strncpy_P(shared_buf,(const char *)pgm_read_word(&msg_units[cm_get_units_mode(a)]),SHARED_BUF_LEN)

// IO settings
#define STD_IN 	XIO_DEV_USB		// default IO settings
#define STD_OUT	XIO_DEV_USB
#define STD_ERR	XIO_DEV_USB

// String compatibility
#define strtof strtod			// strtof is not in the AVR lib

#endif // __AVR

/******************************************************************************
 * DEFINE UNIT TESTS
 ******************************************************************************/

#ifdef __TF1_UNIT_TESTS
void tf1_unit_tests(void);
#define	TF1_UNIT_TESTS tf1_unit_tests();
#else
#define	TF1_UNIT_TESTS
#endif // __UNIT_TESTS

#endif // EXTRUDERFIN_H_ONCE
