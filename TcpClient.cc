#include "TcpClient.h"
#include "Connector.h"
#include "EventLoop.h"
#include "SocketsOps.h"
#include <functional>
#include <memory>
#include <stdio.h>
#include <string>
using namespace netlib;

namespace netlib{

void RemoveConnection(EventLoop* loop, const TcpConnectionPtr& conn) {
	Functor f = std::bind(&TcpConnection::connectDestroyed, conn);
	loop->queueInLoop(f);
}

void removeConector(const ConnectionPtr& connector) {
//	connector->setState(kDisconnected);
}


TcpClient::TcpClient(EventLoop* loop, const InetAddress& serverAddr):
	loop_(loop),
	connector_(new Connector(loop,serverAddr)),
	retry_(false),
	connect_(true),
	nextConnId_(1) {
		std::function<void(int)> f = std::bind(&TcpClient::newConnection, this ,std::placeholders::_1);
		connector_->setNewConnectionCallback(f);
}



TcpClient::~TcpClient(){
	TcpConnectionPtr conn;

	{
		std::lock_guard<std::mutex> lk(mutex_);
		conn = connection_;
	}

	if(conn) {
		CloseCallback  cb = std::bind(&RemoveConnection, loop_, std::placeholders::_1);
		Functor f = std::bind(&TcpConnection::setCloseCallback, conn, cb);
		loop_->runInLoop(f);//(()[cb,conn]{conn->setCloseCallback(cb);});
	}
	else {
		connector_->stop();
	}

}

void TcpClient::connect() {
	connect_ = true;
	connector_->start();
}

void TcpClient::disconnect() {

	connect_ = false;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if(connection_)
			connection_->shutdown();
	}	
}

void TcpClient::stop(){
	connect_ = false;
	connector_->stop();
}

void TcpClient::newConnection(int sockfd) {
	loop_->assertInLoopThread();
	InetAddress peerAddr(sockets::getPeerAddr(sockfd));
	char buf[32];
	snprintf(buf, sizeof buf, ":%s#%d", peerAddr.toHostPort().c_str(), nextConnId_);
	++nextConnId_;
	std::string connName = buf;

	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	TcpConnectionPtr conn(new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr));

	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	std::function<void(const TcpConnectionPtr&)> f= std::bind(&TcpClient::removeConnection, this , std::placeholders::_1);
	conn->setCloseCallback(f);

	{
		std::lock_guard<std::mutex> lock(mutex_);
		connection_ = conn;
	}

	conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn) {
	loop_->assertInLoopThread();
	
	{
		std::lock_guard<std::mutex> lock(mutex_);
		connection_.reset();
	}
	
	Functor f = std::bind(&TcpConnection::connectDestroyed, conn);
	loop_->queueInLoop(f);

	if(retry_ && connect_){
		connector_->restart();
	}

}


}
