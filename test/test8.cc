#include "../TcpServer.h"
#include "../EventLoop.h"
#include "../Timestamp.h"
#include "../InetAddress.h"
#include <stdio.h>
#include <unistd.h>

void onConnection(const netlib::TcpConnectionPtr& conn)
{
	if(conn->connected()) {
		printf("On Conn, %s from %s\n", conn->name().c_str(), conn->peerAddress().toHostPort().c_str());
	}
	else {
		printf("onConnection(): connection [%s] is down\n",conn->name().c_str());
	}
}

void onMessage(const netlib::TcpConnectionPtr& conn, netlib::Buffer* buf, netlib::Timestamp time) {
	printf("onMessage(): received %zd bytes from connection [%s] at %s\n",
         buf->readableBytes(),
         conn->name().c_str(),
         time.toFormattedString().c_str());

	printf("onMessage(): [%s]\n", buf->retrieveAsString().c_str());
	//printf("On Message %s received %d \n", conn->name().c_str(), len);
}

int main() {
	printf("main pid = %d\n", getpid());
	netlib::InetAddress listenAddr(9981);
	netlib::EventLoop loop;
	netlib::TcpServer server(&loop, listenAddr);

	server.setConnectionCallback(onConnection);
	server.setMessageCallback(onMessage);
	server.start();
	
	loop.loop();

}
