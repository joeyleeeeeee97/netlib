#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"
#include <assert.h>
using namespace netlib;


thread_local EventLoop* t_LoopInThisThread = 0;
const int kPollTimeMs = 10000;

EventLoop::EventLoop() :looping(false), quit(false), poller(new Poller(this)) {
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
	quit = false;
//	::poll(NULL, 0, 5 * 1000);
	while(!quit) {
		activeChannels.clear();
		poller->poll(kPollTimeMs,&activeChannels);
		for(Channel* it : activeChannels) {
			it->handleEvent();
		}
	}
//	LOG_TRACE << "EventLoop " << this << " stop looping "
	looping = false;
}

void EventLoop::quitloop(){
	quit = true;
}

void EventLoop::updateChannel(Channel* channel) {
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	poller->updateChannel(channel);
}




