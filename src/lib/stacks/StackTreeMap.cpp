/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cassert>
#include "StackTreeMap.hpp"
#include "BacktraceStack.hpp"
#include "EnterExitStack.hpp"
#include "RLockFreeTree.hpp"
#include <common/Debug.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
StackTreeMap::Key::Key(const Stack* stack,int id)
{
	assert(stack != NULL);
	this->stack = stack;
	this->hash = stack->hash();
	this->id =  id;
}

/*******************  FUNCTION  *********************/
bool StackTreeMap::Key::operator==(const Key& node) const
{
	if (!(hash == node.hash))
		return false;
	else
		return *stack == *node.stack;
}

/*******************  FUNCTION  *********************/
bool StackTreeMap::Key::operator<(const Key& node) const
{
	if (hash < node.hash)
	{
		return true;
	} else if (hash == node.hash) {
		return *stack < *node.stack;
	} else {
		return false;
	}
}

/*******************  FUNCTION  *********************/
void StackTreeMap::Key::cloneStack(void)
{
	assert(stack != NULL);
	void * ptr = MATT_MALLOC(sizeof(Stack));
	stack = new(ptr) Stack(*stack);
}

/*******************  FUNCTION  *********************/
StackTreeMap::StackTreeMap(bool backtrace,bool threadsafe)
	: StackTree()
{
	this->backtrace = backtrace;
	this->threadSafe = threadsafe;
	this->nextId = 0;
}

/*******************  FUNCTION  *********************/
StackTreeMap::~StackTreeMap(void)
{

}

/*******************  FUNCTION  *********************/
StackTreeHandler StackTreeMap::enterThread(void)
{
	Handler * handler = new Handler;
	handler->storage = NULL;
	return handler;
}

/*******************  FUNCTION  *********************/
StackTreeHandler StackTreeMap::exitFunction(StackTreeHandler handler, void* callsite)
{
	assume(!backtrace,"Try to use exitFunction with backtrace mode enabled !");
	Handler * typedHandler = (Handler*)handler;
	typedHandler->enterExitStack.exitFunction(callsite);
	typedHandler->storage = NULL;
	return handler;
}

/*******************  FUNCTION  *********************/
StackTreeHandler StackTreeMap::enterFunction(StackTreeHandler handler, void* callsite)
{
	assume(!backtrace,"Try to use exitFunction with backtrace mode enabled !");
	Handler * typedHandler = (Handler*)handler;
	typedHandler->enterExitStack.enterFunction(callsite);
	typedHandler->storage = NULL;
	return handler;
}

/*******************  FUNCTION  *********************/
void StackTreeMap::exitThread(StackTreeHandler handler)
{
	Handler * typedHandler = (Handler*)handler;
	delete typedHandler;
}

/*******************  FUNCTION  *********************/
void* & StackTreeMap::internalGetData(StackTreeHandler handler, int id)
{
	assert(id < MATT_STACK_TREE_ENTRIES);

	//get handler
	Handler * typedHandler = (Handler*)handler;
	StackTreeStorage * storage = typedHandler->storage;
	
	//check if ok
	if (storage == NULL)
	{
		//get stack
		if (backtrace)
			typedHandler = (Handler*)getFromStack(handler,typedHandler->backtraceStack);
		else
			typedHandler = (Handler*)getFromStack(handler,typedHandler->enterExitStack);
		storage = typedHandler->storage;
	}
	
	//entry
	void * & entry = (*storage)[id];
	
	//allocate
	if (entry == NULL)
		entry = descriptors[id]->allocate();
	
	//return
	return entry;
}

/*******************  FUNCTION  *********************/
void* StackTreeMap::getData(StackTreeHandler handler, int id)
{
	return internalGetData(handler,id);
}

/*******************  FUNCTION  *********************/
StackTreeHandler StackTreeMap::getFromStack(StackTreeHandler handler, const Stack& stack)
{
	//get handler
	Handler * typedHandler = (Handler*)handler;
	
	//build key
	Key key(&stack,nextId);
	
	//search
	NodeMap::iterator it = map.find(key);
	
	//if not found, create
	if (it == map.end())
	{
		key.cloneStack();
		StackTreeStorage storage;
		map[key] = storage;
		nextId++;
		it = map.find(key);
	}
	
	//return
	typedHandler->storage = &(it->second);
	return handler;
}

/*******************  FUNCTION  *********************/
StackTreeHandler StackTreeMap::getFromStack(StackTreeHandler handler, int skip)
{
	assume(backtrace,"Invlid mode, require backtrace enabled !");
	//get handler
	Handler * typedHandler = (Handler*)handler;
	typedHandler->backtraceStack.loadCurrentStack();
	typedHandler->backtraceStack.fastSkip(skip+1);
	return getFromStack(handler,typedHandler->backtraceStack);
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const StackTreeMap& tree)
{
	RLockFreeTree otree;
	StackTreeHandler handler = otree.enterThread();
	
	//copy descr
	for (int i = 0 ; i < MATT_STACK_TREE_ENTRIES ; i++)
		if (tree.descriptors[i] != NULL)
			otree.addDescriptor(tree.names[i],tree.descriptors[i]);
		
	//copy values
	for (StackTreeMap::NodeMap::const_iterator it = tree.map.begin() ; it != tree.map.end() ; ++it)
		otree.copyData(*it->first.stack,it->second);
	
	otree.exitThread(handler);
	
	convertToJson(json,otree);
}

}