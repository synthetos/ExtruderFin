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

#define FIRMWARE_BUILD 			012.02		// working on temperature reading, advanced firmware revision to 0.2
#define FIRMWARE_VERSION		0.2			// firmware major version
#define HARDWARE_PLATFORM		1			// board style 
#define HARDWARE_VERSION		1			// board revision number
#define HARDWARE_VERSION_MAX	HARDWARE_VERSION

/****** COMPILE-TIME SETTINGS ******/
// Comment out what you don't need to skinny down the FLASH / RAM footprint

//#define __SIMULATION

#ifndef __SIMULATION
  #define __TEXT_MODE						// comment out to disable text mode support (saves ~5Kb)
  #define __HELP_SCREENS					// comment out to disable help screens
  #define __PERSISTENCE						// comment out if persistence is not needed / required
#endif
//#define __CANNED_TESTS 					// tehre are no canned tests

#define __ENABLE_USART_DEVICE
#define __ENABLE_SPI_DEVICE
#define __ENABLE_PGM_FILE_DEVICE

/****** DEVELOPMENT SETTINGS ******/

#ifdef __SIMULATION
  #define __CANNED_STARTUP					// run any canned startup commands
  #define __SUPPRESS_STARTUP_MESSAGES 		// what it says
#endif
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

/*******************************
 * AVR ATMEGA328 Compatibility *
 *******************************/
#ifdef __AVR

#include <avr/pgmspace.h>		// defines PROGMEM and PSTR
#include <avr/interrupt.h>

typedef char char_t;			// ARM/C++ version uses uint8_t as char_t

																	// gets rely on cmd->index having been set
#define GET_TABLE_WORD(a)  pgm_read_word(&cfgArray[cmd->index].a)	// get word value from cfgArray
#define GET_TABLE_BYTE(a)  pgm_read_byte(&cfgArray[cmd->index].a)	// get byte value from cfgArray
#define GET_TABLE_FLOAT(a) pgm_read_float(&cfgArray[cmd->index].a)	// get float value from cfgArray
#define GET_TABLE_DWORD(a) pgm_read_dword(&cfgArray[cmd->index].a)	// get 32 bit binary value from cfgArray
#define GET_TOKEN_BYTE(i,a) (char_t)pgm_read_byte(&cfgArray[i].a)	// get token byte value from cfgArray

// populate the shared buffer with the token string given the index
#define GET_TOKEN_STRING(i,a) strcpy_P(a, (char *)&cfgArray[(index_t)i].token);
//#define GET_TOKEN_STRING(i) strcpy_P(shared_buf, (char *)&cfgArray[(index_t)i].token);

// get text from an array of strings in PGM and convert to RAM string
#define GET_TEXT_ITEM(b,a) strncpy_P(shared_buf,(const char *)pgm_read_word(&b[a]),SHARED_BUF_LEN) 

// get units from array of strings in PGM and convert to RAM string
#define GET_UNITS(a) strncpy_P(shared_buf,(const char *)pgm_read_word(&msg_units[cm_get_units_mode(a)]),SHARED_BUF_LEN)

// IO settings
#define STD_IN 	XIO_DEV_USART	// default IO settings
#define STD_OUT	XIO_DEV_USART
#define STD_ERR	XIO_DEV_USART

// String compatibility
#define strtof strtod			// strtof is not in the AVR lib

#endif // __AVR

/*********************
 * ARM Compatibility *
 *********************/
#ifdef __ARM

#endif // __ARM

/******************************************************************************
 ***** APPLICATION DEFINITIONS ************************************************
 ******************************************************************************/

typedef uint16_t magic_t;		// magic number size
#define MAGICNUM 0x12EF			// used for memory integrity assertions

/************************************************************************************ 
 * STATUS CODES
 *
 * The first code range (0-19) is aligned with the XIO codes and must be so.
 * Please don't change them without checking the corresponding values in xio.h
 *
 * Any changes to the ranges also require changing the message strings and 
 * string array in report.c
 *
 * ritorno is a handy way to provide exception returns 
 * It returns only if an error occurred. (ritorno is Italian for return) 
 */
