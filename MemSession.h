#ifndef _SESSION_H
#define _SESSION_H

#include "TcpConnection.h"
#include "nocopyable.h"
#include "Buffer.h"
#include "Memcached.h"

#include <string>
#include <functional>
#include <memory>

namespace netlib{

class Session : public nocopyable {
public:
	Session(Cache<std::string,std::string> *pool_, const TcpConnectionPtr& conn): 
		cache(pool_),
		connection(conn){
		MessageCallback cb = std::bind(&Session::onMessage, this, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3);
		connection->setMessageCallback(cb);
	}	

private:
	enum State {
		kNewCommand,
		kGetKey,
		kSetKey,
		kSetValue,
	};

	void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time);
	

	/*void setState(State _state) {
		state = _state;
	}
	*/
	Cache<std::string,std::string>* cache;
	TcpConnectionPtr connection;
	

};

}





#endif
