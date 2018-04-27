#include "Poller.h"
#include "Channel.h"
#include <assert.h>
#include <poll.h>

using namespace netlib;

Poller::Poller(EventLoop* loop): ownerloop(loop){}

Poller::~Poller(){}

Timestamp Poller::poll(int timeoutMs, ChannelVec* activeChannels) {
	int numEvents = ::poll(&*pollfds.begin(),pollfds.size(),timeoutMs);
	Timestamp now(Timestamp::now());
	if(numEvents > 0) {
		fillActiveChannels(numEvents, activeChannels);
	}
	return now;
}

void Poller::fillActiveChannels(int numEvents, ChannelVec* activeChannels) const {
	for(auto it = pollfds.cbegin();it != pollfds.cend(); it++) {
		if(!numEvents) break;
		if(it->revents > 0) {
			--numEvents;
			ChannelMap::const_iterator ch = channels.find(it->fd);
			assert(ch != channels.end());
			Channel* channel = ch->second;
			assert(channel->fd() == it->fd);
			channel->set_revents(it->revents);
			activeChannels->push_back(channel);
		}
	}
}

void Poller::updateChannel(Channel* channel) {
	assertInLoopThread();
	if(channel->index() < 0) {
		struct pollfd pfd;
		pfd.fd = channel->fd();
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		pollfds.push_back(pfd);
		channel->set_index( static_cast<int>(pollfds.size() - 1));
		channels[pfd.fd] = channel;
	}
	else {
		assert(channels.find(channel->fd()) != channels.end());
		assert(channels[channel->fd()] == channel);	
		int idx = channel->index();
		assert(0 <= idx && idx < static_cast<int>(pollfds.size()));
		struct pollfd& pfd = pollfds[idx];
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		if(channel->isNoneEvent()) {
			pfd.fd = -1;
		}	
	}

}

void Poller::removeChannel(Channel* channel) {
	assertInLoopThread();
	assert(channels[channel->fd()] == channel);
	assert(channel->isNoneEvent());
	int idx = channel->index();
	assert(idx >= 0 && idx < static_cast<int>(pollfds.size()));
	if(idx !=  static_cast<int>(pollfds.size()) - 1) {
		int atEnd = pollfds.back().fd;
		std::swap(pollfds[idx], pollfds.back());
		channels[atEnd]->set_index(idx);
	}
	channels.erase(channel->fd());
	pollfds.pop_back();

}
