#ifndef _CHANNEL_H
#define _CHANNEL_H
#include <functional>
#include "nocopyable"

namespace netlib {

class EventLoop;

class Channel : nocopyable {
public:	
	typedef std::function<void()> EventCallBack;

	Channel(EventLoop* _loop,int _fd);
	void setReadCallBack(EventCallbBack& cb){
		readCallBack = cd;
	}

	void setWriteCallBack(EventCallBack& cb){
		writeCallBack = cb;
	}

	void setErrorCallBack(EventCallBack& cb){
		errorCallBack = cb;
	}

	int fd(){return fd;}
	int events(){return events;}
	int index(){return index;}
	void set_index(int idx){ index = idx;}
	EventLoop* ownerLoop() {
		return loop;
	}


	void set_revents(int val){revents = val;}
	bool isNoneEvent(){
		return events == kNoneEvent;
	}

	void enableReading() {
		events |= kReadEvent;
		update();
	}
	
private:
	void update();

	static const int kNoneEvent;
	static const int kReadEvent;
	static const int kWriteEvent;
	
	EventLoop* loop;
	const int fd;
	int events;
	int revents;
	int index;

	EventCallBack readCallBack, writeCallBack, errorCallBack;


}





}


#endif
