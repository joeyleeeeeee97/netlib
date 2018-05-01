#include "../TcpServer.h"
#include "../EventLoop.h"
#include "../InetAddress.h"
#include <stdio.h>
#include <iostream>
std::string message(200,'C');

void onConnection(const netlib::TcpConnectionPtr& conn)
{
  if (conn->connected())
  {
    printf("onConnection(): new connection [%s] from %s\n",
           conn->name().c_str(),
           conn->peerAddress().toHostPort().c_str());
    conn->send(message);
  }
  else
  {
    printf("onConnection(): connection [%s] is down\n",
           conn->name().c_str());
  }
}

void onWriteComplete(const netlib::TcpConnectionPtr& conn)
{
//	exit(1);
//	std::cout<<1;
	conn->send("abvc");
}

void onMessage(const netlib::TcpConnectionPtr& conn,
               netlib::Buffer* buf,
               netlib::Timestamp receiveTime)
{
  printf("onMessage(): received %zd bytes from connection [%s] at %s\n",
         buf->readableBytes(),
         conn->name().c_str(),
         receiveTime.toFormattedString().c_str());

  buf->retrieveAll();
}

int main()
{
  printf("main(): pid = %d\n", getpid());

 /* std::string line;
  for (int i = 33; i < 127; ++i)
  {
    line.push_back(char(i));
  }
  line += line;

  for (size_t i = 0; i < 127-33; ++i)
  {
    message += line.substr(i, 72) + '\n';
  }
*/
  netlib::InetAddress listenAddr(9981);
  netlib::EventLoop loop;

  netlib::TcpServer server(&loop, listenAddr);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.setWriteCompleteCallback(onWriteComplete);
  server.start();

  loop.loop();
}
