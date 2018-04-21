#pragma once
#ifndef NOCOPYABLE_H
#define NOCOPYABLE_H
namespace netlib {
	struct nocopyable {
		nocopyable() {}
		nocopyable(const nocopyable&) = delete;
		nocopyable& operator=(const nocopyable&) = delete;
		nocopyable(nocopyable&&) = default;
		nocopyable& operator=(nocopyable&&) = default;
	};




}
#endif