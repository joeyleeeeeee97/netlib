#include"../dict.h"
#include"../hashTable.h"
#include"../sds.h"
#include"../dataBase.h"
#include"../dictEntry.h"
#include"../dictData.h"

#include<iostream>
//#include<unordered_map>

using namespace netlib;

int main(){
	dict d;
	auto bb = dictEntry::makeDictEntry("key0","val0");	
	d.insert(bb);

	bb = dictEntry::makeDictEntry("key1","val1");	
	d.insert(bb);

	bb = dictEntry::makeDictEntry("key2","val2");	
	d.insert(bb);

	bb = dictEntry::makeDictEntry("key3","val3");	
	d.insert(bb);


	if(d.rehashing()){
		std::cout<<"RH"<<std::endl;
	}
	
	bb = dictEntry::makeDictEntry("key3","val3");	

	
	if(!d.insert(bb)){
		std::cout<<"duplicate"<<std::endl;
	}
	
	if(d.rehashing()){
		std::cout<<"RH"<<std::endl;
	}
	
	bb = dictEntry::makeDictEntry("key4","val4");	
	d.insert(bb);

	
	dataBase tmp1 = StringPiece::makeStringPiece("key1");
	auto p1 = d.get(tmp1);
	if(p1.first) {
		auto sp = static_cast<Data*>(p1.second.data());
		std::cout<<sp->val.data()<<std::endl;
	}

	dataBase tmp2 = StringPiece::makeStringPiece("key2");
	auto p2 = d.get(tmp2);
	if(p1.first) {
	auto sp = static_cast<Data*>(p2.second.data());
	std::cout<<sp->val.data()<<std::endl;
	}

	dataBase tmp3 = StringPiece::makeStringPiece("key2");
	auto p3 = d.get(tmp3);
	if(p1.first) {
	auto sp = static_cast<Data*>(p3.second.data());
	std::cout<<sp->val.data()<<std::endl;
	}


}
