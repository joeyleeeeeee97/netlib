#include "TcpConnection.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "SocketsOps.h"
#include <functional>
#include <string>
#include <iostream>
using namespace netlib;

TcpConnection::TcpConnection(EventLoop* _loop, const std::string& nameArg,
				int sockfd,
				const InetAddress& localAddr,const InetAddress& peerAddr):
	loop_(_loop), name_(nameArg),state_(kConnecting), socket_(new Socket(sockfd)),
	channel_(new Channel(_loop,sockfd)), localAddr_(localAddr),peerAddr_(peerAddr) {
	
	std::function<void(Timestamp)> f1 = std::bind(&TcpConnection::handleRead, this, std::placeholders::_1);
	channel_->setReadCallBack(f1);
	
	std::function<void()> f;
	f = std::bind(&TcpConnection::handleWrite, this);
	channel_->setWriteCallBack(f);

	f = std::bind(&TcpConnection::handleClose, this);
	channel_->setCloseCallBack(f);
	
	f = std::bind(&TcpConnection::handleClose, this);
	channel_->setErrorCallBack(f);
}


TcpConnection::~TcpConnection()
{
// LOG_DEBUG << "TcpConnection::dtor[" <<  name_ << "] at " << this
//            << " fd=" << channel_->fd();
}

void TcpConnection::shutdown() {
	if(state_ == kConnected) {
		setState(kDisconnecting);
		//TODO 
		// ADD A STATIC METHOD THAT ACCEPTS SHARED_PTR OF TCPPTR	
		Functor f = std::bind(&TcpConnection::shutdownInLoop, this);
		loop_->runInLoop(f);
	}
}	

void TcpConnection::shutdownInLoop(){
	loop_->assertInLoopThread();
	if(!channel_->isWriting()) {
		socket_->shutdownWrite();
	}

}

/*
void TcpConnection::send(const char* str, size_t len) {
	send(std::string(str,len));
}
*/
void TcpConnection::send(const std::string& str) {
	//send(str.data(), str.size());

	if(state_ == kConnected) {
		if(loop_->isInLoopThread()) {
			sendInLoop(str);
		}
		else {
			Functor f = std::bind(&TcpConnection::sendInLoop, this, str);
			loop_->runInLoop(f);
		}
	}
}

void TcpConnection::sendInLoop(const std::string& str) {
//	std::cout<<"In" <<std::endl;
	
	loop_->assertInLoopThread();
	ssize_t nwrote = 0;

	if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
		nwrote = ::write(channel_->fd(), str.data(), str.size());
		if(nwrote >= 0) {
	//		std::cout<<"wrote " << nwrote << std::endl;
		//	assert(nwrote <= str.size());
			if(nwrote >= str.size() && writeCompleteCallback) {
				Functor f = std::bind(writeCompleteCallback,shared_from_this());
//				f();	
				loop_->queueInLoop(f);
	//			std::cout<< "queued " <<std::endl;
			}
		}
		else {
			nwrote = 0;
			assert(errno == EWOULDBLOCK);
		}
	}

	assert(nwrote >= 0);

	if(nwrote < str.size()) {
		outputBuffer_.append(str.data() + nwrote, str.size() - nwrote);
		if(!channel_->isWriting()) {
			channel_->enableWriting();
		}

	}
}

void TcpConnection::connectEstablished() {
	loop_->assertInLoopThread();
	assert(state_ == kConnecting);
	setState(kConnected);
	channel_->enableReading();

	connectionCallback(shared_from_this());

}

void TcpConnection::setTcpNoDelay(bool on) {
	socket_->setTcpNoDelay(on);
}

void TcpConnection::connectDestroyed() {
	loop_->assertInLoopThread();
//	std::cout<<kConnected<<std::endl;
//	std::cout<<state_<<std::endl;
	assert(state_ == kConnected || state_ == kDisconnecting);//kDisConnecting);
	setState(kDisconnected);
	channel_->disableAll();

//	closeCallback(shared_from_this());
	connectionCallback(shared_from_this());
	loop_->removeChannel(channel_.get());
}

void TcpConnection::handleRead(Timestamp t) {
	int savedErrno = 0;
	ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
	if(n > 0) {
        if(messageCallback)
            messageCallback(shared_from_this(), &inputBuffer_, t);
	}
	else if (n == 0) {
		handleClose();
	}
	else {
//		errno = savedErrno;
		handleError();
	}

}


void TcpConnection::handleWrite() { 
	loop_->assertInLoopThread();
	if(channel_->isWriting()) {
		ssize_t n = ::write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
		if( n > 0) {
			outputBuffer_.retrieve(n);
			if(outputBuffer_.readableBytes() == 0) {
				channel_->disableWriting();
				if(writeCompleteCallback) {
					Functor f = std::bind(writeCompleteCallback, shared_from_this());
					loop_->queueInLoop(f);
				}
				if(state_ = kDisconnecting) {
					shutdownInLoop();
				}
			}
		}
	}
}

void TcpConnection::handleClose() {
	loop_->assertInLoopThread();
	assert(state_ == kConnected || state_ == kDisconnecting);
	channel_->disableAll();
	closeCallback(shared_from_this());
}

void TcpConnection::handleError() {
	int err = sockets::getSocketError(channel_->fd());
//
}
