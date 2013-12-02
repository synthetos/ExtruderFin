/*
 * config_app.h - application-specific part of configuration data
 * This file works with any processor on Kinen fins (generic)
 * This file is part of the TinyG project
 *
 * Copyright (c) 2010 - 2013 Alden S. Hart Jr.
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
/* This file contains application specific data for the config system:
 *	-,application-specific functions and function prototypes 
 *	- application-specific message and print format strings
 *	- application-specific config array
 *	- any other application-specific data
 */
/*
 * --- Config objects and the config list ---
 *
 *	The config system provides a structured way to access and set configuration variables
 *	and to invoke commands and functions from the command line and from JSON input.
 *	It also provides a way to get to an arbitrary variable for reading or display.
 *
 *	Config operates as a collection of "objects" (OK, so they are not really objects) 
 *	that encapsulate each variable. The objects are collected into a list (a body) 
 *	which may also have header and footer objects. 
 *
 *	This way the internals don't care about how the variable is represented or 
 *	communicated externally as all operations occur on the cmdObj list. The list is 
 *	populated by the text_parser or the JSON_parser depending on the mode. Lists 
 *	are also used for responses and are read out (printed) by a text-mode or JSON 
 *	print functions.
 */
/* --- Config variables, tables and strings ---
 *
 *	Each configuration value is identified by a short mnemonic string (token). The token 
 *	is resolved to an index into the cfgArray which is an program memory (PROGMEM) array 
 *	of structures with the static assignments for each variable. The array is organized as:
 * 
 *	  - group string identifying what group the variable is part of (if any)
 *	  - token string - the token for that variable - pre-pended with the group (if any)
 *	  - operations flags - flag if the value should be initialized, persisted, etc.
 *	  - pointer to a formatted print string also in program memory (Used only for text mode)
 *	  - function pointer for formatted print() method or text-mode readouts
 *	  - function pointer for get() method - gets values from memory
 *	  - function pointer for set() method - sets values and runs functions
 *	  - target - memory location that the value is written to / read from
 *	  - default value - for cold initialization
 *
 *	Persistence is provided by an NVM array containing values in EEPROM as doubles; 
 *	indexed by cfgArray index
 *
 *	The following rules apply to mnemonic tokens
 *	 - are up to 5 alphnuneric characters and cannot contain whitespace or separators
 *	 - must be unique (non colliding).
 *
 *  "Groups" are collections of values that mimic REST composite resources. Groups include:
 *	 - a system group is identified by "sys" and contains a collection of otherwise unrelated values
 *
 *	"Uber-groups" are groups of groups that are only used for text-mode printing - e.g.
 *	 - group of all groups
 */
/* --- Making changes and adding new values
 *
 *	Adding a new value to config (or changing an existing one) involves touching the following places:
 *
 *	 - Add a formatting string to fmt_XXX strings. Not needed if there is no text-mode print function
 *	   of you are using one of the generic print format strings.
 * 
 *	 - Create a new record in cfgArray[]. Use existing ones for examples. You can usually use 
 *	   generic functions for get and set; or create a new one if you need a specialized function.
 *
 *	   The ordering of group displays is set by the order of items in cfgArray. None of the other 
 *	   orders matter but are generally kept sequenced for easier reading and code maintenance. Also,
 *	   Items earlier in the array will resolve token searches faster than ones later in the array.
 *
 *	   Note that matching will occur from the most specific to the least specific, meaning that
 *	   if tokens overlap the longer one should be earlier in the array: "gco" should precede "gc".
 */
/* --- Rules, guidelines and random stuff
 *
 *	It's the responsibility of the object creator to set the index in the cmdObj when a variable
 *	is "hydrated". Many downstream function expect a valid index int he cmdObj struct. Set the 
 *	index by calling cmd_get_index(). This also validates the token and group if no lookup exists.
 */

/***********************************************************************************
 **** INCLUDE FILES ****************************************************************
 ***********************************************************************************
 * Depending on what's in your functions you may require more include files here
 */

#include "extruderfin.h"	// #1
#include "config.h"			// #2
#include "controller.h"
#include "text_parser.h"
#include "json_parser.h"
#include "hardware.h"
#include "heater.h"
#include "sensor.h"

