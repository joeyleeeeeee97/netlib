#include"dict.h"
#include"hashTable.h"
#include"sds.h"
#include"dataBase.h"
#include"funcTable.h"
#include"dictEntry.h"
#include"dictData.h"

#include<utility>
#include<memory>
#include<iostream>


using namespace netlib;

dict::dict():rehash_index(-1),ht({nullptr,nullptr}){
}

dict::~dict(){}




bool dict::insert(const dataBase& db) {

	if(ht[0] == nullptr) {
		ht[0] = new hashTable();
	}
	// need to rehash
	if(!rehashing() && ht[0]->rehash_flag()) {
		rehash_index = 0;		
		if(ht[1] == nullptr) {
			ht[1] = new hashTable(2 * ht[0]->bucket_size());
		}
	}
	
	if(rehashing()) {
		rehash_step();
		return ht[1]->insert(db);
	}
	else {
		return ht[0]->insert(db);
	}
}


std::pair<bool,dataBase> dict::get(const dataBase& key){
	if(ht[0] == nullptr) {
		return {false ,dataBase() };
	}
	if(rehashing()) {
		//size_t index = key.hash();
		rehash_step();
	
		if(false == rehashing()){
			return ht[0]->get(key);
		}

		auto r1 = ht[0]->get(key), r2 = ht[1]->get(key);
		if(r1.first) {
			return r1;
		}
		else {
			return r2;
		}
	}
	else {
		return ht[0]->get(key);
	}
	
}


bool dict::erase(const dataBase& key) {
	if(ht[0] == nullptr){
		return false;
	}
	if(rehashing()){
		rehash_step();
		return ht[0]->erase(key) || ht[1]->erase(key);
	}
	else {
		return ht[0]->erase(key);
	}
	
}


void dict::rehash_step(){
	dictEntry* head = ht[0]->at(rehash_index);
	if(head != nullptr){
//		dictEntry* nxt = head->next;
		while(head != nullptr) {
			dictEntry* nxt = head->next;
			ht[1]->transfer(head);
			head = nxt;
		}	
	}
	rehash_index++;
	if(rehash_index == ht[0]->bucket_size()){
		delete ht[0];
		ht[0] = ht[1], ht[1] = nullptr;
		rehash_index = -1;
	}
}
