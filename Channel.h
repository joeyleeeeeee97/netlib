#ifndef _CHANNEL_H
#define _CHANNEL_H
#include <functional>
#include "nocopyable.h"
#include "Timestamp.h"

namespace netlib {
	class EventLoop;

	class Channel : nocopyable {
	public:	
		typedef std::function<void()> EventCallBack;
		typedef std::function<void(Timestamp)> ReadEventCallBack;

		Channel(EventLoop* _loop,int _fd);
		~Channel();

		void setReadCallBack(ReadEventCallBack& cb){
			readCallBack = cb;
		}

		void setWriteCallBack(EventCallBack& cb){
			writeCallBack = cb;
		}

		void setErrorCallBack(EventCallBack& cb){
			errorCallBack = cb;
		}

		void setCloseCallBack(const EventCallBack& cb) {
			closeCallBack = cb;
		}

		void handleEvent(Timestamp receiveTime);
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
		void disableAll() {
			events_ = kNoneEvent;
			update();
		}


		void enableWriting() {
			events_ |= kWriteEvent;
			update();
		}	

		void disableWriting() {
			events_ &= ~~kWriteEvent;
			update();
		}

		bool isWriting() const {
			return events_ & kWriteEvent;
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

		bool eventHandling;	

		ReadEventCallBack readCallBack;
		EventCallBack writeCallBack;
		EventCallBack errorCallBack;
		EventCallBack closeCallBack;
	};

}



#endif
