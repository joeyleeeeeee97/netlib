#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"
#include <assert.h>
#include "TimerQueue.h"
#include <iostream>
using namespace netlib;


thread_local EventLoop* t_LoopInThisThread = 0;
const int kPollTimeMs = 10000;

EventLoop::EventLoop() :looping(false), quit(false),_tid(curThreadId()), poller(new Poller(this)){//, timerQueue(new TimerQueue(this)) {
	std::cout<<_tid<<std::endl;
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
		pollReturnTime = poller->poll(kPollTimeMs,&activeChannels);
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
	std::cout<<_tid<<" and " << curThreadId()<<std::endl;
	assert(_tid == curThreadId());
//	assertInLoopThread();
	poller->updateChannel(channel);
}

TimerId EventLoop::runAt(const Timestamp& time, const TimerCallback& cb){
	return timerQueue->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb){
	Timestamp time(addTime(Timestamp::now(), delay));
	return runAt(time, cb);
	
}


TimerId EventLoop::runEvery(double Interval, const TimerCallback& cb) {
	Timestamp time(addTime(Timestamp::now(), Interval));
	return timerQueue->addTimer(cb, time, Interval);

}
