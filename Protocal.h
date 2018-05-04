#pragma once

#include <string>
#include <stdint.h>
#include "Buffer.h"

//using namespace netlib;
namespace netlib{

struct Session {
	int32_t number;
	int32_t length;
} __attribute__((__packed__));

struct PayloadMessage {
	int32_t length;
	char* data;
};

struct Context {
	int count;
	int bytes;
	Session session;
	Buffer output;
	
	Context(): count(0), bytes(0){
		session.number = 0, session.length = 0;
	}
};

}
