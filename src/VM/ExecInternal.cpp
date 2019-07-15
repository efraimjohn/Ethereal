/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../FE/FS.hpp"

#include "ExecInternal.hpp"
#include "CallFunc.hpp"
#include "LoadFile.hpp"

int exec_internal( vm_state_t & vm, long begin, long end )
{
	src_t & src = * vm.srcstack.back();
	begin = begin == -1 ? 0 : begin;
	end = end == -1 ? src.bcode.size() : end;

	for( int i = begin; i < end; ++i ) {
		instr_t & ins = src.bcode[ i ];

		switch( ins.opcode ) {
		case IC_PUSH: {
			var_base_t * val = nullptr;
			if( ins.oper.type == OP_STR ) {
				val = src.vars.get( ins.oper.val );
			} else {
				val = vm.consts.get( ins.oper.val, ins.oper.type );
			}
			if( val == nullptr ) {
				VM_FAIL( "%s '%s' does not exist", ins.oper.type == OP_STR ? "variable" : "constant",
					 ins.oper.val.c_str() );
				goto fail;
			}
			vm.stack->push_back( val );
			break;
		}
		case IC_POP: {
			if( vm.stack->size() == 0 ) {
				VM_FAIL( "cannot pop from vm stack since it is already empty" );
				goto fail;
			}
			vm.stack->pop_back( false );
			break;
		}
		case IC_STORE: // fallthrough
		case IC_STORE_LOAD: {
			std::string var;
			if( ins.oper.type == OP_NONE ) {
				VERIFY_STACK_MIN( 1 );
				var = vm.stack->back()->to_str();
				vm.stack->pop_back();
			} else {
				// TODO:
			}
			var_base_t * newval = vm.stack->back();
			if( src.vars.exists( var, true ) ) {
				var_base_t * val = src.vars.get( var );
				if( val->type() != newval->type() ) {
					VM_FAIL( "variable '%s' already declared at previous location, but with different data type" );
					VM_FAIL_TOK_CTR( val->parse_ctr(), "original declared here" );
					goto fail;
				}
				val->swap( newval );
				vm.stack->pop_back();
				if( ins.opcode == IC_STORE_LOAD ) {
					vm.stack->push_back( val );
				}
				break;
			}
			src.vars.add( var, newval->copy() );
			vm.stack->pop_back();
			if( ins.opcode == IC_STORE_LOAD ) {
				vm.stack->push_back( src.vars.get( var ) );
			}
			break;
		}
		case IC_LDMOD: {
			std::string module_name = ins.oper.val + ".so";
			if( ins.oper.val.front() != '~' && ins.oper.val.front() != '/' && ins.oper.val.front() != '.' ) {
				module_name = STRINGIFY( BUILD_PREFIX_DIR ) "/lib/ethereal/lib" + ins.oper.val + ".so";
			}
			if( !fexists( module_name ) ) {
				VM_FAIL( "failed to locate module file '%s'", module_name.c_str() );
				goto fail;
			}
			if( vm.dlib.load( module_name ) == nullptr ) goto fail;
			init_fnptr_t init_fn = ( init_fnptr_t ) vm.dlib.get( module_name, "init_" + ins.oper.val );
			if( init_fn == nullptr ) {
				VM_FAIL( "failed to find init function in module '%s'\n", module_name.c_str() );
				goto fail;
			}
			init_fn( vm );
			break;
		}
		case IC_IMPORT: {
			const int args = std::stoi( ins.oper.val );
			VERIFY_STACK_MIN( ( size_t )args );
			std::string alias = "";
			if( args > 1 ) {
				alias = vm.stack->back()->to_str();
				vm.stack->pop_back();
			}
			const std::string file = vm.stack->back()->to_str();
			vm.stack->pop_back();
			

			int ret = load_src( vm, file, alias, ins );

			if( ret != E_OK ) {
				VM_FAIL( "could not import '%s', see the error above; aborting",
					 file.c_str() );
				goto fail;
			}
			break;
		}
		case IC_FN_CALL: {
			int res = CallFunc( vm, i );
			if( res != E_OK ) goto fail;
			break;
		}
		case _IC_LAST: {}
		}
	}

	return E_OK;
fail:
	return E_VM_FAIL;
}