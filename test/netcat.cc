#include "../EventLoop.h"
#include "../TcpServer.h"
#include "../SocketsOps.h"
#include "../Buffer.h"
#include "../ThreadFunc.h"
#include "../InetAddress.h"
#include <functional>
#include "../Memcached.h"
#include "../TcpClient.h"
#include <string>
#include <iostream>
#include <unistd.h>  
#include <string.h>  
#include <stdio.h>  
#include <fcntl.h>  
#include <mutex>

using namespace netlib;

TcpConnectionPtr connection;
std::mutex mtx;
Buffer stdin_buf;

void OnConnection(const TcpConnectionPtr& conn) {
	std::cout<<"On Connection\n";
	connection = conn;

}

void OnMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time) {
	std::cout<<buf->retrieveAsString();
}

void Flush() {
	std::cout<<"FLUSH" <<std::endl;
//	int errno;
//	stdin_buf.readFd(STDIN_FILENO, &errno);
//	mtx.lock();
	if(stdin_buf.readableBytes() > 0) {
		std::cout<<"SEND"<<std::endl;
		mtx.lock();
		connection->send(stdin_buf.retrieveAsString());
		mtx.unlock();
	}
//	mtx.unlock();
}

void Stdin_func(){
	string str;
	while(1){
		cin>>str;
		mtx.lock();
	//	std::cout<<"IN BUF "<< str<< std::endl;
		if(!str.empty())
		stdin_buf.append(str);
		mtx.unlock();	
	}
}

void Server() {
	InetAddress addr(9981);
	EventLoop loop;
	TcpServer server(&loop, addr);
	
	server.setConnectionCallback(OnConnection);
        server.setMessageCallback(OnMessage);
//	server.setConnectionCallback(OnConnection);
//	server.setMessageCallback(OnMessage);
	
	server.start();
	loop.loop();
}



void Client() {
	InetAddress addr(9981);
	EventLoop loop;
	TcpClient client(&loop, addr);
	
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);   // 设置为非阻塞形式  
	client.setConnectionCallback(OnConnection);
	client.setMessageCallback(OnMessage);
	loop.runEvery(2, Flush);
	
	//std::thread t(Stdin_func);
	client.connect();
	loop.loop();
}


int main(int argc, char* argv[]) {
	ThreadFunc t;
	if(argc == 2){
		t = ThreadFunc(Client);
	}
	else{
		t = ThreadFunc(Server);
	}
	t.start();
	Stdin_func();
	t.join();
}
