#ifndef _CHANNEL_H
#define _CHANNEL_H
#include <functional>
#include "nocopyable.h"
namespace netlib {
	class EventLoop;

	class Channel : nocopyable {
	public:	
		typedef std::function<void()> EventCallBack;

		Channel(EventLoop* _loop,int _fd);

		void setReadCallBack(EventCallBack& cb){
			readCallBack = cb;
		}

		void setWriteCallBack(EventCallBack& cb){
			writeCallBack = cb;
		}

		void setErrorCallBack(EventCallBack& cb){
			errorCallBack = cb;
		}
		void handleEvent();
		int fd() {
			return fd_;
		}
		int events(){return events_;}
		int index(){return index_;}
		void set_index(int idx){ index_ = idx;}
		EventLoop* ownerLoop() {
			return loop;
		}


		void set_revents(int val){revents = val;}
		bool isNoneEvent(){
			return events_ == kNoneEvent;
		}

		void enableReading() {
			events_ |= kReadEvent;
			update();
		}
	
	private:
		void update();

		static const int kNoneEvent;
		static const int kReadEvent;
		static const int kWriteEvent;
		
		EventLoop* loop;
		const int fd_;
		int events_;
		int revents;
		int index_;
	
		EventCallBack readCallBack;
		EventCallBack writeCallBack;
		EventCallBack errorCallBack;
	};

}



#endif
