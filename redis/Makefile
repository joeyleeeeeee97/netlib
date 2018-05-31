CXXFLAGS=-g -Wall -rdynamic -march=native
CXXFLAGS+=-O2
HEADERS=$(wildcard *.h)
TESTS = test1 \
	test2 \
	test3 \

all : $(TESTS)
$(TESTS): $(HEADERS)

$(TESTS):
	g++ $(CXXFLAGS) -o $@ $(filter %.cc,$^) $(LDFLAGS) -lpthread -std=c++11

test1 : test/test1.cc sds.cc dataBase.cc funcTable.cc Timestamp.cc

test2 : test/test2.cc sds.cc dataBase.cc Timestamp.cc dict.cc hashTable.cc funcTable.cc dictEntry.cc dictData.cc

test3 : test/test3.cc skipList.cc 

clean:
	rm -f $(TESTS)
