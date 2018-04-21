#include "ThreadFunc.h"
namespace netlib{
	ThreadId curThreadId(){
		thread_local static ThreadId  cache = std::this_thread::get_id();
		return cache;
	}
}
