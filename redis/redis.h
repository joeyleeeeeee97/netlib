#ifndef _REDIS_H
#define _REDIS_H

#include"../InetAddress.h"
#include"redisSession.h"
#include"../TcpConnection.h"
#include"../TcpServer.h"
#include"../EventLoop.h"
#include"../TcpServer.h"
#include <mutex>
#include <thread>
#include <memory>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <list>

namespace netlib {

typedef std::shared_ptr<redisSession> SessionPtr;
typedef std::weak_ptr<redisSession> weakSessionPtr;

class dict;

//single thread
class redisServer : std::enable_shared_from_this<redisServer> {

public:
	redisServer(EventLoop*);


	void start();
	void stop();

	//commit transaction
	std::pair<bool,dataBase> get(const dataBase&);
	bool put(const dataBase&);
	
	//List
	void push(const dataBase&);
	dataBase pop();
	//Asyn
	dataBase bpop()	
	
	void watch(const dataBase&, const weakSessionPtr&);
	
	void handleBlokingKeys();

	void queue(const Func&);	

private:
	typedef std::list<SessionPtr> SessionList;
	typedef std::list<dataBase> DataList;
	typedef std::fuction<void()> Func;
	typedef std::list<weakSessionPtr> weakSessionList;


	enum State_{
		//事务
		Normal,
	};

	void onConnection(const TcpConnectionPtr&);
	
	EventLoop* loop_;
	TcpServer server_;
	dict dict_;
	std::mutex mtx_;
	std::list<dataBase> list_;

	std::unordered_map<std::string, SessionPtr> sessions_;
	std::unordered_map<dataBase,weakSessionList,hash_value> bloking_keys;
	std::unordered_map<dataBase,weakSessionList,hash_value> watched_keys;
	DataList ready_list;	
};




}

#endif
