#include"../EventLoop.h"
#include"../ThreadFunc.h"
#include<iostream>

using namespace std;

void f(){
	cout << netlib::curThreadId() << endl;
}

int main(){
	netlib::ThreadFunc tf(f);
	tf.start();
	f();
	tf.join();
	return 0;
}
