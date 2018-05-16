#include "MemSession.h"
#include "Memcached.h"
#include "Buffer.h"
#include "TcpConnection.h"
#include <functional>
#include <memory>
#include <sstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>

using namespace netlib;

void Session::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time) {
	while(buf->readableBytes() > 0) {
		const char* crlf = buf->findCRLF(buf->peek());
		if(!crlf) {
			conn->shutdown();
			break;
		}
		int len = static_cast<int>(crlf - buf->peek());
		std::string line(buf->peek(), len), command;
		buf->retrieveUntill(crlf + 2);	
		std::istringstream ss(line);
		std::vector<std::string> vals;

		std::copy(istream_iterator<std::string>(ss),
			 istream_iterator<std::string>(),
			 std::back_inserter(vals));

		for(auto it = vals.begin(); it != vals.end(); it++) {
			if(*it == "GET") {
				if(vals.end() - it < 2) {
					conn->shutdown();
					return ;
				}
				std::string key = *(it+1);
				auto res = cache->get(key);	
				if(res.first) {
					conn->send(res.second->val);
				}			
				else {
					conn->send("NOT FOUND!");
				}
			}
			else if (*it == "SET") {	
				if(vals.end() - it < 3) {
					conn->shutdown();
					return ;
				}
				std::string key  = *(it + 1), val = *(it + 2);
				cache->put(key, val);
				conn->send("UPDATE! ADD");
			}

		}
				

	}
	conn->shutdown();
}


