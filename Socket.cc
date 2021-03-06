#include "Socket.h"
#include "InetAddress.h"
#include "SocketsOps.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>

using namespace netlib;

Socket::~Socket(){
	sockets::close(sockfd);
}

void Socket::bindAddress(const InetAddress& addr) {
	sockets::bindOrDie(sockfd, addr.getSockAddrInet());
}

void Socket::listen() {
	sockets::listenOrDie(sockfd);
}

int Socket::accept(InetAddress* peeraddr){
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	int connfd = sockets::accept(sockfd, &addr);
	if(connfd > 0) {
		peeraddr->setSockAddrInet(addr);
	}
	return connfd;
}

void Socket::setReuseAddr(bool on)
{
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
               &optval, sizeof optval);
}

void Socket::shutdownWrite(){
	sockets::shutdownWrite(sockfd);
}

void Socket::setTcpNoDelay(bool on) {
	int optval = on ? 1 : 0;
	::setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof optval);
}
