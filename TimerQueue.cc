#include "TimerQueue.h"
#include "EventLoop.h"
#include "Timer.h"
#include "TimerId.h"
#include <iostream>
#include <functional>
#include <sys/timerfd.h>
#include <unistd.h>
#include <strings.h>

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

TimerQueue::TimerQueue(EventLoop* _loop) : loop(_loop), timerfd(createTimerfd()), timerfdChannel(_loop, timerfd), timers(){
	std::function<void()> f = std::bind(&TimerQueue::handleRead, this);
	timerfdChannel.setReadCallBack(f);
	std::cout<<"setTC";
	timerfdChannel.enableReading();
	std::cout<<"Done";
}

TimerQueue::~TimerQueue(){
	::close(timerfd);
}

TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double Interval) {
	Timer* timer = new Timer(cb, when, Interval);
	loop->assertInLoopThread();
	std::unique_ptr<Timer> ptr(timer);
	bool earliestChanged = insert(std::move(ptr));

	if(earliestChanged) {
		resetTimerfd(timerfd, timer->expiration());
	}

	return TimerId(timer);
}

void TimerQueue::handleRead(){
	loop->assertInLoopThread();
	Timestamp now(Timestamp::now());
	readTimerfd(timerfd, now);

	std::vector<Entry> expired = getExpired(now);
	for(Entry& it : expired) {
		it.second->run();
	}

	reset(expired, now);
}


std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
	std::vector<Entry> expired;
	Entry sentry = std::make_pair(now, std::shared_ptr<Timer>() );
	auto it = timers.lower_bound(sentry);
	assert(it == timers.end() || now < it->first);
	std::copy(timers.begin(), it ,std::back_inserter(expired));
	timers.erase(timers.begin(), it);
	return expired;
}

void TimerQueue::reset(std::vector<TimerQueue::Entry>& expired, Timestamp now) {
	Timestamp newExpire;

	for(auto& it : expired){
		if(it.second->repeat()) {
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


bool TimerQueue::insert(std::shared_ptr<Timer>&& ptr) {
	bool flag = false;
	Timestamp when = ptr->expiration();
	if(timers.empty() || when < timers.begin()->first) {
		flag = true;
	}
	auto result = timers.insert({when, ptr});
	return flag;
}



