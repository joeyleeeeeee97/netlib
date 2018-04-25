#ifndef _THREADLOOP_H
#define _THREADLOOP_H
#include <mutex>
#include <condition_variable>
#include "ThreadFunc.h"
#include "nocopyable.h"

namespace netlib{

class EventLoop;

class EventLoopThread : nocopyable {
public:
	EventLoopThread();
	~EventLoopThread();
	EventLoop* startLoop();
private:
	void threadFunc();
	
	EventLoop* loop;
	bool exiting;
	ThreadFunc thread;
	std::mutex lk;
	std::condition_variable cond; 

};


}



#endif
