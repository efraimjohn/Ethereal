/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef PARSER_FUNCTIONS_HPP
#define PARSER_FUNCTIONS_HPP

#include "Stmts.hpp"

#define NEXT_VALID( tok )							\
do {										\
	if( ph.peak( 1 )->type == TOK_INVALID ) {				\
		PARSE_FAIL( "expected token '%s', but found <EOF>",		\
			    TokStrs[ tok ] );					\
		return nullptr;							\
	}									\
	ph.next();								\
	if( ph.peak()->type != tok ) {						\
		PARSE_FAIL( "expected token '%s', but found '%s'",		\
			    TokStrs[ tok ], TokStrs[ ph.peak()->type ] );	\
		return nullptr;							\
	}									\
} while( 0 )

#define NEXT_VALID2( tok1, tok2 )								\
do {												\
	if( ph.peak( 1 )->type == TOK_INVALID ) {						\
		PARSE_FAIL( "expected token '%s' or '%s', but found <EOF>",			\
			    TokStrs[ tok1 ], TokStrs[ tok2 ] );					\
		return nullptr;									\
	}											\
	ph.next();										\
	if( ph.peak()->type != tok1 && ph.peak()->type != tok2 ) {				\
		PARSE_FAIL( "expected token '%s' or '%s', but found '%s'",			\
			    TokStrs[ tok1 ], TokStrs[ tok2 ], TokStrs[ ph.peak()->type ] );	\
		return nullptr;									\
	}											\
} while( 0 )

stmt_enum_t * parse_enum( const src_t & src, parse_helper_t & ph );
stmt_ldmod_t * parse_ldmod( const src_t & src, parse_helper_t & ph );
stmt_import_t * parse_import( const src_t & src, parse_helper_t & ph );

#endif // PARSER_FUNCTIONS_HPP