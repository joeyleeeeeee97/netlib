#include"../Memcached.h"
#include"../InetAddress.h"
#include"../TcpConnection.h"
#include"../EventLoop.h"
#include"../TcpServer.h"
#include <mutex>
#include <thread>
#include <memory>
#include <functional>
#include <unordered_map>
#include <iostream>

using namespace netlib;


class ChatServer {

public:
	typedef std::unordered_map<std::string, TcpConnectionPtr> ConnectionMap;
	typedef std::shared_ptr<ConnectionMap> MapPtr;

	ChatServer(EventLoop* loop):loop_(loop), server_(loop, InetAddress(9981)),mtx_(), conns_(new ConnectionMap()){
		std::function<void(const TcpConnectionPtr&)> f = std::bind(&ChatServer::onConnection, this, std::placeholders::_1);
		server_.setConnectionCallback(f);

	}
	
	void start(){
		server_.start();
	}

	void stop(){
		loop_->runAfter(3.0, std::bind(&EventLoop::quitloop, loop_));
	}


private:

	void onConnection(const TcpConnectionPtr& conn){
		if(conn->connected()){
			std::cout<<conn->name() << " : " << "UP" <<std::endl;
			MessageCallback cb = std::bind(&ChatServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			conn->setMessageCallback(cb);	
	
			std::lock_guard<std::mutex> lk(mtx_);
			(*conns_)[conn->name()] = conn;
		}
		else {
			std::cout<<conn->name() << " : " << "DOWN" <<std::endl;
			std::lock_guard<std::mutex> lk(mtx_);
			conns_->erase(conn->name());
		}

	}

	void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp now) {
		std::cout<<"receiced mes from " << conn->name() << std::endl;
		MapPtr conns_tmp;		

		{
			std::lock_guard<std::mutex> lk(mtx_);
			conns_tmp = std::make_shared<ConnectionMap>(*conns_);
		}	
		
		for(auto it = conns_->begin(); it != conns_->end(); it++) {
			it->second->send(buf->peek(), buf->readableBytes());
		}
		buf->retrieveAll();
		std::cout<<"send successfully"<<std::endl;
	}
	

	EventLoop* loop_;
	TcpServer server_;
	std::mutex mtx_;
	MapPtr conns_;
	

};

int main(){
        EventLoop loop;
        ChatServer ms(&loop);

        ms.start();
        loop.loop();
	return 0;
}
