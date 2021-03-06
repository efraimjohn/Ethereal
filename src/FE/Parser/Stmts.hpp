/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef PARSER_COMMON_HPP
#define PARSER_COMMON_HPP

#include "IO.hpp"
#include "ParseHelper.hpp"

#include "../../VM/Instructions.hpp"
#include "../../VM/MemPool.hpp"

enum GrammarTypes
{
	GRAM_INVALID = -1,
	GRAM_SIMPLE,
	GRAM_EXPR,
	GRAM_ENUM,
	GRAM_LDMOD,
	GRAM_IMPORT,
	GRAM_STRUCT,
	GRAM_BLOCK,
	GRAM_FUNC,
	GRAM_FN_STRUCT_SUBSCR_CALL,
	GRAM_IF,
	GRAM_FOR,
	GRAM_FOREACH,
	GRAM_RETURN,
	GRAM_CONTINUE,
	GRAM_BREAK,
	GRAM_COLLECTION,

	_GRAM_LAST,
};

extern const char * GrammarTypeStrs[ _GRAM_LAST ];

class stmt_base_t
{
public:
	const GrammarTypes m_type;
	const int m_tok_ctr;
	stmt_base_t( const GrammarTypes type, const int tok_ctr );
	virtual ~stmt_base_t();
	virtual void disp( const bool has_next ) const = 0;
	virtual bool bytecode( src_t & src ) const = 0;

	static void * operator new( size_t sz );
	static void operator delete( void * ptr, size_t sz );
};

enum SimpleType
{
	SIMPLE_TOKEN,
	SIMPLE_OPER,
	SIMPLE_KEYWORD,

	_SIMPLE_LAST,
};

extern const char * SimpleTypeStrs[ _SIMPLE_LAST ];

class stmt_simple_t : public stmt_base_t
{
public:
	const SimpleType m_stype;
	const tok_t * m_val;
	bool m_post_dot;
	stmt_simple_t( const SimpleType stype, const tok_t * val,
		       const int tok_ctr );
	~stmt_simple_t();
	void disp( const bool has_next ) const;
	bool bytecode( src_t & src ) const;
};

class stmt_enum_t : public stmt_base_t
{
	const tok_t * m_name;
	const std::vector< tok_t * > m_vals;
	bool m_is_mask;
public:
	stmt_enum_t( const tok_t * name, const std::vector< tok_t * > & vals,
		     const bool is_mask, const int tok_ctr );
	~stmt_enum_t();
	void disp( const bool has_next ) const;
	bool bytecode( src_t & src ) const;
};

class stmt_ldmod_t : public stmt_base_t
{
	const std::vector< tok_t * > m_whats;
	const std::vector< std::string > m_full_names;
public:
	stmt_ldmod_t( const std::vector< tok_t * > & whats,
		      const std::vector< std::string > & full_names,
		      const int tok_ctr );
	~stmt_ldmod_t();
	void disp( const bool has_next ) const;
	bool bytecode( src_t & src ) const;
};

class stmt_import_t : public stmt_base_t
{
	const std::vector< tok_t * > m_whats;
	const std::vector< std::string > m_full_names;
public:
	stmt_import_t( const std::vector< tok_t * > & whats,
		       const std::vector< std::string > & full_names,
		       const int tok_ctr );
	~stmt_import_t();
	void disp( const bool has_next ) const;
	bool bytecode( src_t & src ) const;
};

class stmt_expr_t : public stmt_base_t
{
	const stmt_base_t * m_lhs, * m_rhs;
	const stmt_simple_t * m_oper;
public:
	bool m_is_top_expr;
	bool m_triple_dot;
	stmt_expr_t( const stmt_base_t * lhs, const stmt_simple_t * oper,
		     const stmt_base_t * rhs, const int tok_ctr );
	~stmt_expr_t();
	friend void child_comma_count( const stmt_expr_t * expr, int & cc );
	void disp( const bool has_next ) const;
	bool bytecode( src_t & src ) const;
};

struct struct_field_t
{
	const stmt_simple_t * name;
	stmt_expr_t * def_val;
};

class stmt_struct_t : public stmt_base_t
{
	const stmt_simple_t * m_name;
	const std::vector< struct_field_t > m_fields;
public:
	stmt_struct_t( const stmt_simple_t * name, const std::vector< struct_field_t > & fields,
		       const int tok_ctr );
	~stmt_struct_t();
	void disp( const bool has_next ) const;
	bool bytecode( src_t & src ) const;
};

