#ifndef ALLOC_STATCK_PROFILER_H
#define ALLOC_STATCK_PROFILER_H

#include "SimpleTracer.h"

class AllocStackProfiler
{
	public:
		void init(void);
		void onMalloc(void * ptr,size_t size);
		void onCalloc(void * ptr,size_t nmemb,size_t size);
		void onPrepareRealloc(void * oldPtr);
		void onRealloc(void * oldPtr,void * ptr,size_t newSize);
		void onFree(void * ptr);
		void onExit(void);
	private:
		void countCalls(int skipDepth);
	private:
		SimpleTracer * tracer;
};

#endif //ALLOC_STATCK_PROFILER_H
