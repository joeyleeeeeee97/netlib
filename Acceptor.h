#ifndef _ACCPTOR_H
#define _ACCPTOR_H
#include "nocopyable.h"
#include "Channel.h"
#include "Socket.h"
#include <functional>

namespace netlib{

class EventLoop;
class InetAddress;

class Acceptor : nocopyable {
public:
	typedef std::function<void(int sockfd, const InetAddress&)> NewConnectionCallback;

	Acceptor(EventLoop* _loop, const InetAddress& _listenAddr);

	void setNewConnectionCallback(const NewConnectionCallback& cb){
		newConnectionCallback = cb;
	}

	bool listenning() const {
		return listenning_;
	}

	void listen();

private:
	
	void handleRead();

	EventLoop* loop;
	Socket acceptSocket;
	Channel acceptChannel;
	NewConnectionCallback newConnectionCallback;
	bool listenning_;

};


}




#endif
