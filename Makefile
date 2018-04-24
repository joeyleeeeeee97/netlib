CXXFLAGS=-g -Wall -rdynamic -march=native
CXXFLAGS+=-O2
HEADERS=$(wildcard *.h)
TESTS = test1 \
	test2 \
	test3 

all : $(TESTS)
$(TESTS): $(HEADERS)

$(TESTS):
	g++ $(CXXFLAGS) -o $@ $(filter %.cc,$^) $(LDFLAGS) -lpthread -std=c++11

test1 : test/test1.cc EventLoop.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc TimerQueue.cc Timer.cc

test2 : test/test2.cc EventLoop.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc


test3 : test/test3.cc EventLoop.cc ThreadFunc.cc Channel.cc Poller.cc Timestamp.cc  TimerQueue.cc Timer.cc

clean:
	rm -f $(TESTS)
