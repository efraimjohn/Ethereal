/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef PARSER_HPP
#define PARSER_HPP

#include "Lexer.hpp"
#include "Parser/Stmts.hpp"

std::vector< stmt_base_t * > * parse( src_t & src, const GrammarTypes parent = GRAM_INVALID );

#endif // PARSER_HPP