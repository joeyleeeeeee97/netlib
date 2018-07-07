#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "SocketsOps.h"

#include <functional>
#include <stdio.h>

using namespace netlib;


TcpServer::TcpServer(EventLoop* loop_, const InetAddress& listenAddr):loop(loop_),
	name(listenAddr.toHostPort()),
	acceptor(new Acceptor(loop_,listenAddr)),
	started(false), nextConnId(1), connectionCallback(), messageCallback(), writeCompleteCallback() {
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
	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	
	TcpConnectionPtr conn(new TcpConnection(loop,connName, sockfd, localAddr, peerAddr));

	connections[connName] = conn;
	conn->setConnectionCallback(connectionCallback);
	conn->setMessageCallback(messageCallback);
	conn->setWriteCompleteCallback(writeCompleteCallback);
	CloseCallback f = std::bind(&TcpServer::removeConnection, this , std::placeholders::_1);
	conn->setCloseCallback(f);
	conn->connectEstablished();
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn) {
	loop->assertInLoopThread();
	connections.erase(conn->name());
	Functor f = std::bind(&TcpConnection::connectDestroyed, conn);
	loop->queueInLoop(f);

}
