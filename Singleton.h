#pragma once
#ifndef SINGLETON_H
#define SINGLETON_H
/*
ʹ��static ������ʵ�ֵ���ģʽ(c++ 11 ֮�����̰߳�ȫ�ģ�
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
	//�����Ҫ���������ʹ��
	template<typename... Args>
	static T& getSingleton(Args&&... args) {
		static T data(args...);
		return data;
	}
};



#endif