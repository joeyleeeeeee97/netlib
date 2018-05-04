#include "../TcpServer.h"
#include "../EventLoop.h"
#include "../Buffer.h"
#include "../Protocal.h"
#include <iostream>
#include <functional>

using namespace netlib;

void OnConnection(const TcpConnectionPtr& conn) {
	if(conn->connected()){
		Context context;
		conn->setContext(context);
	}
	else {
		const Context& context = boost::any_cast<Context>(conn->getContext());
		conn->getLoop()->quitloop();
	}
}

void OnMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time) {
	while(buf->readableBytes() >= sizeof(int32_t)) {
//		std::cout<<"	ON MESS		" << std::endl;
		Context* context = boost::any_cast<Context>(conn->getMutableContext());
		Session& session = context->session;
		if(session.number == 0 && session.length == 0) {
			if(buf->readableBytes() >= sizeof(Session)) {
				std::cout<<"received session " << buf->readableBytes() <<std::endl;

				session.number = buf->readInt32();
				std::cout<<session.number<< "         " << buf->readableBytes()<<std::endl;
				session.length = buf->readInt32();
				std::cout<<session.length<< "         " << buf->readableBytes()<<std::endl;

				context->output.appendInt32(session.length);

				std::cout<<context->output.readableBytes() << "         \B";// << context->output.retrieveAsString() << std::endl;
				std::cout<<"received number = " << session.number << " received length = "<< session.length << "\n";
				
				
			}
			else { break; }
		}
		else {
//			std::cout<<session.length <<"      " <<session.number << std::endl; 
			const unsigned total_len = session.length + static_cast<int>(sizeof(int32_t));
			const int32_t length = buf->peekInt32();
//			std::cout<<"Data pack "<< length<<std::endl;
			if(length == session.length){
				if(buf->readableBytes() >= total_len) {
					buf->retrieve(total_len);
//					std::cout<<"Server : send " << context->output.readableBytes()<<std::endl;
					conn->send(context->output.peekAsString());
					++context->count;
					context->bytes += length;
//					std::cout <<" bytes : " << context->bytes << " count : " << context->count << std::endl;
					if(context->count >= session.number) {
						conn->shutdown();
						break;
					}
				}
				else { break; } 
			}
			else {
				std::cout<<"DONE!\n";	
				conn->shutdown();
				break;
			}

		
		
		}

	}

}

int main(){
	InetAddress serverAddr(9981);
	EventLoop loop;

	Timestamp start = Timestamp::now();
	TcpServer server(&loop, serverAddr);
	
	server.setConnectionCallback(OnConnection);
	server.setMessageCallback(OnMessage);
	server.start();
	loop.loop();


	 double elapsed = timeDifference(Timestamp::now(), start);
        double total_mb = 1.0 * 10086 * 8888 / 1024 / 1024;
        printf("%.3f MiB transferred\n%.3f MiB/s\n", total_mb, total_mb / elapsed);
}
