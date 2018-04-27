#include "TcpConnection.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "SocketsOps.h"
#include <functional>

using namespace netlib;

TcpConnection::TcpConnection(EventLoop* _loop, const std::string& nameArg,
				int sockfd,
				const InetAddress& localAddr,const InetAddress& peerAddr):
	loop_(_loop), name_(nameArg),state_(kConnecting), socket_(new Socket(sockfd)),
	channel_(new Channel(_loop,sockfd)), localAddr_(localAddr),peerAddr_(peerAddr) {
	
	std::function<void()> f = std::bind(&TcpConnection::handleRead, this);
	channel_->setReadCallBack(f);
	
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

void TcpConnection::connectEstablished() {
	loop_->assertInLoopThread();
	assert(state_ == kConnecting);
	setState(kConnected);
	channel_->enableReading();

	connectionCallback(shared_from_this());

}

void TcpConnection::connectDestroyed() {
	loop_->assertInLoopThread();
	assert(state_ == kConnected);
	setState(kDisconnected);
	channel_->disableAll();

	closeCallback(shared_from_this());
//	connectionCallback(shared_from_this());
	loop_->removeChannel(channel_.get());
}

void TcpConnection::handleRead() {
	char buf[65535];
	ssize_t n = :: read(channel_->fd(), buf, sizeof buf);
	if(n > 0) {
		messageCallback(shared_from_this(), buf, n);
	}
	else if (n == 0) {
		handleClose();
	}
	else {
		handleError();
	}

}

//todo 
void TcpConnection::handleWrite(){}

void TcpConnection::handleClose() {
	loop_->assertInLoopThread();
	assert(state_ = kConnected);
	channel_->disableAll();
	closeCallback(shared_from_this());
}

void TcpConnection::handleError() {
	int err = sockets::getSocketError(channel_->fd());
//
}
