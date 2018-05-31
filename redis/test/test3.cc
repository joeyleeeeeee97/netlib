#include"../skipListNode.h"
#include"../skipList.h"
#include<iostream>

using namespace netlib;


int main(){

	skipList sl;
	
	sl.insert(1,2);
	sl.insert(3,4);
	sl.insert(5,6);

	int ret = 0;

	sl.get(1,ret);
	std::cout<<ret<<std::endl;

	sl.get(3,ret);
	std::cout<<ret<<std::endl;

	sl.get(5,ret);
	std::cout<<ret<<std::endl;

}
