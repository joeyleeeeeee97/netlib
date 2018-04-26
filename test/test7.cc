#include "../Acceptor.h"
#include "../EventLoop.h"
#include "../InetAddress.h"
#include "../SocketsOps.h"
#include <stdio.h>
#include <unistd.h>

void newConnection(int sockfd, const netlib::InetAddress& peerAddr)
{
  printf("newConnection(): accepted a new connection from %s\n",
         peerAddr.toHostPort().c_str());
  ::write(sockfd, "How are you?\n", 13);
   netlib::sockets::close(sockfd);
}

int main()
{
  printf("main(): pid = %d\n", getpid());

  netlib::InetAddress listenAddr(9981);
  netlib::EventLoop loop;

  netlib::Acceptor acceptor(&loop, listenAddr);
  acceptor.setNewConnectionCallback(newConnection);
  acceptor.listen();

  loop.loop();
}
