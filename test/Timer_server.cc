#include"../InetAddress.h"
#include"../TcpConnection.h"
#include"../EventLoop.h"
#include"../TcpServer.h"
#include"../TimerWheel.h"
#include <mutex>
#include <thread>
#include <memory>
#include <functional>
#include <unordered_map>
#include <iostream>

using namespace netlib;

class TimerServer{
public:
    TimerServer(EventLoop* loop):loop_(loop), server_(loop, InetAddress(9981)), timerwheel_(loop, 1.0, 5) {
        std::function<void(const TcpConnectionPtr&)> f = std::bind(&TimerServer::onConnection, this, std::placeholders::_1);
        server_.setConnectionCallback(f);
    }
    
    void start() {
        timerwheel_.start();
        server_.start();
    }

private:
    void onConnection(const TcpConnectionPtr& conn) {
        if(conn->connected()) {
            std::cout<<conn->name()<<"UP"<<std::endl;
            timerwheel_.add(conn);
        }
        else {
            std::cout<<conn->name()<<"DOWN";
        }
    }
    
    EventLoop* loop_;
    TcpServer server_;
    TimerWheel timerwheel_;

};

int main() {
    EventLoop loop;
    TimerServer ts(&loop);

    ts.start();
    loop.loop();
}
