#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketsOps.h"

#include <functional>

using namespace netlib;

Acceptor::Acceptor(EventLoop* _loop, const InetAddress& listenAddr):loop(_loop),
	acceptSocket(sockets::createNonblockingOrDie()),
	acceptChannel(loop,acceptSocket.fd()),
	listenning_(false) {
	acceptSocket.setReuseAddr(true);
	acceptSocket.bindAddress(listenAddr);
	std::function<void(Timestamp)> f = std::bind(&Acceptor::handleRead, this, std::placeholders::_1);
	acceptChannel.setReadCallBack(f);

}

void Acceptor::listen() {
	loop->assertInLoopThread();
	listenning_ = true;
	acceptSocket.listen();
	acceptChannel.enableReading();
}

void Acceptor::handleRead(Timestamp t) {
	loop->assertInLoopThread();
	InetAddress peerAddr(0);

	int connfd = acceptSocket.accept(&peerAddr);
	if(newConnectionCallback){
		newConnectionCallback(connfd,peerAddr);
	}
	else{
		sockets::close(connfd);
	}

}


