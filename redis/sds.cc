#include"sds.h"
#include"dataBase.h"
#include"funcTable.h"
#include<memory>


using namespace netlib;

StringPiece::StringPiece(const char* str){
	size_t len = strlen(str);
	data_ = static_cast<char*>(std::malloc(len));
	strcpy(data_, str);
	size_ = cap_ = len;
}

StringPiece::StringPiece(const StringPiece& rhs){
	data_ = static_cast<char*>(std::malloc(rhs.size()));
	strcpy(data_, rhs.data());
	size_ = cap_ = rhs.size();
	
}

StringPiece::StrignPiece(const std::string& rhs):this(rhs.c_str()){}


void StringPiece::append(const StringPiece& rhs){
	size_t size = size_ + rhs.size();
	size_t cap = cap_;

	while(cap < size) cap = cap << 1;	

	char* data = static_cast<char*>(std::malloc(cap));
	
	std::strcpy(data, data_);// , size_);
	std::strcpy(data + size_, rhs.data());//, rhs.size());
	
	data_ = data;
	size_ += rhs.size();
	cap_ = cap;
}

dataBase StringPiece::makeStringPiece(const char* data){
	StringPiece* sp = new StringPiece(data);
	return dataBase(sp, dataBase::dataType::SDS); 

}
