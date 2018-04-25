#include "../EventLoop.h"
#include <functional>
#include <stdio.h>
#include <unistd.h>
int cnt = 0;
netlib::EventLoop* g_loop;

void printTid(){
	printf("pid = %d, tid = %d\n", getpid(), netlib::curThreadId());
	printf("now %s\n", netlib::Timestamp::now().toString().c_str());
}

void print(const char* msg)
{
  printf("msg %s %s\n", netlib::Timestamp::now().toString().c_str(), msg);
  if (++cnt == 20)
  {
    g_loop->quitloop();
  }
}

int main()
{
  printTid();
  netlib::EventLoop loop;
  g_loop = &loop;

  print("main");
  std::function<void()> f = std::bind(print,"once1");
  loop.runAfter(1, f);
  f = std::bind(print,"once1.5");
  loop.runAfter(1.5, f);
  f = std::bind(print,"once2.5");
  loop.runAfter(2.5, f);
  f = std::bind(print,"once3.5");
  loop.runAfter(3.5, f);
  f = std::bind(print,"every2.5");
  loop.runEvery(2, f);
  f = std::bind(print,"every3");
  loop.runEvery(3, f);

  loop.loop();
  print("main loop exits");
  sleep(1);
}
