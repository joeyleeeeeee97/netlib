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
#include <mutex>

namespace netlib {

	typedef std::function<void()> TimerCallback;
	typedef std::function<void()> Functor;	

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
		void removeChannel(Channel* channel);

		void assertInLoopThread() {
			assert(isInLoopThread());
		}

		Timestamp pollReturntime() const {
			return pollReturnTime;
		}
	
		void runInLoop(const Functor& f);
		void queueInLoop(const Functor& f);
		

		bool isInLoopThread() {
			return _tid == curThreadId();
		}

		TimerId runAt(const Timestamp&, const TimerCallback&);
		TimerId runAfter(double, const TimerCallback&);
		TimerId runEvery(double, const TimerCallback&);

		void wakeup();

	private:
		typedef std::vector<Channel*> ChannelVec;
		
		void handleRead(Timestamp);
		void doPendingFunctors();
		
		
		bool looping;
		bool quit;
		bool callPendingFunctors;
		ThreadId _tid;
		Timestamp pollReturnTime;
		std::unique_ptr<Poller> poller;
		std::unique_ptr<TimerQueue> timerQueue;
		int wakeupFd;
		std::unique_ptr<Channel> wakeupChannel;
		ChannelVec activeChannels;
		std::mutex lk;
		std::vector<Functor> pendingFunctors;

	};

}




#endif
