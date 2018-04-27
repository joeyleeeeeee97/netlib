CXXFLAGS=-g -Wall -rdynamic -march=native
CXXFLAGS+=-O2
HEADERS=$(wildcard *.h)
TESTS = test1 \
	test2 \
	test3 \
	test4 \
	test5 \
	test6 \
	test7 \
	test8 

all : $(TESTS)
$(TESTS): $(HEADERS)

$(TESTS):
	g++ $(CXXFLAGS) -o $@ $(filter %.cc,$^) $(LDFLAGS) -lpthread -std=c++11

test1 : test/test1.cc EventLoop.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc TimerQueue.cc Timer.cc

test2 : test/test2.cc EventLoop.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc

test3 : test/test3.cc EventLoop.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc

test4 : test/test4.cc EventLoop.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc

test5 : test/test5.cc EventLoop.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc

test6 : test/test6.cc EventLoop.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc EventLoopThread.cc

test7 : test/test7.cc Socket.cc Acceptor.cc EventLoop.cc InetAddress.cc SocketsOps.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc

test8 : test/test8.cc Socket.cc Acceptor.cc EventLoop.cc InetAddress.cc SocketsOps.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc TcpConnection.cc TcpServer.cc

clean:
	rm -f $(TESTS)
