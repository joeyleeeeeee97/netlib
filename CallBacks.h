#ifndef _CALLBACKS_H
#define _CALLBACKS_H
#include<memory>
#include<functional>


namespace netlib{
	class TcpConnection;
	class InetAddress;

	
        typedef std::function<void(int sockfd, const InetAddress&)> NewConnectionCallback;
        typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
        typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
        typedef std::function<void (const TcpConnectionPtr&,
                              const char* data,
                              ssize_t len)> MessageCallback;
	typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;


}

#endif
