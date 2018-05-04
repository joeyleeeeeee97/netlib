#include"../Buffer.h"
#include <assert.h>
#include <stdio.h>

using namespace netlib;

struct x{
	int32_t a,b;
} __attribute__((__packed__));// __attribute__( __packed__ );



int main(){

	Buffer buf;
	x X = {1,2};
	buf.append(&X, sizeof(x));
	buf.readInt32();
	buf.readInt32();

	buf.appendInt32(10086);
	
	printf("%d\n", buf.readableBytes());
	assert(buf.readableBytes() != 0);

}
