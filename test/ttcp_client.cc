#include"../Protocal.h"
#include"../TcpClient.h"
#include"../EventLoop.h"
#include"../Timestamp.h"
#include"../Buffer.h"
#include<functional>
#include<boost/any.hpp>
#include<iostream>
#define L 10086

using namespace netlib;

EventLoop* g_loop;

void onConnection(const TcpConnectionPtr& conn) {
	if(conn->connected()){
		printf("onConnection(): new connection [%s] from %s\n",
           	conn->name().c_str(),
           	conn->peerAddress().toHostPort().c_str());
    		//conn->send(message);

		Context context;
		context.count = 1;
		context.bytes = L;
		context.session.number = 8888;
		context.session.length = L;

		context.output.appendInt32(L);
		context.output.ensureWritableBytes(L);

		for(int i = 0; i < L; i++ ){
			context.output.beginWrite()[i] = "0123456789ABCDEF"[i % 16];
		}

		context.output.hasWritten(L);
		conn->setContext(context);

		Session session_ = {0, 0};
		session_.number = htonl(8888);
		session_.length = htonl(L);
		conn->send(&session_, sizeof(Session));

		conn->send(context.output.peekAsString());//.toStringPiece());

	}
	else {
		printf("onConnection(): connection [%s] is down\n",
		conn->name().c_str());
		g_loop->quitloop();
	}	

}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time) {
	Context* context = boost::any_cast<Context>(conn->getMutableContext());
	
	while(buf->readableBytes() >= sizeof (int32_t)) {
		int32_t len = buf->readInt32();
		if(len == context->session.length && context->count < context->session.number){
//			std::cout<<context->output.readableBytes()<<std::endl;
			conn->send(context->output.peekAsString());//Piece());
			++context->count;
			context->bytes += len;
//			printf("send %d bytes and %d count\n", context->bytes, context->count);
		}
		else {
			conn->shutdown();
			break;
		}
	}

}

int main(){

	InetAddress addr(9981);
	Timestamp start(Timestamp::now());
	EventLoop loop;
	TcpClient client(&loop, addr);
	g_loop = &loop;
	client.setConnectionCallback(onConnection);
	client.setMessageCallback(onMessage);

	client.connect();
	loop.loop();
	
	double elapsed = timeDifference(Timestamp::now(), start);
	double total_mb = 1.0 * 10086 * 8888 / 1024 / 1024;
	printf("%.3f MiB transferred\n%.3f MiB/s\n", total_mb, total_mb / elapsed);
}


