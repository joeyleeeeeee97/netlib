/*************************************************************************
	> File Name: test14.cpp
	> Author: 
	> Mail: 
	> Created Time: Wed 04 Jul 2018 12:01:55 PM DST
 ************************************************************************/

#include<iostream>
#include"../TcpServer.h"
#include"../TcpClient.h"
#include"../EventLoop.h"
#include"../Timestamp.h"
#include"../Buffer.h"
#include<functional>
#include<string>

using namespace netlib;

#pragma pack(push)
#pragma pack(4)
class A{
public:
    A(){}
    std::string name;
    int id;
};
#pragma pack(pop)


void client_onConnection(const TcpConnectionPtr& conn) {
    if(conn->connected()) {
        std::cout<<" connected from " << conn->name()<< std::endl;
        A a;
        
        a.name = "joeylee";
        a.id =  456;



        conn->send(&a, sizeof(A));
        
    }

}



void server_onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time) {
    while(buf->readableBytes() >= sizeof(A)) {
        std::cout<<"read " << buf->peekAsString()<<std::endl;
        A* res = static_cast<A*>((void*)buf->peek());
        std::cout<<res->name<<res->id<<std::endl;
    }
}



int main(){
    int flag ;
    std::cin >> flag;
    if(flag == 1) {
        InetAddress addr(9981);
        EventLoop loop;
        TcpClient client(&loop, addr);
        client.setConnectionCallback(client_onConnection);
        
        client.connect();
        loop.loop();
    }
    else {
        InetAddress addr(9981); 
        EventLoop loop;
        TcpServer server(&loop, addr);
        server.setConnectionCallback([](const TcpConnectionPtr&){});
        server.setMessageCallback(server_onMessage);

        server.start();
        loop.loop();
    }
}
