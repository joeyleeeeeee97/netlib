#include "EventLoopThread.h"
#include "EventLoop.h"
#include <functional>

using namespace netlib;

EventLoopThread::EventLoopThread():loop(NULL), 
				   exiting(false),
				   lk(), cond(),
				   thread(&EventLoopThread::threadFunc,this){
//	std::function<void()> f = std::bind(&EventLoopThread::threadFunc, this);
//	thread = std::move(thread(f));
}

EventLoopThread::~EventLoopThread() {
	exiting = true;
	loop->quitloop();
	thread.join();
}

EventLoop* EventLoopThread::startLoop()
{
	assert(!thread.hasStarted());
	thread.start();
	std::unique_lock<std::mutex> lock(lk);
	cond.wait(lock,[this]{return loop != NULL;});
	return loop;
}

void EventLoopThread::threadFunc() {
	EventLoop el;
	{
		std::lock_guard<std::mutex> lock(lk);
		loop = &el;
		cond.notify_one();
	}
	loop->loop();

}
