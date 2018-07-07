#include "../TcpServer.h"
#include "../EventLoop.h"
#include "../InetAddress.h"
#include <stdio.h>
#include <unistd.h>


void onConnection(const netlib::TcpConnectionPtr& conn)
{
  if (conn->connected())
  {
    printf("onConnection(): new connection [%s] from %s\n",
           conn->name().c_str(),
           conn->peerAddress().toHostPort().c_str());
    conn->shutdown();
  }
  else
  {
    printf("onConnection(): connection [%s] is down\n",
           conn->name().c_str());
  }
}


int main()
{
  printf("main(): pid = %d\n", getpid());

  netlib::InetAddress listenAddr(9981);
  netlib::EventLoop loop;

  netlib::TcpServer server(&loop, listenAddr);
  server.setConnectionCallback(onConnection);
  server.start();

  loop.loop();
}
