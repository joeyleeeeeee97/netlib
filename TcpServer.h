#pragma once
#ifndef _TCPSERVER_H
#define _TCPSERVER_H
#include "CallBacks.h"
#include "TcpConnection.h"
#include <map>
#include <memory>
#include "nocopyable.h"

namespace netlib{

class Acceptor;
class EventLoop;

class TcpServer : nocopyable{
public:
	TcpServer(EventLoop*,const InetAddress&);
	~TcpServer();

	void start();
	
	void setConnectionCallback(const ConnectionCallback& cb) {
		connectionCallback = cb;
	}
	
	void setMessageCallback(const MessageCallback& cb) {
		messageCallback = cb;
	}
private:
	typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;
	
	void newConnection(int sockfd, const InetAddress& peerAddr);
	
	EventLoop* loop;
	const std::string name;
	std::unique_ptr<Acceptor> acceptor;
	ConnectionCallback connectionCallback;
	MessageCallback messageCallback;
	bool started;
	int nextConnId;
	ConnectionMap connections;	

};


}

#endif
