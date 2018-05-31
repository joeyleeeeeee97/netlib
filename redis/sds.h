#ifndef _SDS_H
#define _SDS_H

#include<cstring>
#include<string>
#include<memory>
#include<iostream>
//#include"dataBase.h"
#include"../Timestamp.h"
#include"../nocopyable.h"

namespace netlib {


class dataBase;

class StringPiece{
public:
	StringPiece(const char*);
	StringPiece(const StringPiece&);


	~StringPiece(){
		std::free(data_);

	}

	static dataBase makeStringPiece(const char*);


	size_t hash(){
//		std::cout<<size_<<std::endl;
		size_t len = size_;	
		size_t hash = 1315423911;  
   		size_t i    = 0;  
   		for(i = 0; i < len;i++)  
  		{  
      			hash ^= ((hash << 5) + (*data_ + i) + (hash >> 2));  
   		}  
   		return hash;  
	}

	bool operator==(const StringPiece& rhs){
		return strcmp(data_,rhs.data_) == 0;//memcmp(data_, rhs.data_, size);
	}

	void append(const StringPiece&);
		
	void clear(){
		std::free(data_);
		size_ = 0;
	}

	const char* data() const{
		return data_;
	}

	size_t size() const{
		return size_;
	}

	size_t size(){
		return size_;
	}

	size_t cap() const{
		return cap_;
	}

	size_t cap() {
		return cap_;
	}

private:
	char* data_;
	size_t size_;
	size_t cap_;

};

//typedef ihared_ptr<StringPiece> StringPtr;

}




#endif
