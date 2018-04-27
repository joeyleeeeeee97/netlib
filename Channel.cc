#include "Channel.h"
#include "EventLoop.h"
#include <sstream>
#include <poll.h>

using namespace netlib;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* _loop, int _fd)
 : loop(_loop), fd_(_fd), events_(0), revents(0), index_(-1), eventHandling(false){

}

Channel::~Channel() {
	assert(!eventHandling);
}

void Channel::update(){
	loop->updateChannel(this);
} 

void Channel::handleEvent(){
	eventHandling = true; 
	if (revents & (POLLERR | POLLNVAL)) {
		if (errorCallBack) errorCallBack();
	}
	if (revents & (POLLIN | POLLPRI | POLLRDHUP)) {
		if (readCallBack) readCallBack();
	}
	if (revents & POLLOUT) {
		if (writeCallBack) writeCallBack();
	}
	if( (revents & POLLHUP) && !(revents & POLLIN) ) {
		if(closeCallBack) closeCallBack();
	}
	eventHandling = false;
}