typedef uint8_t stat_t;
extern stat_t status_code;				// allocated in main.c
extern char shared_buf[];				// allocated in main.c

char *get_status_message(stat_t status);

#define ritorno(a) if((status_code=a) != STAT_OK) { return(status_code); }

// OS, communications and low-level status (must align with XIO_xxxx codes in xio.h)
#define	STAT_OK 0						// function completed OK
#define	STAT_ERROR 1					// generic error return (EPERM)
#define	STAT_EAGAIN 2					// function would block here (call again)
#define	STAT_NOOP 3						// function had no-operation
#define	STAT_COMPLETE 4					// operation is complete
#define STAT_TERMINATE 5				// operation terminated (gracefully)
#define STAT_RESET 6					// operation was hard reset (sig kill)
#define	STAT_EOL 7						// function returned end-of-line
#define	STAT_EOF 8						// function returned end-of-file
#define	STAT_FILE_NOT_OPEN 9
#define	STAT_FILE_SIZE_EXCEEDED 10
#define	STAT_NO_SUCH_DEVICE 11
#define	STAT_BUFFER_EMPTY 12
#define	STAT_BUFFER_FULL 13
#define	STAT_BUFFER_FULL_FATAL 14
#define	STAT_INITIALIZING 15			// initializing - not ready for use
#define	STAT_ENTERING_BOOT_LOADER 16	// this code actually emitted from boot loader, not TinyG
#define	STAT_ERROR_17 17
#define	STAT_ERROR_18 18
#define	STAT_ERROR_19 19				// NOTE: XIO codes align to here

// Internal errors and startup messages
#define	STAT_INTERNAL_ERROR 20			// unrecoverable internal error
#define	STAT_INTERNAL_RANGE_ERROR 21	// number range other than by user input
#define	STAT_FLOATING_POINT_ERROR 22	// number conversion error
#define	STAT_DIVIDE_BY_ZERO 23
#define	STAT_INVALID_ADDRESS 24
#define	STAT_READ_ONLY_ADDRESS 25
#define	STAT_INIT_FAIL 26
#define	STAT_ALARMED 27
#define	STAT_ERROR_28 28
#define	STAT_ERROR_29 29
#define	STAT_ERROR_30 30
#define	STAT_ERROR_31 31
#define	STAT_ERROR_32 32
#define	STAT_ERROR_33 33
#define	STAT_ERROR_34 34
#define	STAT_ERROR_35 35
#define	STAT_ERROR_36 36
#define	STAT_ERROR_37 37
#define	STAT_ERROR_38 38
#define	STAT_ERROR_39 39

