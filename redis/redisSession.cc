#include<memory>
#include<unordered_map>
#include<string>
#include<functional>
#include<queue>


#include"dataBase.h"
#include"redis.h"
#include"dict.h"

redisSession::redisSession(const std::shared_ptr<redisServer>& s, const TcpConnection& p):state_(EXEC),mtx_(), cond_(), conn_(p),server_(s),func_queue(){}

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
		//conn->send(res.toStringPiece());	
	}
	server_->queue(f);	
}

void redisSession::queue(const std::function<void()>& f){
	if(state_ != QUEUE) {
		server_->queue(f);
	}
	else {
		func_queue.push_back(f);
	}
}


void redisSession::wakeup(){
	cond.notify_one();
}

void redisSession::wait(){
	std::unique_lock<std::mutex> lk(mtx_);
	cond.wait(lk);
}


Command_ redisSession::parse(const std::string& str) {
	switch(str){
	case "GET":
		return GET;
	case "SET":
		return SET;
	case "PUSH":
		return PUSH;
	case "POP":
		return POP;
	case "BPOP":
		return BPOP;

	default:
		return DATA;
	}

}


bool redisSession::checkCommand(Command_ c) {
	if(state_ == DIRTY_STATE) {
		return false;
	}
	else if(state_ == EXEC && c == MULTI) {
		state_ = QUEUE;
	}
	else if(state_ = QUEUE && c == EXEC) {

	}
	return true;
}



void redisSession::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time){
	while(buf->readableBytes() > 0) {
		const char* crlf = buf->findCRLF(buf->peek());
		if(!crlf) {
			conn->shutdown();
			break;
		}
		int len = static_cast<int>(crlf - buf->peek());
		std::string line(buf->peek(),len);
		buf->retrieveUntill(crlf+2);
		std::istringstream ss(line);
		std::vector<std::string> vals;

		std::copy(istream_iterator<std::string>(ss),
			  istream_iterator<std::string(),
			  std::back_inserter(vals));
	
		for(auto it = vals.begin(); it != vals.end(); it++) {
			Command_ c = parse(*it);
			if( !checkCommand(c)) { continue; }
			if(c == GET) {
				if(it + 1 == vals.end(){ continue; }
				auto f = [&](){
					dataBase key = StringPiece::makeStringPiece((++it)->c_str());
					auto res = server_->get(key);
					if(res.first) {
						 auto data = static_cast<Data*>(p3.second.data());
						 conn->send(data->val.data());
					}
					else {
						conn->send("NOT FOUND!\n");
					}
				};
				queue(f);
			}
			else if(c == SET) {
				if(it + 1 == vals.end()){ continue; }
				auto f = [&](){
					auto ent = dictEntry::makeDictEntry(it->c_str(),(++it)->c_str());
					server_->put(ent);
				};
				queue(f);
			}
			else if(c == PUSH) {
                                if(it + 1 == vals.end()){ continue; }
                                auto f = [&](){
					auto data = StringPiece::makeStringPiece((++it)->c_str());
					server_->push(data);
				};
				queue(f);
                        }
			else if(c == POP) {
				auto f = [&](){
					auto res = server_->pop();
					const char* str = static_cast<StringPiece*>(res.data())->data();
					conn->send(str);
				};
				queue(f);
			}
			else if(c == BPOP) {
				auto f= [&](){
					auto res = server_->pop();
                                        if(res.datatype != dataBase::dataType::NOTYPE) {
						const char* str = static_cast<StringPiece*>(res.data())->data();
                                        	conn->send(str);
						wakeup();
					}
				};
				queue(f);
				wait();
			}
			else if(c == MULTI) {}
			else if(c == EXEC) {
				auto f = std::bind(&redisSession::exec, this);
				queue(f);
			}
			else if(c == WATCH) {
				if(it + 1 == vals.end()){ continue; }
				auto data = StringPiece::makeStringPiece((++it)->c_str());
				std::weak_ptr<redisSession> wp = shared_from_this();
				auto f = [this, wp, &data](){
					server_->watch(data, wp);
				};
			}
		}
		
	}	
	conn->shutdown();

}

