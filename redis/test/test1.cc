#include"../dataBase.h"
#include"../sds.h"
#include<iostream>

using namespace netlib;


int main(){

	dataBase db = StringPiece::makeStringPiece("我是asdsf");
	dataBase rh = db;
	dataBase rsdf = rh;

	dataBase a = StringPiece::makeStringPiece("gggg");
	
	if(a == db){
		std::cout<<"Equal"<<std::endl;
	}

	std::cout<<db.hash()<<std::endl;

}
