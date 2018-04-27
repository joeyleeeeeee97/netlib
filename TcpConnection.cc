#include "TcpConnection.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"

#include <functional>

using namespace netlib;

TcpConnection::TcpConnection(EventLoop* _loop, const std::string& nameArg,
				int sockfd,
				const InetAddress& localAddr,const InetAddress& peerAddr):
	loop_(_loop), name_(nameArg),state_(kConnecting), socket_(new Socket(sockfd)),
	channel_(new Channel(_loop,sockfd)), localAddr_(localAddr),peerAddr_(peerAddr) {
	
	std::function<void()> f = std::bind(&TcpConnection::handleRead, this);
	channel_->setReadCallBack(f);
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

void TcpConnection::handleRead() {
	char buf[65535];
	ssize_t n = :: read(channel_->fd(), buf, sizeof buf);
	messageCallback(shared_from_this(), buf, n);

}
