#include"../EventLoop.h"
#include"../ThreadFunc.h"
using namespace std;
void f(netlib::EventLoop* p){
	p->loop();
}
int main(){
	netlib::EventLoop el;
	netlib::ThreadFunc tf(f,&el);
	tf.start();
	el.loop();
	tf.join();
	return 0;
}
