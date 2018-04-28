#ifndef _CALLBACKS_H
#define _CALLBACKS_H
#include<memory>
#include<functional>
#include"Timestamp.h"


namespace netlib{

	class Buffer;
	class TcpConnection;
	class InetAddress;

	
        typedef std::function<void(int sockfd, const InetAddress&)> NewConnectionCallback;
        typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
        typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
        typedef std::function<void (const TcpConnectionPtr&,
                              Buffer*,
			      Timestamp)> MessageCallback;
	typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;


}

#endif
