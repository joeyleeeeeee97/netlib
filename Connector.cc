#include "Connector.h"
#include  "Channel.h"
#include "EventLoop.h"
#include "SocketsOps.h"

#include <functional>
#include <errno.h>
#include <iostream>

using namespace netlib;

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& addr):
	loop_(loop),
	serverAddr_(addr),
	connect_(false),
	state_(kDisconnected),
	retryDelayMs_(kInitRetryDelayMs){}

Connector::~Connector(){
	loop_->cancel(timerId_);
}

void Connector::start(){
	connect_ = true;
	Functor f = std::bind(&Connector::startInLoop, this);
	loop_->runInLoop(f);
}

void Connector::startInLoop() {
	loop_->assertInLoopThread();
	assert(state_ == kDisconnected);
	if(connect_){
		connect();
	}

}

void Connector::connect(){
	int sockfd = sockets::createNonblockingOrDie();
	int ret = sockets::connect(sockfd, serverAddr_.getSockAddrInet());
	int savedErrno = (ret == 0) ? 0 : errno;
	switch (savedErrno)
	{
    	case 0:
    	case EINPROGRESS:
    	case EINTR:
    	case EISCONN:
     		connecting(sockfd);
      		break;

    	case EAGAIN:
    	case EADDRINUSE:
    	case EADDRNOTAVAIL:
    	case ECONNREFUSED:
    	case ENETUNREACH:
      		retry(sockfd);
      		break;

    	case EACCES:
    	case EPERM:
    	case EAFNOSUPPORT:
    	case EALREADY:
    	case EBADF:
    	case EFAULT:
    	case ENOTSOCK:
//      		LOG_SYSERR << "connect error in Connector::startInLoop " << savedErrno;
      		sockets::close(sockfd);
      	break;

        default:
  //    		LOG_SYSERR << "Unexpected error in Connector::startInLoop " << savedErrno;
      		sockets::close(sockfd);
      // connectErrorCallback_();
      		break;
  	}
}

void Connector::restart(){
	loop_->assertInLoopThread();
	setState(kDisconnected);
	retryDelayMs_ = kInitRetryDelayMs;
	connect_ = true;
	startInLoop();
}


void Connector::stop(){
	connect_ = false;
	loop_->cancel(timerId_);
}


void Connector::connecting(int sockfd) {
	setState(kConnecting);
	channel_.reset(new Channel(loop_, sockfd));
	Functor f = std::bind(&Connector::handleWrite, this);
	channel_->setWriteCallBack(f);
	f = std::bind(&Connector::handleError, this);
	channel_->setErrorCallBack(f);

	channel_->enableWriting();
}


int Connector::removeAndResetChannel() {
	channel_->disableAll();
	loop_->removeChannel(channel_.get());
	int sockfd = channel_->fd();
	Functor f = std::bind(&Connector::resetChannel,this);
	loop_->queueInLoop(f);
	return sockfd;

}


void Connector::resetChannel(){
	channel_.reset();
}

void Connector::retry(int sockfd){
	sockets::close(sockfd);
	setState(kDisconnected);
	if(connect_) {
		std::function<void()> f = std::bind(&Connector::startInLoop, this);
		timerId_ = loop_->runAfter(retryDelayMs_ / 1000.0, f);
		retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
	}

}

void Connector::handleWrite() {
	if(state_ == kConnecting) {
		int sockfd = removeAndResetChannel();
		int err = sockets::getSocketError(sockfd);
		if(err){
	//		std::cout<< "Connector::handleWrite - SO_ERROR = "
        //       << err << " " << strerror_tl(err)<<std::endl;
			retry(sockfd);
		}
		else if(sockets::isSelfConnect(sockfd)) {retry(sockfd);}
		else {

			setState(kConnected);
			if(connect_){
				if(newConnectionCallback)
					newConnectionCallback(sockfd);
			}
			else{
				sockets::close(sockfd);
			}
		}
	}
}

void Connector::handleError(){
	int sockfd = removeAndResetChannel();
	int err = sockets::getSocketError(sockfd);
	retry(sockfd);
}
