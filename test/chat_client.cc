#include"../EventLoop.h"
#include"../TcpClient.h"
#include"../Buffer.h"
#include"../TcpConnection.h"
#include"../WaitGroup.h"
#include"../InetAddress.h"
#include"../EventLoop.h"
#include"../TcpServer.h"
#include<sstream>
#include<iostream>
#include<functional>
#include<memory>
#include<string>
#include<unordered_map>
#include<vector>

using namespace netlib;


class ChatClient{
public:
	typedef std::unordered_map<std::string, TcpConnectionPtr> UserMap;
	
	ChatClient(EventLoop* loop, const std::string& username, int port):port_(port), loop_(loop), server_(loop_,InetAddress(port)), username_(username), lk_map_(), cond_map_(), wait_(), conn_server_(), to_server_(new TcpClient(loop, InetAddress(9981))), conns_(), addrs_(), peers_(), threads_(){
		auto onc = [this](const TcpConnectionPtr& conn){
        		std::cout<<"CONNECTED!"<<std::endl;
	
	                conn_server_ = conn;
                        std::stringstream ss;
                        ss<<"SET "<<username_<<" "<<port_ <<"\n";
                        conn->send(ss.str());
                };
                MessageCallback onm = [this](const TcpConnectionPtr& conn, Buffer* buf, Timestamp time){
                 	std::cout<<"RECEIVED MSG FROM "  << conn->name() << std::endl;
		       while(buf->readableBytes() > 0) {
                                std::stringstream ss;
                                std::cout<<buf->peekAsString()<<std::endl;
				ss << buf->retrieveAsString();
                                std::string key, val;
                                ss >> key >> val;
                                addrs_[key] = val;
                                connectToPeer(key, val);
                        }
                };

                to_server_->setConnectionCallback(onc);
                to_server_->setMessageCallback(onm);
        
		std::function<void(const TcpConnectionPtr&)> f = std::bind(&ChatClient::onConnection, this ,std::placeholders::_1);
		server_.setConnectionCallback(f);	
		

};




	void start(){
                server_.start();
		to_server_->connect();
       		
	}

        void stop(){
                loop_->runAfter(3.0, std::bind(&EventLoop::quitloop, loop_));
        } 


	void connectToPeer(const std::string& username, const std::string& addr) {
		std::stringstream ss(addr);
		int port;
		ss >> port;
		std::shared_ptr<TcpClient> client = std::make_shared<TcpClient>(loop_, InetAddress(port));
                auto onc = [this,username, addr](const TcpConnectionPtr& conn){
			conns_[username] = conn;
			auto cond = cond_map_[username];
			cond->notify_one(); 
	        };

                MessageCallback onm = [this](const TcpConnectionPtr& conn, Buffer* buf, Timestamp time){
                	std::cout<<buf->retrieveAsString()<<std::endl;
		};
		client->setConnectionCallback(onc);
		client->setMessageCallback(onm);		
		peers_.push_back(client);
		peers_.back()->connect();
		
	}
	
	void sendToPeer(const std::string& peername, const std::string& content){
		auto iter = conns_.find(peername);
		if(iter != conns_.end()) {
			iter->second->send(content);
		}
		else {
			std::cout<<peername + " NOT EXIST IN CURRENT USERTABLE"<<std::endl;
			
			cond_map_.insert({peername,std::make_shared<std::condition_variable>()});
			lk_map_.insert({peername,std::make_shared<std::mutex>()});			
			std::cout<<"OK1"<<std::endl;
	
			std::shared_ptr<ThreadFunc> tf = std::make_shared<ThreadFunc>([this,content, peername](){
				std::stringstream ss;
				ss<<"GET "<<peername<<"\n";
				conn_server_->send(ss.str());	
				std::cout<<"OK3"<<std::endl;
				auto& cond = cond_map_[peername];
				auto& mtx = lk_map_[peername];
				std::cout<<"OK4"<<std::endl;
				std::unique_lock<std::mutex> lk(*mtx);
				cond->wait(lk);
				conns_[peername]->send(content);
			});

			threads_.push_back(tf);
			threads_.back()->start();
			
			std::cout<<"OK2"<<std::endl;
		}
	};	

	void boardcastToPeer(const std::vector<std::string>& list) {

	};
		


private:
	void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time) {
		std::cout<<buf->retrieveAsString()<<std::endl;
		
	}

	void onConnection(const TcpConnectionPtr& conn) {
		 if(conn->connected()){
                        std::cout<<conn->name() << " : " << "UP" <<std::endl;
                        MessageCallback cb = std::bind(&ChatClient::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
                        conn->setMessageCallback(cb);
                }
                else {
                        std::cout<<conn->name() << " : " << "DOWN" <<std::endl;
                }


	}
	int port_;
	EventLoop* loop_;
	TcpServer server_;
	
	std::string username_;
	std::unordered_map<std::string, std::shared_ptr<std::mutex>> lk_map_;
	std::unordered_map<std::string, std::shared_ptr<std::condition_variable>> cond_map_;
	WaitGroup wait_;

	std::shared_ptr<TcpClient> to_server_;
	TcpConnectionPtr conn_server_;
	
	UserMap conns_;
	std::unordered_map<std::string, std::string> addrs_;
	std::vector<std::shared_ptr<TcpClient>> peers_;
	std::vector<std::shared_ptr<ThreadFunc>> threads_;	
};


int main(){
        int port;

        std::string username;

        std::cout<<"请输入 用户名 和 端口" << std::endl;

        std::cin>>username>>port;

        EventLoop loop;
        ChatClient client(&loop, username, port);

        client.start();
        loop.loop();
        return 0;
}


