/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Internal.hpp"

stmt_enum_t * parse_enum( const src_t & src, parse_helper_t * ph )
{
	int tok_ctr = ph->tok_ctr();

	const tok_t * name = nullptr;

	NEXT_VALID( TOK_IDEN );
	name = ph->peak();

	std::vector< tok_t * > vals;

	NEXT_VALID( TOK_LBRACE );
	NEXT_VALID( TOK_IDEN );
val_begin:
	vals.push_back( ph->peak() );

	if( ph->peak( 1 )->type == TOK_COMMA ) {
		ph->next();
		if( ph->peak( 1 )->type == TOK_IDEN ) {
			ph->next();
			goto val_begin;
		}
	}

	NEXT_VALID( TOK_RBRACE );
	// now at RBRACE
	return new stmt_enum_t( name, vals, tok_ctr );
}