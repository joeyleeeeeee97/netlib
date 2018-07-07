#include "TimerQueue.h"
#include "EventLoop.h"
#include "Timer.h"
#include "TimerId.h"
#include <iostream>
#include <functional>
#include <sys/timerfd.h>
#include <unistd.h>
#include <strings.h>
#include <algorithm>

namespace netlib {
int createTimerfd()
{
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                 TFD_NONBLOCK | TFD_CLOEXEC);
  return timerfd;
}
struct timespec howMuchTimeFromNow(Timestamp when)
{
  int64_t microseconds = when.microSecondsSinceEpoch()
                         - Timestamp::now().microSecondsSinceEpoch();
  if (microseconds < 100)
  {
    microseconds = 100;
  }
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(
      microseconds / Timestamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(
      (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
  return ts;
}
void readTimerfd(int timerfd, Timestamp now)
{
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
  if (n != sizeof howmany)
  {
  }
}
void resetTimerfd(int timerfd, Timestamp expiration)
{
  // wake up loop by timerfd_settime()
  struct itimerspec newValue;
  struct itimerspec oldValue;
  bzero(&newValue, sizeof newValue);
  bzero(&oldValue, sizeof oldValue);
  newValue.it_value = howMuchTimeFromNow(expiration);
  int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
}
}

using namespace netlib;

TimerQueue::TimerQueue(EventLoop* _loop) : loop(_loop), timerfd(createTimerfd()), timerfdChannel(_loop, timerfd), timers(), callingExpiredTimers_(false){
	std::function<void(Timestamp)> f = std::bind(&TimerQueue::handleRead, this, std::placeholders::_1);
	timerfdChannel.setReadCallBack(f);
	timerfdChannel.enableReading();
}

TimerQueue::~TimerQueue(){
	::close(timerfd);
}
/*
TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double Interval) {
	Timer* timer = new Timer(cb, when, Interval);
	loop->assertInLoopThread();
	std::shared_ptr<Timer> ptr(timer);
	bool earliestChanged = insert(ptr);

	if(earliestChanged) {
		resetTimerfd(timerfd, timer->expiration());
	}

	return TimerId(timer);
}
*/
void TimerQueue::cancel(TimerId timerId) {
	Functor f = std::bind(&TimerQueue::cancelInLoop, this , timerId);	
	loop->runInLoop(f);
}

void TimerQueue::cancelInLoop(TimerId timerId) {
	loop->assertInLoopThread();
	ActiveTimer timer(std::shared_ptr<Timer>(timerId.value_), timerId.seq_);
	ActiveTimerSet::iterator it = activeTimers_.find(timer);
	if( it != activeTimers_.end()) {
		size_t n = timers.erase(Entry(it->first->expiration(), it->first));
		activeTimers_.erase(it);
	}
	else if (callingExpiredTimers_ ){
		cancelTimers_.insert(timer);
	}

}

TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double Interval) {
	Timer* timer = new Timer(cb, when ,Interval);
	std::function<void()> f = std::bind(&TimerQueue::addTimerInLoop, this , timer);
	loop->runInLoop(f);
	return TimerId(timer);
}

void TimerQueue::addTimerInLoop(Timer* timer) {
	loop->assertInLoopThread();
	std::shared_ptr<Timer> ptr(timer);
	bool earliestChanged = insert(ptr);
	if(earliestChanged) {
		resetTimerfd(timerfd, timer->expiration());
	}
}

void TimerQueue::handleRead(Timestamp t){
	loop->assertInLoopThread();
	Timestamp now(Timestamp::now());
	readTimerfd(timerfd, now);
	std::vector<Entry> expired = getExpired(now);
	
	callingExpiredTimers_ = true;
	cancelTimers_.clear();
	
	for(Entry& it : expired) {
		it.second->run();
	}

	callingExpiredTimers_ = false;
	reset(expired, now);
}


std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
	std::vector<Entry> expired;
	Entry sentry = std::make_pair(now, std::shared_ptr<Timer>() );
	auto it = timers.lower_bound(sentry);
	assert(it == timers.end() || now < it->first);
	std::copy(timers.begin(), it ,std::back_inserter(expired));
	timers.erase(timers.begin(), it);
	
	for(auto& entry : expired) {
		ActiveTimer timer(entry.second, entry.second->sequence());
		activeTimers_.erase(timer);
	}
	return expired;
}

void TimerQueue::reset(std::vector<TimerQueue::Entry>& expired, Timestamp now) {
	Timestamp newExpire;

	for(auto& it : expired){
		ActiveTimer timer(it.second, it.second->sequence());

		if(it.second->repeat() && cancelTimers_.find(timer) ==cancelTimers_.end()) {
			it.second->restart(now);
			insert(std::move(it.second));
		}
	}

	if(!timers.empty()) {
		newExpire = timers.begin()->second->expiration();
	}

	if(newExpire.valid()) {
		resetTimerfd(timerfd, newExpire);
	}

}


bool TimerQueue::insert(const std::shared_ptr<Timer>& ptr) {
	loop->assertInLoopThread();
	bool flag = false;
	Timestamp when = ptr->expiration();
	if(timers.empty() || when < timers.begin()->first) {
		flag = true;
	}
	auto result = timers.insert({when, ptr});
	activeTimers_.insert({ptr, ptr->sequence()});
	//activeTimers.insert(ActiveTimer(timer, timer->sequence()));
	return flag;
}




