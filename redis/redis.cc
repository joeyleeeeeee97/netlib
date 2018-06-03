#include"../InetAddress.h"
#include"redisSession.h"
#include"../TcpConnection.h"
#include"dict.h"
#include"redisSession.h"
#include"../TcpServer.h"
#include"../EventLoop.h"
#include"../TcpServer.h"
#include <memory>
#include <functional>
#include <unordered_map>
#include <list>
#include <mutex>

using namespace netlib;


redisServer::redisServer(EventLoop* l) : loop_(l), server(loop_, InetAddress(9981)),
					 dict(), mtx_(),list_(), sessions_(), blocking_keys(), watched_keys(),ready_list(){
	std::function<void(const TcpConnectionPtr&)> f = std::bind(&redisServer::onConnection,this, std::placeholders::_1);
	server.setConnectionCallback(f);
}


void redisServer::start() {
	server.start();
}

void redisServer::stop(){
	loop_->runAfter(3.0, std::bind(&EventLoop::quitLoop, loop));
}

void redisServer::onConnection(const TcpConnectionPtr& conn) {
	if(conn->connected()) {
		SessionPtr session(new redisSession(std::shared_from_this(), conn);
		std::lock_guard<std::mutex> lk(mtx_);
		sessions_[conn->name()] = session;
	}
	else {
		std::lock_guard<std::mutex> lk(mtx_);
		sessions_.erase(conn->name());
	}
}



std::pair<bool,dataBase>  redisServer::get(const dataBase& db) {
	return dict_.get(db);
}

bool redisServer::put(const dataBase& db) {
	return dict_.insert(db);
}


void push(const dataBase& db){
	list_.push_back(db);
}

dataBase redisServer::pop(){
	if(list_.empty()) {
		return dataBase();
	}
	dataBase ret = list_.front();
	list_.pop_front();
	return ret;
}

dataBase redisServer::bpop(){
	//Session 会检查dataBase 的返回类型然后挂起阻塞
	return pop();
}

void redisServer::queue(const Func& f) {
	loop_->queueInLoop(f);
}

void redisServer::watch(const dataBase& key,const weakSessionPtr& wptr){
	blocking_keys[key].push_back(wptr);
}

void redisServer::handleBlockingKeys() {
	for(auto& it : ready_list) {
		weakSessionList& l = blocking_keys[it];
		if(l.emtpy()){
			continue;
		} 
		else {
			if(auto ptr = l.front().lock()){
				ptr->wakeup();
			}
			l.pop_front();
		}	
	}
}














