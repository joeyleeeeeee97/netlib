#include "SocketsOps.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>  // snprintf
#include <strings.h>  // bzero
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
using namespace netlib;

//namespace{

template<typename To, typename From>
inline To implicit_cast(From f)
{
  return f;
}

typedef struct sockaddr SA;

const SA* sockaddr_cast(const struct sockaddr_in* addr) {
	return static_cast<const SA*>(implicit_cast<const void*>(addr));
}

SA* sockaddr_cast(struct sockaddr_in* addr) {
	return static_cast<SA*>(implicit_cast<void*>(addr));
}

void setNonblockAndCloseOnExec(int sockfd) {
	int flags = ::fcntl(sockfd,F_GETFL,0);
	flags |= O_NONBLOCK;
	int ret = ::fcntl(sockfd, F_SETFL, flags);

	flags = ::fcntl(sockfd, F_GETFD, 0);
	flags |= FD_CLOEXEC;
	ret  = ::fcntl(sockfd, F_SETFD, flags);

}
//createNonblockingOrDie
int sockets::createNonblockingOrDie() {
	int sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	setNonblockAndCloseOnExec(sockfd);
	return sockfd;
}

void sockets::bindOrDie(int sockfd, const struct sockaddr_in& addr) {
	int ret = ::bind(sockfd, sockaddr_cast(&addr), sizeof(addr));
//	assert(ret>=0);
}

void sockets::listenOrDie(int sockfd) {
	int ret = ::listen(sockfd, SOMAXCONN);
//	assert(ret>=0);
}

int sockets::accept(int sockfd, struct sockaddr_in* addr) {
	socklen_t addrlen = sizeof(*addr);
//	setNonblockAndCloseOnExec(sockfd);

	int connfd = ::accept(sockfd, sockaddr_cast(addr), &addrlen);//, SOCK_NONBLOCK | SOCKCLOEXEC);
	setNonblockAndCloseOnExec(connfd);
//	assert(connfd >= 0);
	return connfd;
}

int sockets::connect(int sockfd, const struct sockaddr_in& addr) {
	return ::connect(sockfd, sockaddr_cast(&addr), sizeof addr);
}

void sockets::close(int sockfd) {
	assert(::close(sockfd)>=0);
}

void sockets::shutdownWrite(int sockfd) {
	if(::shutdown(sockfd, SHUT_WR) < 0) {
//		LOG_SYSERR << "sockets::shutdown";
	}
}

void sockets::toHostPort(char* buf, size_t size, const struct sockaddr_in& addr) {
	char host[INET_ADDRSTRLEN] = "INVALID";
	::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof(host));
	uint16_t port = sockets::networkToHost16(addr.sin_port);
	snprintf(buf,size,"%s:%u", host, port);
}


void sockets::fromHostPort(const char* ip, uint16_t port, struct sockaddr_in* addr) {
	addr->sin_family = AF_INET;
	addr->sin_port = hostToNetwork16(port);
	::inet_pton(AF_INET, ip, &addr->sin_addr);
		
//	assert(::inet_pton(AF_INET, ip, &addr->sinaddr) > 0 );
}

struct sockaddr_in sockets::getLocalAddr(int sockfd) {
	struct sockaddr_in localaddr;
	bzero(&localaddr , sizeof localaddr);
	socklen_t addrlen = sizeof(localaddr);
	::getsockname(sockfd,sockaddr_cast(&localaddr), &addrlen);
	return localaddr;
}

struct sockaddr_in sockets::getPeerAddr(int sockfd) {
	struct sockaddr_in peeraddr;
	bzero(&peeraddr, sizeof peeraddr);
	socklen_t addrlen = sizeof(peeraddr);
	if(::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0) {
		
	}
	return peeraddr;

}
int sockets::getSocketError(int sockfd) {
	int optval;
	socklen_t optlen = sizeof optval;

	if(::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
		return -1;	
	}
	else {
		return optval;
	}
		
}

bool sockets::isSelfConnect(int sockfd) {
	struct sockaddr_in localaddr = getLocalAddr(sockfd);
	struct sockaddr_in peeraddr = getPeerAddr(sockfd);
	
	return localaddr.sin_port == peeraddr.sin_port && 
		localaddr.sin_addr.s_addr == peeraddr.sin_addr.s_addr;

}
