#pragma once
#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H
#include "nocopyable.h"
#include <assert.h>
#include "ThreadFunc.h"
#include <pthread.h>
#include <memory>

namespace netlib {

	class Poller;
	class Channel;

	class EventLoop : nocopyable {
	public:
		EventLoop();
		~EventLoop();

		void loop();
		void quit();

		void updateChannel(Channel* channel);

		void assertInLoopThread() {
			assert(isInLoopThread());
		}

		bool isInLoopThread() {
			return _tid == curThreadId();
		}
	private:
		typedef std::vector<Channel*> ChannelVec;
		std::unique_ptr<Poller> poller;
		ChannelVec channels; 
		ThreadId _tid;
		bool looping;
		bool quit;

	};
}




#endif
