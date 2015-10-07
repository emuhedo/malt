/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALTV2_BACKTRACE_STACK_HPP
#define MALTV2_BACKTRACE_STACK_HPP

/********************  HEADERS  *********************/
//internal stacks
#include "Stack.hpp"
//internal core
// #include <core/SymbolSolver.hpp>
// #include <core/CallStackInfo.hpp>

/*******************  NAMESPACE  ********************/
namespace MALTV2
{

/*********************  CLASS  **********************/
/**
 * Implement a specific class to provide backtrace integration on top of our internal
 * stack representation.
**/
class BacktraceStack : public Stack
{
	public:
		BacktraceStack(void);
		void loadCurrentStack(void);
};

}

#endif //MALTV2_BACKTRACE_STACK_HPP
