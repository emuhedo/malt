/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//extern
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iomanip>
#include <iostream>
//os specific (TODO move)
#include <sys/time.h>
#include <unistd.h>
//locals
#include "CodeTiming.hpp"
#include "Helpers.hpp"

/***************** USING NAMESPACE ******************/
using namespace std;

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************** CONSTS **********************/
//Init static values of CodeTiming
ticks CodeTiming::globalStart = 0;
size_t CodeTiming::globalCntTimers = 0;
size_t CodeTiming::globalCntTimersFinished = 0;
CodeTiming * CodeTiming::globalTimers[64];

/*******************  FUNCTION  *********************/
/**
 * Used to register the timers (called from their constructor). All registered
 * timers will be print by the static function CodeTiming::printAll().
 * Timers must be allocated static or global to stay alive until usage of printAll().
 * 
 * @param timer Pointer to the timer to register.
**/
void CodeTiming::registerTimer(CodeTiming* timer)
{
	assert(timer != NULL);
	if (globalStart == 0)
		globalStart = getticks();
	assert(globalCntTimers < sizeof(globalTimers)/sizeof(globalTimers[0]));
	globalTimers[globalCntTimers++] = timer;
}

/*******************  FUNCTION  *********************/
/**
 * Just follow the status of registered timers. This functions might be called
 * from CodeTiming destructor.
 * @param timer Pointer to the finshed timer.
**/
void CodeTiming::timerFinish(CodeTiming* timer)
{
	assert(timer != NULL);
	globalCntTimersFinished++;
}

/*******************  FUNCTION  *********************/
/**
 * Compare function to be used into sort() to order timers before printing.
 * @param a Pointer to first timer (need to be CodeTiming).
 * @param b Pointer to second timer (need to be CodeTiming).
**/
int CodeTiming::compare(const void* a, const void* b)
{
	ticks sa = (*(const CodeTiming**)a)->sum;
	ticks sb = (*(const CodeTiming**)b)->sum;
	
	if (sa == sb)
		return 0;
	else if (sa < sb)
		return 1;
	else
		return -1;
}

/*******************  FUNCTION  *********************/
/**
 * Static function to print final restults of all registered timers. This function
 * will order the results on asc order based on the total cost (sum).
 * It will print on std::cerr.
**/
void CodeTiming::printAll(std::ostream & out)
{
	#ifdef MATT_ENABLE_CODE_TIMING
		out << "=============================================================== MATT TIMINGS ====================================================================" << endl;
		qsort(globalTimers,globalCntTimers,sizeof(globalTimers[0]),CodeTiming::compare);
		for (int i = 0 ; i < globalCntTimers ; i++)
		{	
			globalTimers[i]->finalPrint();
		}
		out << "=================================================================================================================================================" << endl;
	#endif
}

/*******************  FUNCTION  *********************/
/**
 * Constructor of CodeTiming class. Only setup default parameters and register the object
 * into global registry.
 * @param name Define the name of the current timer.
**/
CodeTiming::CodeTiming(const char* name)
{
	this->name = name;
	this->count = 0;
	this->max = 0;
	this->min = 0;
	this->sum = 0;
	#ifdef MATT_ENABLE_CODE_TIMING
		registerTimer(this);
	#endif
}

/*******************  FUNCTION  *********************/
/**
 * The destructor is in charge of printing the final results.
**/
CodeTiming::~CodeTiming(void)
{
	#ifdef MATT_ENABLE_CODE_TIMING
		timerFinish(this);
	#endif
}

/*******************  FUNCTION  *********************/
void CodeTiming::finalPrint(ostream& out) const
{
	#ifdef MATT_ENABLE_CODE_TIMING
		ticks wholeExec = getticks() - globalStart;
		double ratio = (double)(100*sum) / (double)wholeExec;
		out << "TIMING OF " << std::setw(32) << std::left << this->name << " => [ ";
		Helpers::printValue(out,min);
		out << " , ";
		Helpers::printValue(out,(double)sum / (double)count);
		out << " , ";
		Helpers::printValue(out,max);
		out << " ] => TOTAL ( calls : ";
		Helpers::printValue(out,count);
		out << " , time : ";
		Helpers::printValue(out,sum);
		out << " , " << "ratio : ~ " << std::setprecision( 2 ) << setw(8) << std::left << ratio << "% )" << endl;
	#endif
}

/*******************  FUNCTION  *********************/
/**
 * @return Return the number of ticks per seconds on the current CPU.
 * The measurement is done by mixing informations from getticks and gettimeofday.
**/
ticks ticksPerSecond(void)
{
	//remember the value to not measure it every time
	static ticks alreadyMeasured = 0;
	
	//measure if required
	if (alreadyMeasured == 0)
	{
		//measure
		timeval ts0,ts1;
		ticks t0,t1;
		gettimeofday(&ts0,NULL);
		t0 = getticks();
// 		sleep(1);
		t1 = getticks();
		gettimeofday(&ts1,NULL);
		
		//compute delta and store
		timeval delta;
		timersub(&ts1,&ts0,&delta);
		alreadyMeasured = (double)(t1-t0)/((double)delta.tv_sec + (double)delta.tv_usec/(double)1000000.0);
	}
	
	//final return
	return alreadyMeasured;
}

}