// Generic data errors (400's, if you will)
#define	STAT_UNRECOGNIZED_COMMAND 40		// parser didn't recognize the command
#define	STAT_EXPECTED_COMMAND_LETTER 41		// malformed line to parser
#define	STAT_BAD_NUMBER_FORMAT 42			// number format error
#define	STAT_INPUT_EXCEEDS_MAX_LENGTH 43	// input string is too long
#define	STAT_INPUT_VALUE_TOO_SMALL 44		// input error: value is under minimum
#define	STAT_INPUT_VALUE_TOO_LARGE 45		// input error: value is over maximum
#define	STAT_INPUT_VALUE_RANGE_ERROR 46		// input error: value is out-of-range
#define	STAT_INPUT_VALUE_UNSUPPORTED 47		// input error: value is not supported
#define	STAT_JSON_SYNTAX_ERROR 48			// JSON input string is not well formed
#define	STAT_JSON_TOO_MANY_PAIRS 49			// JSON input string has too many JSON pairs
#define	STAT_JSON_TOO_LONG 50				// JSON output exceeds buffer size
#define	STAT_NO_BUFFER_SPACE 51				// Buffer pool is full and cannot perform this operation
#define	STAT_CONFIG_NOT_TAKEN 52			// configuration value not taken while in machining cycle
#define	STAT_ERROR_53 53
#define	STAT_ERROR_54 54
#define	STAT_ERROR_55 55
#define	STAT_ERROR_56 56
#define	STAT_ERROR_57 57
#define	STAT_ERROR_58 58
#define	STAT_ERROR_59 59
/*
// Application specific errors
#define	STAT_MINIMUM_LENGTH_MOVE_ERROR 60	// move is less than minimum length
#define	STAT_MINIMUM_TIME_MOVE_ERROR 61		// move is less than minimum time
#define	STAT_GCODE_BLOCK_SKIPPED 62			// block is too short - was skipped
#define	STAT_GCODE_INPUT_ERROR 63			// general error for gcode input
#define	STAT_GCODE_FEEDRATE_ERROR 64		// move has no feedrate
#define	STAT_GCODE_AXIS_WORD_MISSING 65		// command requires at least one axis present
#define	STAT_MODAL_GROUP_VIOLATION 66		// gcode modal group error
#define	STAT_HOMING_CYCLE_FAILED 67			// homing cycle did not complete
#define	STAT_MAX_TRAVEL_EXCEEDED 68
#define	STAT_MAX_SPINDLE_SPEED_EXCEEDED 69
#define	STAT_ARC_SPECIFICATION_ERROR 70		// arc specification error
#define	STAT_SOFT_LIMIT_EXCEEDED 71			// soft limit error
#define	STAT_COMMAND_NOT_ACCEPTED 72		// command cannot be accepted at this time
#define	STAT_PROBING_CYCLE_FAILED 73		// probing cycle did not complete
#define	STAT_JOGGING_CYCLE_FAILED 74		// jogging cycle did not complete
#define	STAT_MACHINE_ALARMED 75				// machine is alarmed. Command not processed
#define	STAT_LIMIT_SWITCH_HIT 76			// a limit switch was hit causing sutdown
#define	STAT_ERROR_77 77
#define	STAT_ERROR_78 78
#define	STAT_ERROR_79 79
#define	STAT_ERROR_80 80
#define	STAT_ERROR_81 81
#define	STAT_ERROR_82 82
#define	STAT_ERROR_83 83
#define	STAT_ERROR_84 84
#define	STAT_ERROR_85 85
#define	STAT_ERROR_86 86
#define	STAT_ERROR_87 87
#define	STAT_ERROR_88 88
#define	STAT_ERROR_89 89
#define	STAT_ERROR_90 90
#define	STAT_ERROR_91 91
#define	STAT_ERROR_92 92
#define	STAT_ERROR_93 93
#define	STAT_ERROR_94 94
#define	STAT_ERROR_95 95
#define	STAT_ERROR_96 96
#define	STAT_ERROR_97 97
#define	STAT_ERROR_98 98
#define	STAT_ERROR_99 99

// Assertion failures and application specific other fatal errors 
#define	STAT_GENERIC_ASSERTION_FAILURE 100	// generic assertion failure - unclassified
#define STAT_GENERIC_EXCEPTION_REPORT 101	// used for test
#define	STAT_MEMORY_FAULT 102				// generic memory corruption detected by magic numbers
#define	STAT_STACK_OVERFLOW 103
#define	STAT_CONTROLLER_ASSERTION_FAILURE 104
#define	STAT_CANONICAL_MACHINE_ASSERTION_FAILURE 105
#define	STAT_PLANNER_ASSERTION_FAILURE 106
#define	STAT_STEPPER_ASSERTION_FAILURE 107
#define	STAT_XIO_ASSERTION_FAILURE 108
#define	STAT_PREP_LINE_MOVE_TIME_IS_INFINITE 109
#define	STAT_PREP_LINE_MOVE_TIME_IS_NAN 110
*/
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
