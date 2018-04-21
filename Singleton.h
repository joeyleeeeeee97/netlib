#pragma once
#ifndef SINGLETON_H
#define SINGLETON_H
/*
使用static 变量来实现单利模式(c++ 11 之后是线程安全的）
"If multiple threads attempt to initialize the same static local variable concurrently, 
the initialization occurs exactly once (similar behavior can be obtained for arbitrary functions with std::call_once). 
Note: usual implementations of this feature use variants of the double-checked locking pattern, 
which reduces runtime overhead for already-initialized local statics to a single non-atomic boolean comparison."
*/
template<typename T>
class Singleton {
public:
	static T& getSingleton() {
		static T data;
		return data;
	}
	//如果需要传入参数来使用
	template<typename... Args>
	static T& getSingleton(Args&&... args) {
		static T data(args...);
		return data;
	}
};



#endif