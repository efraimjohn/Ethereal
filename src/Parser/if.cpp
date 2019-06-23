/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Internal.hpp"
#include "../Ethereal.hpp"

stmt_if_t * parse_if( src_t & src, parse_helper_t * ph )
{
	int tok_ctr = ph->tok_ctr();

	stmt_expr_t * expr = nullptr;
	stmt_block_t * block = nullptr;
	std::vector< condition_t > conds;

	int err, start_brace;
begin_cond:
	ph->next();
	err = find_next_of( ph, start_brace, { TOK_LBRACE }, TOK_RBRACE );
	if( err < 0 ) {
		if( err == -1 ) {
			PARSE_FAIL( "could not find the left brace for if block" );
		} else if( err == -2 ) {
			PARSE_FAIL( "found end of statement (semicolon) before left braces for if block" );
		}
		goto fail;
	}

	expr = parse_expr( src, ph, start_brace );
	if( expr == nullptr ) goto fail;
	ph->set_tok_ctr( start_brace );

begin_block:
	block = parse_block( src, ph, GRAM_IF );
	if( block == nullptr ) goto fail;
	conds.push_back( { expr, block } );
	expr = nullptr;
	block = nullptr;
	if( ph->peak( 1 )->type == TOK_ELIF ) {
		ph->next();
		goto begin_cond;
	} else if( ph->peak( 1 )->type == TOK_ELSE ) {
		ph->next();
		NEXT_VALID_FAIL( TOK_LBRACE );
		goto begin_block;
	}

	return new stmt_if_t( conds, tok_ctr );

fail:
	for( auto & cond : conds ) {
		if( cond.cond ) delete cond.cond;
		delete cond.block;
	}
	if( expr ) delete expr;
	if( block ) delete block;
	return nullptr;
}