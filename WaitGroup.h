#pragma once
#ifndef WAITGROUP_H
#define WAITGROUP_H
#include<mutex>
#include<condition_variable>
#include"nocopyable.h"
namespace netlib {
	class WaitGroup : nocopyable {
	public:
		WaitGroup() :mtx(), waitCount(0), cond() {}
		WaitGroup(size_t num) :mtx(), waitCount(num), cond() {}

		void add(size_t num) {
			std::lock_guard<std::mutex> lk(mtx);
			waitCount += num;
		}
		void done() {
			std::lock_guard<std::mutex> lk(mtx);
			waitCount--;
			cond.notify_all();
		}
		void wait() {
			std::unique_lock<std::mutex> lk(mtx);
			cond.wait(lk, [this]() {return this->waitCount == 0; });
		}
	private:
		size_t waitCount;
		std::mutex mtx;
		std::condition_variable cond;
	};

}
#endif