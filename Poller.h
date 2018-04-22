#ifndef _POLLER_H
#define _POLLER_H
#include <map>
#include <vector>
#include "Timestamp.h"
#include "EventLoop.h"
#include "nocopyable.h"

struct pollfd;
namespace netlib{

class Channel;

class Poller : nocopyable{
public:
	typedef std::vector<Channel*> ChannelVec;
	
	Poller(EventLoop* loop);
	~Poller();

	Timestamp poll(int timeoutMs, ChannelVec* activeChannels);

	void updateChannel(Channel* channel);

	void assertInLoopThread() {
		ownerloop->assertInLoopThread();
	}

private:
	void fillActiveChannels(int numEvents,
				ChannelVec* activeChannels) const;

	typedef std::vector<struct pollfd> PollFdVec;
	typedef std::map<int, Channel*> ChannelMap;

	EventLoop* ownerloop;
	PollFdVec pollfds;
	ChannelMap channels;
};


}
#endif
