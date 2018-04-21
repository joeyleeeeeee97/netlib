CXXFLAGS=-g -Wall -rdynamic -march=native
CXXFLAGS+=-O2
HEADERS=$(wildcard *.h)
TESTS = test1 \
	test2 

all : $(TESTS)
$(TESTS): $(HEADERS)

$(TESTS):
	g++ $(CXXFLAGS) -o $@ $(filter %.cc,$^) $(LDFLAGS) -lpthread -std=c++11

test1 : test/test1.cc EventLoop.cc ThreadFunc.cc

test2 : test/test2.cc EventLoop.cc ThreadFunc.cc

clean:
	rm -f $(TESTS)
