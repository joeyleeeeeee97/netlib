#ifndef _EPOLLER_H
#define _EPOLLER_H

#include<map>
#include<vector>
#include "nocopyable.h"
#include"Timestamp.h"
#include"EventLoop.h"

struct epoll_event;

namespace netlib{

class Channel;

class EPoller : nocopyable {

public:
	typedef std::vector<Channel*> ChannelList;

	EPoller(EventLoop*);
	~EPoller();

	Timestamp poll(int, ChannelList*);
	void updateChannel(Channel*);
	void removeChannel(Channel*);
	void assertInLoopThread(){
		ownerLoop_->assertInLoopThread();
	}

private:
	static const int kInitEventListSize = 16;
	
	void fillActiveChannels(int, ChannelList*) const;

	void update(int , Channel*);
	typedef std::vector<struct epoll_event> EventList;
	typedef std::map<int, Channel*> ChannelMap;

	EventLoop* ownerLoop_;
	int epollfd_;
	EventList events_;
	ChannelMap channels_;
};

}
#endif
