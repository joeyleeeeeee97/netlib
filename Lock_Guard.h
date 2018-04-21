#pragma once
#ifndef LOCK_GUARD_H
#define LOCK_GURAD_H

#include"nocopyable.h"
#include<mutex>
namespace netlib{
	struct defer_lock_t { explicit defer_lock_t() = default; };
	struct try_to_lock_t { explicit try_to_lock_t() = default; };
	struct adopt_lock_t { explicit adopt_lock_t() = default; };

	template<typename mutex_type>
	class lock_guard : nocopyable {
	public:
		explicit lock_guard(mutex_type& m): mtx(m) {
			mtx.lock();
		}
		lock_guard(mutex_type& m, netlib::adopt_lock_t t): mtx(m) {
		}
		~lock_guard() {
			mtx.unlock();
		}
	private:
		mutex_type& mtx;
	};

	template<typename mutex_type>
	class unique_lock {
	public:
		typedef mutex_type* mtx_ptr;

		unique_lock() noexcept: mtx(nullptr), owned(false) {

		}
		unique_lock(unique_lock&& other) noexcept {
			mtx = other.mtx, owned = other.owned;
			other.mtx = nullptr, other.owned = false;
		}
		explicit unique_lock(mutex_type& m): mtx(&m), owned(true) {
			mtx->lock();
		}
		unique_lock(mutex_type& m, defer_lock_t t) noexcept : mtx(&m), owned(false) {

		}
		unique_lock(mutex_type& m, try_to_lock_t t) :mtx(&m), owned(false) {
			owned = mtx->try_lock();
		}
		unique_lock(mutex_type& m, adopt_lock_t t) : mtx(&m), owned(true) {

		}


		~unique_lock() {
			if (owned) {
				mtx->unlock();
				owned = false;
			}
		}

		void lock() {
			mtx->lock();
		}

		void unlock() {
			mtx->unlock();
		}

		void release() {
			mtx = nullptr, owned = false;
		}

	private:
		bool owned;
		mtx_ptr mtx;
	};

}






#endif