#include<memory>
#include<unordered_map>
#include<string>
#include<functional>
#include<queue>


#include"dataBase.h"
#include"redis.h"
#include"dict.h"

redisSession::redisSession(const std::shared_ptr<redisServer>& s, const TcpConnection& p):state_(EXEC),mtx_(), cond_(), conn_(p),server_(s),func_queue(),pending_func(){}

redisSession::~redisSession(){}

void redisSession::multi(){
	state_ = QUEUE;
}


void redisSession::exec(){
	state_ = EXEC;
	std::function<void()> f = [=func_queue](){
		for(auto& func : func_queue){
			func();
		}		

	}
	func_queue.clear();
	server_->queue(f);
}

void redisSession::discard() {
	state_ = EXEC;
	func_queue.clear();

}


void redisSession::watch(const dataBase& db){
	server_->watch(db);
}

void queueWithRes(const std:function<void()>& f){
	auto f = [&conn_, &f](){
		dataBase res = f();
		conn->send(res.toStringPiece());	
	}
	server_->queue(f);	
}

void redisSession::queue(const std::function<void()>& f){
	server_->queue(f);
}


void redisSession::wakeup(){
	cond.notify_one();
}

void redisSession::wait(){
	std::unique_lock<std::mutex> lk(mtx_);
	cond.wait(lk);
}

void redisSession::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time){
	//TODO
	
}

