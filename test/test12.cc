#include "../Connector.h"
#include "../EventLoop.h"

#include <stdio.h>

netlib::EventLoop* g_loop;

void connectCallback(int sockfd)
{
  printf("connected.\n");
  g_loop->quitloop();
}

int main(int argc, char* argv[])
{
  netlib::EventLoop loop;
  g_loop = &loop;
  netlib::InetAddress addr("127.0.0.1", 9981);
  netlib::ConnectorPtr connector(new netlib::Connector(&loop, addr));
  connector->setNewConnectionCallback(connectCallback);
  connector->start();

  loop.loop();
}