#ifdef __cplusplus
extern "C"{
#endif

static stat_t _do_all(cmdObj_t *cmd);

/*** structures ***/

//cfgParameters_t cfg; 				// application specific configuration parameters

/***********************************************************************************
 **** CONFIG TABLE  ****************************************************************
 ***********************************************************************************
 *
 *	NOTES AND CAVEATS
 *
 *	- Token matching occurs from the most specific to the least specific. This means
 *	  that if shorter tokens overlap longer ones the longer one must precede the
 *	  shorter one. E.g. "gco" needs to come before "gc"
 *
 *	- Mark group strings for entries that have no group as nul -->  "". 
 *	  This is important for group expansion.
 *
 *	- Groups do not have groups. Neither do uber-groups, e.g.
 *	  'x' is --> { "", "x",  	and 'm' is --> { "", "m",  
 *
 *	- Be careful not to define groups longer than CMD_GROUP_LEN (3) and tokens longer
 *	  than CMD_TOKEN_LEN (5). (See config.h for lengths). The combined group + token 
 *	  cannot exceed CMD_TOKEN_LEN. String functions working on the table assume these 
 *	  rules are followed and do not check lengths or perform other validation.
 *
 *	NOTE: If the count of lines in cfgArray exceeds 255 you need to change index_t
 *	uint16_t in the config.h file. 
 */

const cfgItem_t cfgArray[] PROGMEM = {
	// group token flags p, print_func,	 get_func,  set_func, target for get/set,   	default value
//	{ "sys", "fb", _f07, 2, hw_print_fb, get_flt,   set_nul,  (uint32_t *)&cs.fw_build,		FIRMWARE_BUILD }, // MUST BE FIRST!
	{ "sys", "fb", _f07, 2, hw_print_fb, get_flt,   set_nul,  &cs.fw_build,					FIRMWARE_BUILD }, // MUST BE FIRST!
	{ "sys", "fv", _f07, 3, hw_print_fv, get_flt,   set_nul,  &cs.fw_version,				FIRMWARE_VERSION },
	{ "sys", "hp", _f07, 0, hw_print_hp, get_flt,   set_flt,  &cs.hw_platform,				HARDWARE_PLATFORM },
	{ "sys", "hv", _f07, 0, hw_print_hv, get_flt,   hw_set_hv,&cs.hw_version,				HARDWARE_VERSION },
//	{ "sys", "id", _fns, 0, hw_print_id, hw_get_id, set_nul,  &cs.null, 0 },  // device ID (ASCII signature)

	// Heater object
	{ "h1", "h1st",  _f00, 0, h1_print_st,  get_ui8, set_ui8, &heater.state,				HEATER_OFF },
	{ "h1", "h1tmp", _f00, 0, h1_print_tmp, get_flt, set_flt, &heater.temperature,			LESS_THAN_ZERO },
	{ "h1", "h1set", _f00, 0, h1_print_set, get_flt, set_flt, &heater.setpoint,				HEATER_HYSTERESIS },
	{ "h1", "h1hys", _f00, 0, h1_print_hys, get_ui8, set_ui8, &heater.hysteresis,			HEATER_HYSTERESIS },
	{ "h1", "h1amb", _f00, 0, h1_print_amb, get_flt, set_flt, &heater.ambient_temperature,	HEATER_AMBIENT_TEMPERATURE },
	{ "h1", "h1ovr", _f00, 0, h1_print_ovr, get_flt, set_flt, &heater.overheat_temperature, HEATER_OVERHEAT_TEMPERATURE },
	{ "h1", "h1ato", _f00, 0, h1_print_ato, get_flt, set_flt, &heater.ambient_timeout,		HEATER_AMBIENT_TIMEOUT },
	{ "h1", "h1reg", _f00, 0, h1_print_reg, get_flt, set_flt, &heater.regulation_range,		HEATER_REGULATION_RANGE },
	{ "h1", "h1rto", _f00, 0, h1_print_rto, get_flt, set_flt, &heater.regulation_timeout,	HEATER_REGULATION_TIMEOUT },
	{ "h1", "h1bad", _f00, 0, h1_print_bad, get_ui8, set_ui8, &heater.bad_reading_max,		HEATER_BAD_READING_MAX },

	// Sensor object
	{ "s1", "s1st",  _f00, 0, s1_print_st,  get_ui8, set_ui8, &sensor.state,				SENSOR_OFF },
	{ "s1", "s1tmp", _f00, 0, s1_print_tmp, get_flt, set_flt, &sensor.temperature,			LESS_THAN_ZERO },
	{ "s1", "s1svm", _f00, 0, s1_print_svm, get_flt, set_flt, &sensor.sample_variance_max,	SENSOR_SAMPLE_VARIANCE_MAX },
	{ "s1", "s1rvm", _f00, 0, s1_print_rvm, get_flt, set_flt, &sensor.reading_variance_max, SENSOR_READING_VARIANCE_MAX },

	// PID object
//	{ "p1", "p1st",  _f00, 0, p1_print_nul, get_ui8, set_ui8, &pid.state, 0 },
	{ "p1", "p1kp",	 _f00, 0, p1_print_kp,  get_flt, set_flt, &pid.Kp, PID_Kp },
	{ "p1", "p1ki",	 _f00, 0, p1_print_ki,  get_flt, set_flt, &pid.Ki, PID_Ki },
	{ "p1", "p1kd",	 _f00, 0, p1_print_kd,  get_flt, set_flt, &pid.Kd, PID_Kd },
	{ "p1", "p1smx", _f00, 0, p1_print_smx, get_flt, set_flt, &pid.output_max, PID_MAX_OUTPUT },
	{ "p1", "p1smn", _f00, 0, p1_print_smn, get_flt, set_flt, &pid.output_min, PID_MIN_OUTPUT },

	// Group lookups - must follow the single-valued entries for proper sub-string matching
	// *** Must agree with CMD_COUNT_GROUPS below ****
	{ "","sys",_f00, 0, tx_print_nul, get_grp, set_grp, &cs.null,0 },	// system group
	{ "","h1", _f00, 0, tx_print_nul, get_grp, set_grp, &cs.null,0 },	// heater group
	{ "","s1", _f00, 0, tx_print_nul, get_grp, set_grp, &cs.null,0 },	// sensor group
	{ "","p1", _f00, 0, tx_print_nul, get_grp, set_grp, &cs.null,0 },	// PID group
																		//  ^  watch the final (missing) comma!
	// Uber-group (groups of groups, for text-mode displays only)
	// *** Must agree with CMD_COUNT_UBER_GROUPS below ****
	{ "", "$", _f00, 0, tx_print_nul, _do_all, set_nul, &cs.null,0 }
};

/***** Make sure these defines line up with any changes in the above table *****/

#define CMD_COUNT_GROUPS 		4		// count of simple groups
#define CMD_COUNT_UBER_GROUPS 	1 		// count of uber-groups

/* <DO NOT MESS WITH THESE DEFINES> */
#define CMD_INDEX_MAX (sizeof cfgArray / sizeof(cfgItem_t))
#define CMD_INDEX_END_SINGLES		(CMD_INDEX_MAX - CMD_COUNT_UBER_GROUPS - CMD_COUNT_GROUPS)
#define CMD_INDEX_START_GROUPS		(CMD_INDEX_MAX - CMD_COUNT_UBER_GROUPS - CMD_COUNT_GROUPS)
#define CMD_INDEX_START_UBER_GROUPS (CMD_INDEX_MAX - CMD_COUNT_UBER_GROUPS)
/* </DO NOT MESS WITH THESE DEFINES> */

index_t	cmd_index_max() { return ( CMD_INDEX_MAX );}
uint8_t cmd_index_is_single(index_t index) { return ((index <= CMD_INDEX_END_SINGLES) ? true : false);}
uint8_t cmd_index_is_group(index_t index) { return (((index >= CMD_INDEX_START_GROUPS) && (index < CMD_INDEX_START_UBER_GROUPS)) ? true : false);}
uint8_t cmd_index_lt_groups(index_t index) { return ((index <= CMD_INDEX_START_GROUPS) ? true : false);}

/*
 * Uber Groups +++++ Why does this print out multiple times?
 */

static stat_t _do_all(cmdObj_t *cmd)	// print all parameters
{
	strcpy_P(cmd->token, PSTR("sys"));	// print system group
	get_grp(cmd);
	cmd_print_list(STAT_OK, TEXT_MULTILINE_FORMATTED, JSON_RESPONSE_FORMAT);

	strcpy_P(cmd->token, PSTR("h1"));
	get_grp(cmd);
	cmd_print_list(STAT_OK, TEXT_MULTILINE_FORMATTED, JSON_RESPONSE_FORMAT);

	strcpy_P(cmd->token,PSTR("p1"));
	get_grp(cmd);
	cmd_print_list(STAT_OK, TEXT_MULTILINE_FORMATTED, JSON_RESPONSE_FORMAT);

	strcpy_P(cmd->token,PSTR("s1"));
	get_grp(cmd);
	cmd_print_list(STAT_OK, TEXT_MULTILINE_FORMATTED, JSON_RESPONSE_FORMAT);

	return (STAT_OK);
}

#ifdef __cplusplus
}
#endif
