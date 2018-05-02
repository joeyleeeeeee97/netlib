#ifndef _TIMERQUEUE_H
#define _TIMERQUEUE_H
#include <set>
#include <vector>
#include <memory>
#include <mutex>
#include <functional>

#include "Timestamp.h"
#include "Channel.h"
#include "nocopyable.h"

namespace netlib{

typedef std::function<void()> TimerCallback;

class EventLoop;
//class Channel;
class TimerId;
class Timer;

class TimerQueue : nocopyable {
public:
	TimerQueue(EventLoop* loop);
	~TimerQueue();

	TimerId addTimer(const TimerCallback& cb, Timestamp when, double Interval);
	void cancel(TimerId);
private:
	typedef std::pair<Timestamp, std::shared_ptr<Timer>> Entry;
	typedef std::set<Entry> TimerList;
	typedef std::pair<std::shared_ptr<Timer>, int64_t> ActiveTimer;
	typedef std::set<ActiveTimer> ActiveTimerSet;


	void addTimerInLoop(Timer* timer);
	void cancelInLoop(TimerId);	

	void handleRead(Timestamp);
	
	std::vector<Entry> getExpired(Timestamp now);
	void reset(std::vector<Entry>& expired, Timestamp now);

	bool insert(const std::shared_ptr<Timer>& timer);
	
	EventLoop* loop;
	const int timerfd;
	Channel timerfdChannel;
	TimerList timers;

	bool callingExpiredTimers_;
	ActiveTimerSet activeTimers_;
	ActiveTimerSet cancelTimers_;

};


}



#endif
