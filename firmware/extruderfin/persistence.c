/*
 * persistence.c - application independent configuration persistence
 * This file works with ATMEGA328's on Kinen fins
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

#include "extruderfin.h"	// #1
#include "config.h"			// #2
#include "hardware.h"
#include "persistence.h"

#ifdef __cplusplus
extern "C"{
#endif

/***********************************************************************************
 **** STRUCTURE ALLOCATIONS ********************************************************
 ***********************************************************************************/

cmdStr_t cmdStr;
cmdObj_t cmd_list[CMD_LIST_LEN];	// JSON header element

/***********************************************************************************
 **** CODE *************************************************************************
 ***********************************************************************************/
/*
 * nvm_init()
 */
void nvm_init()
{
	hw.nvm_base_addr = NVM_BASE_ADDR;
	hw.nvm_profile_base = hw.nvm_base_addr;
}

/*
 * nvm_persist()- persist value to NVM. Takes special cases into account
 */
void nvm_persist(cmdObj_t *cmd)
{
#ifdef __PERSISTENCE	// cutout for faster simulation in test
	if (cmd_index_lt_groups(cmd->index) == false) return;
	if (GET_TABLE_BYTE(flags) & F_PERSIST) nvm_write_value(cmd);
#endif
}

/*
 * nvm_read_value()	 - return value (as float) by index
 * nvm_write_value() - write to NVM by index, but only if the value has changed
 *
 *	It's the responsibility of the caller to make sure the index does not exceed range
 */

stat_t nvm_read_value(cmdObj_t *cmd)
{
//	int8_t nvm_byte_array[NVM_VALUE_LEN];
//	uint16_t nvm_address = cfg.nvm_profile_base + (cmd->index * NVM_VALUE_LEN);
//	(void)EEPROM_ReadBytes(nvm_address, nvm_byte_array, NVM_VALUE_LEN);
//	memcpy(&cmd->value, &nvm_byte_array, NVM_VALUE_LEN);
	return (STAT_OK);
}

stat_t nvm_write_value(cmdObj_t *cmd)
{
//	float tmp = cmd->value;
//	ritorno(cmd_read_NVM_value(cmd));
//	if (cmd->value != tmp) {		// catches the isnan() case as well
//		cmd->value = tmp;
//		int8_t nvm_byte_array[NVM_VALUE_LEN];
//		memcpy(&nvm_byte_array, &tmp, NVM_VALUE_LEN);
//		uint16_t nvm_address = cfg.nvm_profile_base + (cmd->index * NVM_VALUE_LEN);
//		(void)EEPROM_WriteBytes(nvm_address, nvm_byte_array, NVM_VALUE_LEN);
//	}
	return (STAT_OK);
}

/****************************************************************************
 ***** Config Unit Tests ****************************************************
 ****************************************************************************/

#ifdef __UNIT_TESTS
#ifdef __UNIT_TEST_PERSISTENCE

#define NVMwr(i,v) { cmd.index=i; cmd.value=v; cmd_write_NVM_value(&cmd);}
#define NVMrd(i)   { cmd.index=i; cmd_read_NVM_value(&cmd); printf("%f\n",cmd.value);}

void nvm_unit_tests()
{

// NVM tests
/*	cmdObj_t cmd;
	NVMwr(0, 329.01)
	NVMwr(1, 111.01)
	NVMwr(2, 222.02)
	NVMwr(3, 333.03)
	NVMwr(4, 444.04)
	NVMwr(10, 10.10)
	NVMwr(100, 100.100)
	NVMwr(479, 479.479)

	NVMrd(0)
	NVMrd(1)
	NVMrd(2)
	NVMrd(3)
	NVMrd(4)
	NVMrd(10)
	NVMrd(100)
	NVMrd(479)
*/

}

#endif
#endif

#ifdef __cplusplus
}
#endif // __cplusplus

