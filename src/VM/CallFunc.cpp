/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "ExecInternal.hpp"
#include "CallFunc.hpp"

std::string args_types_to_string( const std::vector< var_base_t * > & args, bool is_mem );

int CallFunc( vm_state_t & vm, const int ins_ctr )
{
	src_t & src = * vm.srcstack.back();
	instr_t & ins = src.bcode[ ins_ctr ];
	int args_count;
	std::string fn_name;
	var_base_t * member = nullptr;
	std::vector< std::string > arg_types;
	std::vector< var_base_t * > args;
	const function_t * fn;
	modfnptr_t mfnptr = nullptr;
	const langfn_t * lfnptr = nullptr;
	// 1 for function name
	int func_name_count = 1;
	res_t< var_base_t * > res = { 0, nullptr };
	std::vector< void * > rem_locs;

	VERIFY_STACK_MIN( 1 );
	args_count = std::stoi( ins.oper.val );
	VERIFY_STACK_MIN( ( size_t )args_count + func_name_count );
	if( vm.stack->back()->type() != VT_STR ) {
		VM_FAIL( "expected a name (identifier) for function call, found '%s'",
			 vm.stack->back()->to_str().c_str() );
		goto fail;
	}
	fn_name = vm.stack->back()->to_str();
	vm.stack->pop_back();
	// fetch args
	for( int i = 0; i < args_count; ++i ) {
		arg_types.push_back( vm.stack->back()->type_str() );
		args.push_back( vm.stack->back() );
		vm.stack->pop_back( false );
	}

	if( ins.opcode == IC_MFN_CALL ) {
		member = vm.stack->back();
		vm.stack->pop_back( false );
		args.insert( args.begin(), member );
		fn = vm.typefuncs[ member->type_str() ].get( fn_name, args_count, arg_types );
		if( fn == nullptr ) {
			fn = vm.typefuncs[ "_any_" ].get( fn_name, args_count, arg_types );
		}
	} else {
		// fetch the function pointer from Functions
		fn = vm.funcs.get( fn_name, args_count, arg_types );
	}
	if( fn == nullptr ) {
		VM_FAIL( "%sfunction with name '%s' and arg count %d (%s) does not exist",
			 ins.opcode == IC_MFN_CALL ? "member " : "", fn_name.c_str(),
			 args_count, args_types_to_string( args, member ).c_str() );
		goto fail;
	}
	if( fn->type == FnType::MODULE ) mfnptr = fn->func.modfn;
	else lfnptr = & fn->func.langfn;
	if( mfnptr == nullptr && lfnptr == nullptr ) {
		VM_FAIL( "function with name '%s' is null", fn_name.c_str() );
		goto fail;
	}

	vm.vars->add_scope();

	// execute the function
	res.code = E_OK;
	if( mfnptr ) {
		res.data = mfnptr( args );
		// for lang function, args are moved to vars' new layer which is popped at the end
		// member is also added to args and hence erased
		for( auto & arg : args ) VAR_DREF( arg );
	} else {
		vm.srcstack.push_back( vm.srcs[ lfnptr->src ] );
		if( member ) {
			args.erase( args.begin() );
			vm.vars->add( "self", member );
		}
		for( size_t i = 0; i < fn->arg_types.size(); ++i ) {
			vm.vars->add( fn->arg_types[ i ], args[ i ] );
		}
		args.clear();
		res.code = exec_internal( vm, lfnptr->beg, lfnptr->end, res.data );
		vm.srcstack.pop_back();
		if( res.code != E_OK ) {
			VM_FAIL( "function '%s' failed to execute properly", fn_name.c_str() );
			goto fail;
		}
	}
	if( mfnptr && res.data != nullptr ) {
		vm.stack->push_back( res.data, !fn->manual_res_free );
	}

	vm.vars->pop_scope( & rem_locs );
	for( auto & rll : rem_locs ) VAR_DREF( rll );
	return E_OK;
fail:
	// for lang function, args are moved to vars' new layer which is popped at the end
	vm.vars->pop_scope( & rem_locs );
	for( auto & arg : args ) VAR_DREF( arg );
	for( auto & rll : rem_locs ) VAR_DREF( rll );
	return E_VM_FAIL;
}

std::string args_types_to_string( const std::vector< var_base_t * > & args, bool is_mem )
{
	std::string res = "";
	size_t size = args.size();
	for( size_t i = 0 + is_mem; i < size; ++i ) {
		res += args[ i ]->type_str();
		if( i < size - 1 ) res += ", ";
	}
	return res;
}