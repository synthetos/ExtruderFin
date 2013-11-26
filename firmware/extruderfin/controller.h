/*
 * controller.h - extruderfin controller and main dispatch loop
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
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT7
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef CONTROLLER_H_ONCE
#define CONTROLLER_H_ONCE

#ifdef __cplusplus
extern "C"{
#endif


//#define INPUT_BUFFER_LEN 128
#define TEXT_BUFFER_LEN 128

typedef struct controllerSingleton {		// main kinen control struct
//	double version;					// kinen version number
//	double build;					// kinen build number
//	double null;					// dumping ground for items with no target
//	uint8_t default_src;			// default source device

	magic_t magic_start;				// magic number to test memory integrity
	uint8_t state;						// controller state
	float null;							// dumping ground for items with no target
	float fw_build;						// tinyg firmware build number
	float fw_version;					// tinyg firmware version number
	float hw_platform;					// tinyg hardware compatibility - platform type
	float hw_version;					// tinyg hardware compatibility - platform revision

	// communications state variables
	uint8_t active_src;					// active source device
	uint8_t primary_src;				// primary input source device
	uint8_t secondary_src;				// secondary input source device
	uint8_t default_src;				// default source device
	uint8_t network_mode;				// 0=master, 1=repeater, 2=slave
	uint16_t linelen;					// length of currently processing line
	uint16_t read_index;				// length of line being read

	uint8_t comm_mode;				// communications mode 1=JSON
	uint16_t nvm_base_addr;			// NVM base address
	uint16_t nvm_profile_base;		// NVM base address of current profile

//	uint8_t linelen;				// length of currently processing line
//	uint8_t led_state;				// 0=off, 1=on
//	int32_t led_counter;			// a convenience for flashing an LED
//	char in_buf[INPUT_BUFFER_LEN];	// input text buffer
//	char out_buf[OUTPUT_BUFFER_LEN];// output text buffer
	uint8_t hard_reset_requested;		// flag to perform a hard reset
	uint8_t bootloader_requested;		// flag to enter the bootloader

	// controller serial buffers
	char_t *bufp;						// pointer to primary or secondary in buffer
	char buf[TEXT_BUFFER_LEN];		// input/output text buffer
//	char_t in_buf[INPUT_BUFFER_LEN];	// primary input buffer
//	char_t out_buf[OUTPUT_BUFFER_LEN];	// output buffer
//	char_t saved_buf[SAVED_BUFFER_LEN];	// save the input buffer
	magic_t magic_end;
	
} controller_t;

extern controller_t cs;					// controller state structure

enum cmControllerState {				// manages startup lines
	CONTROLLER_INITIALIZING = 0,		// controller is initializing - not ready for use
	CONTROLLER_NOT_CONNECTED,			// controller has not yet detected connection to USB (or other comm channel)
	CONTROLLER_CONNECTED,				// controller has connected to USB (or other comm channel)
	CONTROLLER_STARTUP,					// controller is running startup messages and lines
	CONTROLLER_READY					// controller is active and ready for use
};

/**** function prototypes ****/

void controller_init(uint8_t std_in, uint8_t std_out, uint8_t std_err);
void controller_run(void);

#ifdef __cplusplus
}
#endif

#endif // End of include guard: CONTROLLER_H_ONCE
