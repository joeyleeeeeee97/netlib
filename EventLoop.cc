#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"
#include <assert.h>
#include "TimerQueue.h"
#include <functional>
#include <sys/eventfd.h>
#include <unistd.h>
#include <signal.h>

using namespace netlib;


thread_local EventLoop* t_LoopInThisThread = 0;
const int kPollTimeMs = 10000;

class IgnoreSigPipe{ 
public:
	IgnoreSigPipe() {
		::signal(SIGPIPE, SIG_IGN);
	}
};

IgnoreSigPipe initObj;


static int createEventfd() {
	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	assert(evtfd >= 0);
	return evtfd;
}

EventLoop::EventLoop() :looping(false),
			quit(false),
			callPendingFunctors(false),
			_tid(curThreadId()),
			poller(new Poller(this)),
			timerQueue(new TimerQueue(this)),
			wakeupFd(createEventfd()),
			wakeupChannel(new Channel(this, wakeupFd)) {
	if (t_LoopInThisThread) {
        //FATAL ERROR
    }
	else {
		t_LoopInThisThread = this;
	}
	std::function<void(Timestamp)> f = std::bind(&EventLoop::handleRead, this, std::placeholders::_1);
	wakeupChannel->setReadCallBack(f);
	wakeupChannel->enableReading();
}

EventLoop::~EventLoop() {
	assert(!looping);
	::close(wakeupFd);
	t_LoopInThisThread = NULL;
}

void EventLoop::loop() {
	assert(!looping);
	assert(_tid == curThreadId());
	looping = true;
	quit = false;

	while(!quit) {
		activeChannels.clear();
		pollReturnTime = poller->poll(kPollTimeMs,&activeChannels);
		for(Channel* it : activeChannels) {
			it->handleEvent(pollReturnTime);
		}

		doPendingFunctors();
	}

	looping = false;
}

void EventLoop::quitloop(){
	quit = true;
	if(!isInLoopThread()) {
		wakeup();
	}
}

void EventLoop::runInLoop(const Functor& cb) {
	if(isInLoopThread()) {
		cb();
	}
	else {
		queueInLoop(cb);
	}
}

void EventLoop::queueInLoop(const Functor& cb) {
	{
		std::lock_guard<std::mutex> lock(lk);
		pendingFunctors.push_back(cb);
	}
	if(!isInLoopThread() || callPendingFunctors) {
		wakeup();
	}
}


void EventLoop::updateChannel(Channel* channel) {
	assert(channel->ownerLoop() == this);
	assert(_tid == curThreadId());
	poller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
	assert(channel->ownerLoop() == this);
	assert(_tid == curThreadId());
	poller->removeChannel(channel);
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

void EventLoop::cancel(TimerId timerId) {
	return timerQueue->cancel(timerId);
}


void EventLoop::handleRead(Timestamp) {
	uint64_t one = 1;
	ssize_t n = ::read(wakeupFd, &one, sizeof(one));
}

void EventLoop::wakeup() {
	uint64_t one = 1;
	ssize_t n = ::write(wakeupFd, &one ,sizeof(one));

}

void EventLoop::doPendingFunctors() {
	std::vector<Functor> funcs;
	callPendingFunctors = true;
	{
		std::lock_guard<std::mutex> lock(lk);
		std::swap(funcs,pendingFunctors);
	}

	for(auto& it : funcs) {
		it();
	}
	callPendingFunctors = false;

}
