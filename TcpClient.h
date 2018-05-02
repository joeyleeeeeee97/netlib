#pragma once
#ifndef _TCPCLIENT_H
#define _TCPCLIENT_H

#include "nocopyable.h"

#include <mutex>
#include "TcpConnection.h"

namespace netlib{

class Connector;
typedef std::shared_ptr<Connector> ConnectionPtr;

class TcpClient : nocopyable {
public:
	TcpClient(EventLoop*, const InetAddress&);
	~TcpClient();

	void connect();
	void disconnect();
	void stop();

	TcpConnectionPtr connection() const{
		std::lock_guard<std::mutex> lk(mutex_);
		return connection_;
	}

	bool retry() const;
	void enableRetry() {
		retry_ = true;
	}

	void setConnectionCallback(const ConnectionCallback& cb) {
		connectionCallback_ = cb;
	}

	void setMessageCallback(const MessageCallback& cb) {
		messageCallback_ = cb;
	}

	void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
		writeCompleteCallback_ = cb;
	}
private:
	
	void newConnection(int sockfd);
	void removeConnection(const TcpConnectionPtr& conn);
	
	EventLoop* loop_;
	ConnectionPtr connector_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	
	bool retry_;
	bool connect_;
	int nextConnId_;
	mutable std::mutex mutex_;
	TcpConnectionPtr connection_;
};

}



#endif

