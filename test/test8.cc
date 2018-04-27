#include "../TcpServer.h"
#include "../EventLoop.h"
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

void onMessage(const netlib::TcpConnectionPtr& conn, const char* data, ssize_t len) {

	printf("On Message %s received %d \n", conn->name().c_str(), len);
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
