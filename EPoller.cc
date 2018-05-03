#include "EPoller.h"
#include "Channel.h"
#include <functional>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <sys/epoll.h>

using namespace netlib;
const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EPoller::EPoller(EventLoop* loop):
	ownerLoop_(loop),
	epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
	events_(kInitEventListSize) {}


EPoller::~EPoller(){
	::close(epollfd_);
}


Timestamp  EPoller::poll(int timeoutMs, ChannelList* activeChannels) {
	int numEvents = ::poll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
	Timestamp now(Timestamp::now());
	
	if(numEvents > 0) {
		fillActiveChannels(numEvents, activeChannels);
		if(numEvents == static_cast<int>(events_.size())) {
			events_.resize(events_.size() * 2);
		}
	}	

	return now;
}

void EPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
	for(int i = 0;i < numEvents;i++){
		Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
		channel->set_revents(events_[i].events);
		activeChannels->push_back(channel);
	}

}


void EPoller::updateChannel(Channel* channel) {
	assertInLoopThread();
	const int index = channel->index();
	
	if(index == kNew || index == kDeleted) {
		int fd = channel->fd();
		if(index == kNew) {
			channels_[fd] = channel;
		}
		channel->set_index(kAdded);
		update(EPOLL_CTL_ADD, channel);	
	}
	else {
		int fd = channel->fd();
		if(channel->isNoneEvent()){
			update(EPOLL_CTL_DEL, channel);
			channel->set_index(kDeleted);
		}
		else {
			update(EPOLL_CTL_ADD, channel);
		}
	}
}


void EPoller::removeChannel(Channel* channel) {
	//assertInLoopThread();
	int fd = channel->fd();
	channels_.erase(fd);
	if(index == kAdded) {
		update(EPOLL_CTL_DEL, channel);
	}	
	channel->set_index(kNew);
}


void EPoller::update(int operation, Channel* channel) {
	struct epoll_event event;
	bezero(&event, sizeof event);
	event.events = channel->evnets();
	event.data.ptr = channel;
	
	int fd = channel->fd();
	
	::epoll_ctl(epollfd_, operation, fd, &event);

}















