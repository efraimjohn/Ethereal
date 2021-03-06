/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_MODULES_CORE_FLT_HPP
#define VM_MODULES_CORE_FLT_HPP

#include "../../../src/VM/Core.hpp"

#define DECL_FUNC_ALLOC__FLT( name, oper, ret_type )			\
	var_base_t * name( vm_state_t & vm, func_call_data_t & fcd )	\
	{								\
		auto & lhs = AS_FLT( fcd.args[ 1 ] )->get();		\
		auto & rhs = AS_FLT( fcd.args[ 0 ] )->get();		\
		return new ret_type( lhs oper rhs );			\
	}

#define DECL_FUNC_ALLOC__INT_FLT( name, oper, ret_type )		\
	var_base_t * name( vm_state_t & vm, func_call_data_t & fcd )	\
	{								\
		auto & lhs = AS_INT( fcd.args[ 1 ] )->get();		\
		auto & rhs = AS_FLT( fcd.args[ 0 ] )->get();		\
		return new ret_type( lhs.get_mpz_t() oper rhs );	\
	}

#define DECL_FUNC_ALLOC__FLT_INT( name, oper, ret_type )		\
	var_base_t * name( vm_state_t & vm, func_call_data_t & fcd )	\
	{								\
		auto & lhs = AS_FLT( fcd.args[ 1 ] )->get();		\
		auto & rhs = AS_INT( fcd.args[ 0 ] )->get();		\
		return new ret_type( lhs oper rhs.get_mpz_t() );	\
	}

#define DECL_FUNC_ASSN__FLT( name, oper )				\
	var_base_t * name( vm_state_t & vm, func_call_data_t & fcd )	\
	{	/* lhs = 0 because Right to Left associativity */	\
		auto & lhs = AS_FLT( fcd.args[ 0 ] )->get();		\
		auto & rhs = AS_FLT( fcd.args[ 1 ] )->get();		\
		lhs oper rhs;						\
		return fcd.args[ 0 ];					\
	}

#define DECL_FUNC_BOOL__FLT( name, oper )				\
	var_base_t * name( vm_state_t & vm, func_call_data_t & fcd )	\
	{								\
		auto & lhs = AS_FLT( fcd.args[ 1 ] )->get();		\
		auto & rhs = AS_FLT( fcd.args[ 0 ] )->get();		\
		return TRUE_FALSE( lhs oper rhs );			\
	}

/*
 * Basic arithmetic operators
 */
DECL_FUNC_ALLOC__FLT( addf, +, var_flt_t )
DECL_FUNC_ALLOC__FLT( subf, -, var_flt_t )
DECL_FUNC_ALLOC__FLT( mulf, *, var_flt_t )
DECL_FUNC_ALLOC__FLT( divf, /, var_flt_t )

DECL_FUNC_ALLOC__INT_FLT( addif, +, var_flt_t )
DECL_FUNC_ALLOC__INT_FLT( subif, -, var_flt_t )
DECL_FUNC_ALLOC__INT_FLT( mulif, *, var_flt_t )
DECL_FUNC_ALLOC__INT_FLT( divif, /, var_flt_t )

DECL_FUNC_ALLOC__FLT_INT( addfi, +, var_flt_t )
DECL_FUNC_ALLOC__FLT_INT( subfi, -, var_flt_t )
DECL_FUNC_ALLOC__FLT_INT( mulfi, *, var_flt_t )
DECL_FUNC_ALLOC__FLT_INT( divfi, /, var_flt_t )

/*
 * Basic arithmetic operators and assign to LHS
 */
DECL_FUNC_ASSN__FLT( add_assnf, += )
DECL_FUNC_ASSN__FLT( sub_assnf, -= )
DECL_FUNC_ASSN__FLT( mul_assnf, *= )
DECL_FUNC_ASSN__FLT( div_assnf, /= )

/*
 * comparison between floating point values
 */
DECL_FUNC_BOOL__FLT( eqf, == )
DECL_FUNC_BOOL__FLT( nef, != )
DECL_FUNC_BOOL__FLT( ltf, < )
DECL_FUNC_BOOL__FLT( lef, <= )
DECL_FUNC_BOOL__FLT( gtf, > )
DECL_FUNC_BOOL__FLT( gef, >= )

/*
 * raise a floating point number LHS to power integer RHS
 */
var_base_t * powerf( vm_state_t & vm, func_call_data_t & fcd )
{
	mpfr::mpreal & lhs = AS_FLT( fcd.args[ 1 ] )->get();
	mpz_class & rhs = AS_INT( fcd.args[ 0 ] )->get();
	return new var_flt_t( mpfr::pow( lhs, rhs.get_mpz_t() ) );
}

/*
 * returns negative value of the given floating point argument
 */
var_base_t * unary_subf( vm_state_t & vm, func_call_data_t & fcd )
{
	mpfr::mpreal & num = AS_FLT( fcd.args[ 0 ] )->get();
	return new var_flt_t( -num );
}

/*
 * boolean not: converts float to its negative boolean
 * !(5.0) => false
 * !(0.0) => true
 */
var_base_t * not_operf( vm_state_t & vm, func_call_data_t & fcd )
{
	mpfr::mpreal & num = AS_FLT( fcd.args[ 0 ] )->get();
	return TRUE_FALSE( !num );
}

/*
 * converts a string to float
 */
var_base_t * flt( vm_state_t & vm, func_call_data_t & fcd )
{
	const std::string & flt_str = fcd.args[ 0 ]->to_str();
	return new var_flt_t( flt_str );
}

/*
 * returns a hash string from float
 */
var_base_t * hash_flt( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_str_t( fcd.args[ 0 ]->to_str() );
}

#endif // VM_MODULES_CORE_FLT_HPP
