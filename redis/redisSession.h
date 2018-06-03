#ifndef _REDIS_SESSION_H
#define _REDIS_SESSION_H

#include"../TcpConnection.h"
#include"../nocopyable.h"
#include "../Buffer.h"
#include "../Memcached.h"

#include<mutex>
#include<condition_variable>
#include<string>
#include<functional>
#include<memory>
#include<unordered_map>
#include<queue>


class netlib{

class dict;
class dataBase;
class redisServer;

class redisSession : public nocopyable , public std::enable_shared_from_this<redisSession>{

public:
	enum State_ {
		QUEUE,
		EXEC,
		DIRTY_STATE,
	};

	enum Command_{
		GET,
		SET,
		PUSH,
		POP,
		BPOP,
		MULTI,
		EXEC,
		WATCH,
		DISCARD,
		DATA,
	};

	redisSession(const std::shared_ptr<redisServer>&,const TcpConnection&);
	~redisSession();

	void multi();
	void exec();
	void discard();
	void watch(const dataBase&);

	void queueWithRes(const std::function<void()>&);
	void queue(const std::function<void()>&);

	void wakeup();
	void wait();

	Command_ parse(const std::string&);
	bool checkCommand(Command_);	
	void comDispatcher(Command_);

	void setState(State_ s) {
		state_ = s;
	}

private:

	void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time);
	
	State_ state_;
	std::mutex mtx_;
	std::condition_variable cond_;	

	TcpConnectionPtr conn_;
	std::shared_ptr<redisServer> server_;

	std::queue<std::function<void()>> func_queue;
};


};



#endif
