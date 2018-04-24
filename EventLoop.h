#pragma once
#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H
#include "Timestamp.h"
#include "nocopyable.h"
#include <assert.h>
#include "TimerId.h"
//#include "Poller.h"
//#include "Channel.h"
#include "ThreadFunc.h"
//#include <pthread.h>
#include <memory>
#include <functional>
#include <vector>

namespace netlib {

	typedef std::function<void()> TimerCallback;
	
	class Poller;

	class Channel;
	
	class TimerQueue;

	class EventLoop : nocopyable {
	public:
		EventLoop();
		~EventLoop();

		void loop();
		void quitloop();

		void updateChannel(Channel* channel);

		void assertInLoopThread() {
			assert(isInLoopThread());
		}

		bool isInLoopThread() {
			return _tid == curThreadId();
		}

		TimerId runAt(const Timestamp&, const TimerCallback&);
		TimerId runAfter(double, const TimerCallback&);
		TimerId runEvery(double, const TimerCallback&);

	private:
		typedef std::vector<Channel*> ChannelVec;
		std::unique_ptr<Poller> poller;
		std::unique_ptr<TimerQueue> timerQueue;
		Timestamp pollReturnTime;
		ChannelVec activeChannels; 
		ThreadId _tid;
		bool looping;
		bool quit;

	};

}




#endif