class stmt_block_t : public stmt_base_t
{
	std::vector< stmt_base_t * > * m_stmts;
	bool m_in_func;
public:
	stmt_block_t( std::vector< stmt_base_t * > * stmts, const int tok_ctr, const bool in_func = false );
	~stmt_block_t();
	void disp( const bool has_next ) const;
	bool bytecode( src_t & src ) const;
};

class stmt_func_t : public stmt_base_t
{
	const stmt_simple_t * m_name;
	const stmt_expr_t * m_args;
	const stmt_block_t * m_block;
	const std::vector< stmt_simple_t * > m_mem_types;
public:
	stmt_func_t( const stmt_simple_t * name, const stmt_expr_t * args,
		     const stmt_block_t * block, const std::vector< stmt_simple_t * > & mem_types,
		     const int tok_ctr );
	~stmt_func_t();
	void disp( const bool has_next ) const;
	bool bytecode( src_t & src ) const;
};

enum CallType
{
	CT_FUNC,
	CT_STRUCT,
	CT_SUBSCR,

	_CT_LAST,
};

extern const char * CallTypeStrs[ _CT_LAST ];

class stmt_func_struct_subscr_call_t : public stmt_base_t
{
	const stmt_simple_t * m_name;
public:
	std::vector< stmt_expr_t * > m_args;
	CallType m_ctype;
	bool m_post_dot;
	stmt_func_struct_subscr_call_t( const stmt_simple_t * name, std::vector< stmt_expr_t * > args,
					const int tok_ctr );
	~stmt_func_struct_subscr_call_t();
	void disp( const bool has_next ) const;
	bool bytecode( src_t & src ) const;
};

class stmt_collection_t : public stmt_base_t
{
	const stmt_expr_t * m_vals;
	const int m_line, m_col;
public:
	bool m_is_map;
	int m_arg_count;
	stmt_collection_t( const stmt_expr_t * vals, const int tok_ctr,
			   const int line, const int col );
	~stmt_collection_t();
	void disp( const bool has_next ) const;
	bool bytecode( src_t & src ) const;
};

struct condition_t
{
	const stmt_expr_t * cond;
	const stmt_block_t * block;
};

class stmt_if_t : public stmt_base_t
{
	std::vector< condition_t > m_conds;
public:
	stmt_if_t( const std::vector< condition_t > & conds, const int tok_ctr );
	~stmt_if_t();
	void disp( const bool has_next ) const;
	bool bytecode( src_t & src ) const;
};

class stmt_for_t : public stmt_base_t
{
	stmt_expr_t * m_init, * m_cond, * m_step;
	const stmt_block_t * m_block;
public:
	stmt_for_t( stmt_expr_t * init, stmt_expr_t * cond, stmt_expr_t * step,
		     const stmt_block_t * block, const int tok_ctr );
	~stmt_for_t();
	void disp( const bool has_next ) const;
	bool bytecode( src_t & src ) const;
};

class stmt_foreach_t : public stmt_base_t
{
	const stmt_simple_t * m_iter;
	stmt_expr_t * m_expr;
	const stmt_block_t * m_block;
public:
	stmt_foreach_t( const stmt_simple_t * name, stmt_expr_t * expr,
			const stmt_block_t * block, const int tok_ctr );
	~stmt_foreach_t();
	void disp( const bool has_next ) const;
	bool bytecode( src_t & src ) const;
};

struct stmt_return_t : public stmt_base_t
{
	stmt_expr_t * m_ret_val;
	stmt_return_t( stmt_expr_t * ret_val, const int tok_ctr );
	~stmt_return_t();
	void disp( const bool has_next ) const;
	bool bytecode( src_t & src ) const;
};

struct stmt_continue_t : public stmt_base_t
{
	stmt_continue_t( const int tok_ctr );
	~stmt_continue_t();
	void disp( const bool has_next ) const;
	bool bytecode( src_t & src ) const;
};

struct stmt_break_t : public stmt_base_t
{
	stmt_break_t( const int tok_ctr );
	~stmt_break_t();
	void disp( const bool has_next ) const;
	bool bytecode( src_t & src ) const;
};

#define PARSE_FAIL( ... ) src_fail( src.file, src.code[ ph->peak()->line - 1 ], \
				    ph->peak()->line, ph->peak()->col, __VA_ARGS__ )

#endif // PARSER_COMMON_HPP