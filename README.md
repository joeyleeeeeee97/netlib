# netlib
> 基于reactor + one thread per loop 模型的网络库

## 网络库基本并发组件
自己实现了部分服务器开发常用的网络组件，包装使用了部分c++11的多线程组件。
> * ThreadFunc 模型
>> 是对于线程中运行的单个函数的抽象而非线程的抽象
>
> * ThreadPool 线程池
>> std::future 配合 functional 实现线程池
> 
> * lock_guard  And conditional_variable 
>> 使用守护锁便于管理，利用条件变量异步通知
>
> * WaitGroup 
>> 仿造Go语言的WaitGroup ,进行并发控制，功能相当于操作系统中的Barrier
>
> * Timestamp 
>> 时间戳，封装一些时间计算
>
> * CircleBuffer
>> 用来实现timing wheel 踢掉空闲连接
>
> * Singleton.h
>> 线程安全的单例模式

## 网络库核心组件
> * EventLoop.h
>> 11
>
> * Poller.h / Epoller.h
>> 22
>

> * Channel.h
>> 33
>
