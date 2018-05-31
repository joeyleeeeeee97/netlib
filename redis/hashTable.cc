#include"hashTable.h"
#include<vector>
#include<utility>
#include<cstddef>
#include"dictEntry.h"
#include"dictData.h"
#include"dataBase.h"
#include<iostream>

using namespace netlib;

#define INIT_SIZE 4



hashTable::hashTable():buckets(INIT_SIZE,nullptr),size_(0), used_(0), size_mask_(INIT_SIZE - 1){}

hashTable::hashTable(size_t n):buckets(n,nullptr), size_(0), used_(0), size_mask_(INIT_SIZE - 1){}

bool hashTable::insert(const dataBase& pair) {
	if(get(pair).first == true) return false;

	size_t idx = pair.hash()%buckets.size();
	if(buckets[idx] == nullptr) used_++;
	dictEntry* de = new dictEntry(pair);
	de->next = buckets[idx];
	buckets[idx] = de;
	size_++;	
	return true;	
}



std::pair<bool, dataBase>  hashTable::get(const dataBase& key) {
	
	size_t idx = key.hash()%buckets.size();
	dictEntry* head = buckets[idx];
	while(head != nullptr) {
		if(head->data == key) {
			return { true, head->data };
		}
		head = head->next;
	}
	return {false, dataBase()};

}


bool hashTable::erase(const dataBase& key) {
        size_t idx = key.hash()%buckets.size();
        dictEntry* head = buckets[idx];

	if(head == nullptr) return false;

	if(head->data == key) {
		buckets[idx] = head->next;	
		delete head;
		size_--,used_--;		
		return true;
	}

	dictEntry* next = head->next;
	
	while(head != nullptr) {
		if(next!=nullptr && next->data == key) {
			head->next = next->next;
			delete next;
			size_--;
			return true;
		}
		head = next;
		next = head->next;
	}
	
	return false;
}

bool hashTable::rehash_flag(){
	return size_ >= buckets.size();
}

size_t hashTable::bucket_size(){
	return buckets.size();
}

dictEntry* hashTable::at(int pos){
	if(pos < 0 || pos >= buckets.size()){
		return nullptr;
	}
	return buckets[pos];
}


void hashTable::transfer(dictEntry* de){
	int idx = de->hash()%buckets.size();
	if(buckets[idx] == nullptr) used_++;

	de->next = buckets[idx];
	buckets[idx] = de;
	size_++;
}

