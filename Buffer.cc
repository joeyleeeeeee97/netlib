#include "Buffer.h"
#include "SocketsOps.h"
#include <errno.h>
#include <memory.h>
#include <sys/uio.h>

using namespace netlib;

const char* Buffer::kCRLF = "\r\n";


ssize_t Buffer::readFd(int fd, int* saveErrno) {
	char extrabuf[65535];
	struct iovec vec[2];
	const size_t writable = writableBytes();
	
	vec[0].iov_base = begin() + writeIndex;
	vec[0].iov_len = writable;
	vec[1].iov_base = extrabuf;
	vec[1].iov_len = sizeof extrabuf;
	
	const size_t n = readv(fd, vec, 2);

	if(n < 0) {
		*saveErrno = errno;
	}
	else if ( n < writable) {
		writeIndex += n;
	}
	else {
		writeIndex = buffer_.size();
		append(extrabuf, n - writable);
	}
	
	return n;

}
