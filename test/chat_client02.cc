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
        };




	void start(){
                server_.start();
		to_server_->connect();
       		
		std::function<void()> f = std::bind(&ChatClient::sendToPeer, this, "joey", "hello! Are you ok!?");
			
		loop_->runAfter(4.0, f);	
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
	EventLoop loop;
	ChatClient client(&loop, "lee", 9983);

	client.start();
	loop.loop();
	
	
	return 0;	
}


