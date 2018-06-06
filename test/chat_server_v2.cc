#include"../Memcached.h"
#include"../InetAddress.h"
#include"../TcpConnection.h"
#include"../EventLoop.h"
#include"../TcpServer.h"
#include <mutex>
#include <algorithm>
#include <sstream>
#include <thread>
#include <memory>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <vector>

using namespace netlib;

class ChatServer {
public:
	typedef std::unordered_map<std::string, std::string> AddrMap;
	typedef std::unordered_map<std::string, TcpConnectionPtr> ConnectionMap;

	ChatServer(EventLoop* loop):loop_(loop),server_(loop,InetAddress(9981)), addrs_(), conns_(), mtx_(){
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

	void onConnection(const TcpConnectionPtr& conn) {
                if(conn->connected()){
                        std::cout<<conn->name() << " : " << "UP" <<std::endl;
                        MessageCallback cb = std::bind(&ChatServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
                        conn->setMessageCallback(cb);

                        std::lock_guard<std::mutex> lk(mtx_);
                        conns_[conn->name()] = conn;
                }
                else {
                        std::cout<<conn->name() << " : " << "DOWN" <<std::endl;
                        std::lock_guard<std::mutex> lk(mtx_);
                        conns_.erase(conn->name());
                }
	

	}
	// 
	void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp now) {
		std::cout<<"receiced mes from " << conn->name() << buf->peekAsString()<< std::endl;

		while(buf->readableBytes() > 0 ) {
			const char* crlf = buf->findLINE(buf->peek());
			if(!crlf) {
	//			conn->shutdown();
				break;
			}
			int len = static_cast<int>(crlf - buf->peek());
	
	                std::string line(buf->peek(), len);//, command;
	                buf->retrieveUntill(crlf + 1);
	                std::istringstream ss(line);
	                std::vector<std::string> vals;
			
			for(auto& str : vals) {
				std::cout<<str<<std::endl;
			}
	
	
	                std::copy(istream_iterator<std::string>(ss),
        	                 istream_iterator<std::string>(),
        	                 std::back_inserter(vals));
			
			if(vals.empty()){
				continue;
			}			
			else if(vals[0] == "SET" && vals.size() == 3) {
				addrs_[vals[1]] = vals[2];
			} 
			else if(vals[0] == "GET" && vals.size() == 2) {
				auto res = addrs_.find(vals[1]);
				if( res  != addrs_.end() ) {
					conn->send(res->first + " " + res->second);
				}
				else {
					conn->send("NOT FOUND ");
				}
			}

		}	
	}

	EventLoop* loop_;
	TcpServer server_;

	AddrMap addrs_;
	ConnectionMap conns_;
	std::mutex mtx_;

};


int main(){
        EventLoop loop;
        ChatServer ms(&loop);

        ms.start();
        loop.loop();
        return 0;
}

