#pragma once
#ifndef _CIRCLEBUFFER_H
#define _CIRCLEBUFFER_H

#include <deque>

namespace netlib {

	template<typename T>
	class CircleBuffer {
	public:

		CircleBuffer(size_t n) :data(), num(0) {}
		~CircleBuffer() = default;

		void push(const T& rhs) {
			if (num == n) {
				data.pop_front();
			}
			else {
				num++;
			}
			data.push_back(rhs);
		}

		std::deque<T> data;
		size_t num;
	};

}



#endif
