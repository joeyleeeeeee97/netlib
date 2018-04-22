#include "Channel.h"
#include "EventLoop.h"
#include <sstream>
#include <poll.h>

using namespace netlib;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* _loop, int _fd) : loop(_loop), fd(_fd), events(0), revents(0), index(-1){}

void Channel::update(){
	loop->updateChannel(this);
} 

void Channel::handleEvent(){
  if (revents_ & POLLNVAL) {
    LOG_WARN << "Channel::handle_event() POLLNVAL";
  }

  if (revents & (POLLERR | POLLNVAL)) {
    if (errorCallback) errorCallback();
  }
  if (revents & (POLLIN | POLLPRI | POLLRDHUP)) {
    if (readCallback) readCallback();
  }
  if (revents & POLLOUT) {
    if (writeCallback) writeCallback();
  }
}
