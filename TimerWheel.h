/*************************************************************************
	> File Name: TimerWheel.h
	> Author: 
	> Mail: 
	> Created Time: Sat 07 Jul 2018 07:08:22 PM DST
 ************************************************************************/

#ifndef _TIMERWHEEL_H
#define _TIMERWHEEL_H

#include"nocopyable.h"
#include"EventLoop.h"
#include"TimerQueue.h"
#include"TcpConnection.h"
#include<deque>
#include<vector>

namespace netlib {

    class TimerWheel : nocopyable{
        public:
            TimerWheel(EventLoop* loop, double interval,  int length);

            ~TimerWheel();

            void add(const TcpConnectionPtr&);
            void  start();

        private:
            typedef  std::vector<TcpConnectionPtr> ConnectionSet;
            void timerfunc();

            double interval_;
            int length_;
            EventLoop* loop_;
            std::deque<ConnectionSet> queue_;
    };

}


#endif
