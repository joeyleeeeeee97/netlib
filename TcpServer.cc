#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "SocketsOps.h"

#include <functional>
#include <stdio.h>
#include <iostream>

using namespace netlib;


TcpServer::TcpServer(EventLoop* loop_, const InetAddress& listenAddr):loop(loop_),
	name(listenAddr.toHostPort()),
	acceptor(new Acceptor(loop_,listenAddr)),
	started(false), nextConnId(1) {
//	std::function<void()>
	NewConnectionCallback f = std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2);
	acceptor->setNewConnectionCallback(f);	
}


TcpServer::~TcpServer() {

}

void TcpServer::start() {
	if(!started) {
		started = true;
	}
	if(!acceptor->listenning()) {
		std::function<void()> f = std::bind(&Acceptor::listen, acceptor.get());
		loop->runInLoop(f);
	}
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr) {
	loop->assertInLoopThread();
	char buf[32];
	snprintf(buf , sizeof buf, "#%d", nextConnId);
	++nextConnId;
	std::string connName = name + buf;

	std::cout  << "TcpServer::newConnection [" << name
           << "] - new connection [" << connName
           << "] from " << peerAddr.toHostPort();
	
	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	
	TcpConnectionPtr conn(new TcpConnection(loop,connName, sockfd, localAddr, peerAddr));

	connections[connName] = conn;
	conn->setConnectionCallback(connectionCallback);
	conn->setMessageCallback(messageCallback);
	conn->connectEstablished();
}
