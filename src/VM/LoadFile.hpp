/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef LOAD_FILE_HPP
#define LOAD_FILE_HPP

#include "VM.hpp"

int load_src( vm_state_t & vm, const std::string & file );

#endif // LOAD_FILE_HPP