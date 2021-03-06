/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "VM.hpp"
#include "ExecInternal.hpp"

int vm_exec( vm_state_t & vm )
{
	int res = exec_internal( vm );
	if( res == E_OK ) res = vm.exit_status;
	return res;
}