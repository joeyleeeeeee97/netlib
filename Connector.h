#ifndef _CONNECTOR_H
#define _CONNECTOR_H

#include "InetAddress.h"
#include "TimerId.h"
#include "nocopyable.h"

#include <functional>
#include <memory>

namespace netlib{

class Channel;
class EventLoop;

class Connector : nocopyable{
public:
	typedef std::function<void (int)> NewConnectionCallback;

	Connector(EventLoop*,const InetAddress&);
	~Connector();

	void setNewConnectionCallback(const NewConnectionCallback& cb) {
		newConnectionCallback = cb;
	}

	void start();
	void restart();
	void stop();

	const InetAddress& serverAddress() const {
		return serverAddr_;
	}
private:
	enum States {
		kDisconnected,
		kConnecting,
		kConnected
	};

	static const int kMaxRetryDelayMs = 30 * 1000;
	static const int kInitRetryDelayMs = 500;

	void setState(States s ) {
		state_ = s;
	}
	void startInLoop();
	void connect();
	void connecting(int sockfd);
	void handleWrite();
	void handleError();
	void retry(int);
	int removeAndResetChannel();
	void resetChannel();
	
	EventLoop* loop_;
	InetAddress serverAddr_;
	bool connect_;
	States state_;
	std::unique_ptr<Channel> channel_;
	NewConnectionCallback newConnectionCallback;
	int retryDelayMs_;
	TimerId timerId_;
};
typedef std::shared_ptr<Connector> ConnectorPtr;

}









#endif
