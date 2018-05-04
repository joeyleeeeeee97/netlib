#ifndef _BUFFER_H
#define _BUFFER_H

#include <vector>
#include <string>
#include <algorithm>
#include "SocketsOps.h"
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

namespace netlib {

class Buffer {
public:
	static const int kCheapPrepend = 8;
	static const int kInitialSize = 1024;
	
	Buffer(): buffer_(kCheapPrepend + kInitialSize), readIndex(kCheapPrepend), writeIndex(kCheapPrepend) {}
	
	void swap(Buffer& rhs){
		buffer_.swap(rhs.buffer_);
		std::swap(readIndex, rhs.readIndex);
		std::swap(writeIndex, rhs.writeIndex);
	}

	size_t readableBytes() const {
		return writeIndex - readIndex;
	}

	size_t writableBytes() const {
		return buffer_.size() - writeIndex;
	}
	
	void hasWritten(size_t len) {
		writeIndex += len;
	}

	size_t prependableBytes() const {
		return readIndex;
	} 

	char* beginWrite() {
		return begin() + writeIndex;
	}	
	
	const char* beginWrite() const {
		return cbegin() + writeIndex;
	}

	const char* peek() const {
		return cbegin() + readIndex;
	}

	int32_t peekInt32() {
		int b32 = 0;
		::memcpy(&b32, peek(), sizeof(int32_t));
//		return ntohl(b32);
		return sockets::networkToHost32(b32);
	}

	std::string peekAsString() {
		std::string str(peek(), readableBytes());
		return str;
	}
	
	void retrieveInt32(){
		retrieve(sizeof(int32_t));

	}

	int32_t readInt32(){
		int32_t be32 = peekInt32();
		retrieveInt32();
		return be32;
	}

	void appendInt32(int32_t x)
  	{
    		int32_t be32 = sockets::hostToNetwork32(x);
    		append(&be32, sizeof(int32_t));
  	}

	void retrieve(size_t len) {
		assert(len <= readableBytes());
		if(len == readableBytes()){
			retrieveAll();
		}
		else {
			readIndex += len;
		}
	}

	void retrieveUntill(const char* End) {
		assert(peek() <= End);
		assert(End <= beginWrite());
		retrieve(End - peek());
	}
	
	void retrieveAll() {
		readIndex = kCheapPrepend, writeIndex = kCheapPrepend;
	}	

	std::string retrieveAsString() {
		std::string str(peek(), readableBytes());
		retrieveAll();
		return str;
	}

	void append(const std::string& str) {
		append(str.data(), str.length());
	}	

	void append(const char* data, size_t len) {
		ensureWritableBytes(len);
		std::copy(data, data + len, beginWrite());
		writeIndex += len;//hasWritten(len);
		assert(writeIndex != readIndex);
		std::cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<writeIndex<<std::endl;
	}

	void append(const void* data, size_t len) {
		append(static_cast<const char*>(data), len);
	}

	void prepend(const char* data, size_t len) {
		assert(len <= prependableBytes());
//		char* dest = static_cast<char*>(peek()) - len;
//		char* pos = static_cast<char*>(data);
		std::copy(data, data + len, begin() + readIndex - len); 
		readIndex -= len;
	}

	void ensureWritableBytes(size_t len) {
		if(writableBytes() < len) {
			makeSpace(len);
		}
	}
	
	void shrink(size_t reserve) {
		std::vector<char> buf(kCheapPrepend + readableBytes() + reserve);
		std::copy(peek(), peek() + readableBytes(), buf.begin() + kCheapPrepend);
		buf.swap(buffer_);
	}

	ssize_t readFd(int fd, int* saveErrno);

private:

	char* begin() {
		return &*buffer_.begin();
	}
	
	const char* cbegin() const {
		return &*buffer_.begin();
	}

	void makeSpace(size_t len) {
		if( writableBytes() + prependableBytes() < len + kCheapPrepend) {
			buffer_.resize(writeIndex + len);
		}
		else {
			size_t rdble = readableBytes();
			std::copy(begin() + readIndex, begin() + writeIndex, begin() + kCheapPrepend);
			readIndex = kCheapPrepend;
			writeIndex = kCheapPrepend  + rdble;
		}

	}



	std::vector<char> buffer_;
	size_t readIndex;
	size_t writeIndex;

};




}
#endif
