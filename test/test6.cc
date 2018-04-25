#include "../EventLoop.h"
#include "../EventLoopThread.h"
#include <stdio.h>
#include <unistd.h>


void runInThread()
{
  printf("runInThread(): pid = %d, tid = %d\n",
         getpid(), netlib::curThreadId());//muduo::CurrentThread::tid());
}

int main()
{
  printf("main(): pid = %d, tid = %d\n",
         getpid(),netlib::curThreadId());// muduo::CurrentThread::tid());

  netlib::EventLoopThread loopThread;
  printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1"); 
  netlib::EventLoop* loop = loopThread.startLoop();
  printf("!!!!!!!!!!!!!!!!!!!!!!2"); 
  loop->runInLoop(runInThread);
  printf("!!!!!!!!!!!!!!!!!!!3"); 
 	sleep(1);
  loop->runAfter(2, runInThread);
  sleep(3);
  loop->quitloop();

  printf("exit main().\n");
}
