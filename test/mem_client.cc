#include"../EventLoop.h"
#include"../TcpClient.h"
#include"../Buffer.h"
#include"../TcpConnection.h"

#include<iostream>
#include<functional>
#include<memory>
#include<string>

using namespace netlib;

std::string message = "GET 1\r\nSET 1 A\r\nGET 1\r\n";

void OnConnection(const TcpConnectionPtr& conn) {
	std::cout<<"Connected!"<<std::endl;
	conn->send(message);
}

void OnMessage(const TcpConnectionPtr& conn, Buffer* buf ,Timestamp t) {
	std::cout<<buf->retrieveAsString()<<std::endl;
}

int main(){
	EventLoop loop;
	InetAddress addr(9981);
	TcpClient client(&loop, addr);

	client.setConnectionCallback(OnConnection);
	client.setMessageCallback(OnMessage);
	client.connect();
	loop.loop();


}
