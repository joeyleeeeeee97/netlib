#pragma once
#ifndef _TCPCONNECTION_H
#define _TCPCONNECTION_H

#include "Buffer.h"
#include"InetAddress.h"
#include"nocopyable.h"
#include "CallBacks.h"
#include <boost/any.hpp>
#include <memory>

namespace netlib{


class Channel;
class EventLoop;
class Socket;

class TcpConnection : nocopyable, public std::enable_shared_from_this<TcpConnection> {
public:
	TcpConnection(EventLoop*,const std::string&, int sockfd, const InetAddress&,const InetAddress&);
	~TcpConnection();

	void send(const std::string&);
//	void send(const char*, size_t);

	void shutdown(); 

	EventLoop* getLoop() const {
		return loop_;
	}
	
	const std::string& name() const{
		return name_;
	}
	const InetAddress& localAddress() { return localAddr_; }
	const InetAddress& peerAddress() { return peerAddr_; }
	bool connected() const { return state_ == kConnected; }

	void setConnectionCallback(const ConnectionCallback& cb){
		connectionCallback = cb;
	}

	void setMessageCallback(const MessageCallback& cb){
		messageCallback = cb;
	}

	void setCloseCallback(const CloseCallback& cb) {
		closeCallback = cb;

	}

	void setTcpNoDelay(bool on);

	void connectDestroyed();	
	void connectEstablished();

	void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
		writeCompleteCallback = cb;
	}

private:

	enum StateE {
		kConnecting,
		kConnected,
		kDisconnecting,
		kDisconnected,
	};

	void setState(StateE s) {
		state_ = s;
	}

	void handleRead(Timestamp);
	void handleWrite();
	void handleClose();
	void handleError();

	//无法使用bind 绑定一个重载函数
	void sendInLoop(const std::string&);
//	void sendInLoop(const char*,size_t);
	void shutdownInLoop();

	EventLoop* loop_;
	std::string name_;
	StateE state_;
	std::unique_ptr<Socket> socket_;
	std::unique_ptr<Channel> channel_;
	InetAddress localAddr_;
	InetAddress peerAddr_;
	ConnectionCallback connectionCallback;
	MessageCallback messageCallback;
	CloseCallback closeCallback;
	WriteCompleteCallback writeCompleteCallback;
	Buffer inputBuffer_;
	Buffer outputBuffer_;
};
/*
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&,
                              const char* data,
                              ssize_t len)> MessageCallback;

*/
}


#endif
