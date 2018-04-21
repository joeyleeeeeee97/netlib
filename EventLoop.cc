#include "EventLoop.h"
#include "ThreadFunc.h"
#include <assert.h>
#include <poll.h>
using namespace netlib;


thread_local EventLoop* t_LoopInThisThread = 0;

EventLoop::EventLoop() :looping(false) {
	_tid = curThreadId();
//	LOG_TRACE << "EventLoop Created " << this << " in thread " << _tid;
	if (t_LoopInThisThread) {
//		LOG_FATAL << "Another EventLoop " << t_LoopInThisThread << " Exists in this Thread " << _tid;
	}
	else {
		t_LoopInThisThread = this;
	}
}

EventLoop::~EventLoop() {
	assert(!looping);
	t_LoopInThisThread = NULL;
}

void EventLoop::loop() {
	assert(!looping);
	assert(_tid == curThreadId());
	looping = true;

	::poll(NULL, 0, 5 * 1000);

//	LOG_TRACE << "EventLoop " << this << " stop looping ";
	looping = false;
}
