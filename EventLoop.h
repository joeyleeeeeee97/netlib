#pragma once
#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H
#include "nocopyable.h"
#include <assert.h>
#include "ThreadFunc.h"
#include <pthread.h>
namespace netlib {
	class EventLoop : nocopyable {
	public:
		EventLoop();
		~EventLoop();

		void loop();

		void assertInLoopThread() {
			assert(isInLoopThread());
		}

		bool isInLoopThread() {
			return _tid == curThreadId();
		}
	private:
		ThreadId _tid;
		bool looping;

	};
}




#endif