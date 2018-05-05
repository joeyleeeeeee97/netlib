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
	test8 \
	test9 \
	test10 \
	test11\
	test12\
	test13\
	ttcp_client\
	ttcp_server\
	buf_test\
	netcat\
	memcached\
	mem_test\

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

test8 : test/test8.cc Socket.cc Acceptor.cc EventLoop.cc InetAddress.cc SocketsOps.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc TcpConnection.cc TcpServer.cc Buffer.cc

test9 : test/test9.cc Socket.cc Acceptor.cc EventLoop.cc InetAddress.cc SocketsOps.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc TcpConnection.cc TcpServer.cc Buffer.cc

test10 : test/test10.cc Socket.cc Acceptor.cc EventLoop.cc InetAddress.cc SocketsOps.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc TcpConnection.cc TcpServer.cc Buffer.cc

test11 : test/test11.cc Socket.cc Acceptor.cc EventLoop.cc InetAddress.cc SocketsOps.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc TcpConnection.cc TcpServer.cc Buffer.cc

test12 : test/test12.cc Socket.cc Acceptor.cc EventLoop.cc InetAddress.cc SocketsOps.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc TcpConnection.cc TcpServer.cc Buffer.cc Connector.cc

test13 : test/test13.cc Socket.cc Acceptor.cc EventLoop.cc InetAddress.cc SocketsOps.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc TcpConnection.cc TcpServer.cc Buffer.cc Connector.cc TcpClient.cc

ttcp_client : test/ttcp_client.cc Socket.cc Acceptor.cc EventLoop.cc InetAddress.cc SocketsOps.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc TcpConnection.cc TcpServer.cc Buffer.cc Connector.cc TcpClient.cc

ttcp_server : test/ttcp_server.cc Socket.cc Acceptor.cc EventLoop.cc InetAddress.cc SocketsOps.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc TcpConnection.cc TcpServer.cc Buffer.cc

buf_test : test/Buffer_test.cc Buffer.cc

netcat : test/netcat.cc Socket.cc Acceptor.cc EventLoop.cc InetAddress.cc SocketsOps.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc TcpConnection.cc TcpServer.cc Buffer.cc TcpClient.cc Connector.cc 

memcached : test/mem_server.cc Socket.cc Acceptor.cc EventLoop.cc InetAddress.cc SocketsOps.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc TcpConnection.cc TcpServer.cc Buffer.cc MemSession.cc

mem_test : test/mem_client.cc Socket.cc Acceptor.cc EventLoop.cc InetAddress.cc SocketsOps.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc TcpConnection.cc TcpClient.cc Buffer.cc MemSession.cc Connector.cc

clean:
	rm -f $(TESTS)
