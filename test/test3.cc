#include"../Timestamp.h"
#include"../Channel.h"
#include"../EventLoop.h"
#include<unistd.h>
#include<strings.h>
#include<stdio.h>
#include<sys/timerfd.h>
#include<functional>
netlib::EventLoop*  g_loop;

void timeout(netlib::Timestamp tt){
	printf("%s Timeout\n", tt.toFormattedString().c_str());
	g_loop->quitloop();
}

int main(){
	printf("%s started\n", netlib::Timestamp::now().toFormattedString().c_str());
	netlib::EventLoop el;
	g_loop = &el;
	std::function<void(netlib::Timestamp)> f(timeout);
	int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  	
	netlib::Channel channel(&el, timerfd);
  	channel.setReadCallBack(f);
  	channel.enableReading();
	
	struct itimerspec howlong;
	bzero(&howlong, sizeof howlong);
	howlong.it_value.tv_sec = 5;
	::timerfd_settime(timerfd, 0, &howlong, NULL);

	el.loop();

	::close(timerfd);
}

