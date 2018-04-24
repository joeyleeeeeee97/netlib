#ifndef _TIMER_ID_H
#define _TIMER_ID_H

namespace netlib{

class Timer;

class TimerId {
public:
	explicit TimerId(Timer* timer) : value_(timer) {
		
	}
private:
	Timer* value_;

};

}

#endif
