#include"../Memcached.h"
#include"../InetAddress.h"
#include"../MemSession.h"
#include"../TcpConnection.h"
//#include"../TcpServer.h"
#include"../EventLoop.h"
#include"../TcpServer.h"
#include <mutex>
#include <thread>
#include <memory>
#include <functional>
#include <unordered_map>
#include <iostream>

using namespace netlib;

typedef std::shared_ptr<Session> SessionPtr;

class MemServer{
public:
	MemServer(EventLoop* loop_):loop(loop_),
			server(loop_, InetAddress(9981)), 
			sessions_(),
			mtx(),
			cache(100){
		std::function<void(const TcpConnectionPtr&)> f = std::bind(&MemServer::onConnection, this, std::placeholders::_1);
		server.setConnectionCallback(f);
	}

	void start(){
		server.start();
	}

	void stop(){
		loop->runAfter(3.0, std::bind(&EventLoop::quitloop, loop));
	}


private:
	void onConnection(const TcpConnectionPtr& conn){//, Buffer* buf, timestamp time){
		if(conn->connected()) {	
			SessionPtr session(new Session(&cache, conn));
			std::lock_guard<std::mutex> lk(mtx);
			sessions_[conn->name()] = session;
		}
		else {
			std::lock_guard<std::mutex> lk(mtx);
			sessions_.erase(conn->name());
		}
		
	}


	EventLoop* loop; 
	TcpServer server;
	std::unordered_map<std::string, SessionPtr> sessions_;
	Cache<string,string> cache;
	std::mutex mtx;
};

int main(){
	EventLoop loop;
	MemServer ms(&loop);

	ms.start();
	loop.loop();

}
