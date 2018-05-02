#ifndef _TIMER_ID_H
#define _TIMER_ID_H

namespace netlib{

class Timer;

class TimerId {
public:
	explicit TimerId(Timer* timer = NULL, int64_t seq = 0) : value_(timer), seq_(seq) {
		
	}
	friend class TimerQueue;
private:
	Timer* value_;
	int64_t seq_;
};

}

#endif
