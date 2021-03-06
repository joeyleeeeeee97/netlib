#ifndef _SOCKET_H
#define _SOCKET_H

#include "nocopyable.h"

namespace netlib{

class InetAddress;

class Socket : nocopyable {
public:
	explicit Socket(int _sockfd):sockfd(_sockfd){}
	~Socket();

	int fd() const {
		return sockfd;
	} 

	void setTcpNoDelay(bool);
	
	void bindAddress(const InetAddress& localaddr);

	void listen();
	
	void shutdownWrite();
		
	int accept(InetAddress* peeraddr);

	void setReuseAddr(bool on);

private:
	const int sockfd;
};

}


#endif
