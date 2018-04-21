#pragma once
#ifndef THREADFUNC_H
#define THREADFUNC_H

#include "nocopyable.h"
#include<functional>
#include<thread>
namespace netlib {
	using ThreadId = std::thread::id;
	
	ThreadId curThreadId() {
		thread_local static ThreadId cachedId = std::this_thread::get_id();
		return cachedId;
	}


	class ThreadFunc : nocopyable {
	public:
		ThreadFunc() :t(), func(),started(false),joined(false) {
		}
		ThreadFunc(ThreadFunc&& other) noexcept {
			func = std::move(other.func), t = std::move(other.t);
			started = other.started, joined = other.joined;
		}
		template< class Function, class... Args >
		explicit ThreadFunc(Function&& f, Args&&... args) : ThreadFunc() {
			func = std::bind(f, args...);
		}

		ThreadFunc& operator=(ThreadFunc&& rhs) = default;

		~ThreadFunc() {
			if (started && !joined) {
				t.detach();
			}
		}

		void start() {
			t = std::move(std::thread(func));
			started = true;
		}

		void join() {
			t.join();
			joined = true;
		}

		bool joinable() {
			if (started && !joined) {
				return true;
			}
			return false;
		}
	private:
		std::function<void()> func;
		bool started;
		bool joined;
		std::thread t;
	};
}



#endif