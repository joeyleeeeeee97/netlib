#include"TimerWheel.h"
#include"TcpConnection.h"
#include"EventLoop.h"
#include<deque>
#include<vector>

using namespace netlib;

TimerWheel::TimerWheel(EventLoop* loop, double interval, int len):interval_(interval),length_(len),loop_(loop),queue_(){
    queue_.push_front({});
}

TimerWheel::~TimerWheel(){}

void TimerWheel::start() {
    loop_->runEvery(interval_,[this](){ timerfunc(); });
}


void TimerWheel::timerfunc() {
    queue_.push_front({});
    if(queue_.size() > length_) {
        auto& s = queue_.back();
        for(auto& conn : s) {
            conn->shutdown();
        }
        queue_.pop_back();
    }
}

void TimerWheel::add(const TcpConnectionPtr& p) {
    auto& s = queue_.front();
    s.push_back(p);
}




