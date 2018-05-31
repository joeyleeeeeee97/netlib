#ifndef _DATA_BASE_H
#define _DATA_BASE_H

#include<functional>
#include<memory>
#include<stdint.h>
#include"../Timestamp.h"
#include"funcTable.h"

namespace netlib{

class funcMap;


class dataBase{
public:

	enum dataType{ SDS, LIST, DICT, SKIPLIST, INTSET, ZIPLIST, DICTENT, NOTYPE };
	//NOTYPE 
	dataBase():data_(nullptr),refcount(new size_t(1)),expired_(0),datatype(NOTYPE){}	
	dataBase(void* data, dataType dt, Timestamp t):data_(data),datatype(dt), expired_(t), refcount(new size_t(1)){}
	dataBase(void* data, dataType dt):data_(data),datatype(dt), expired_(0), refcount(new size_t(1)){}	

	dataBase(const dataBase& rhs):datatype(rhs.datatype),data_(rhs.data_),expired_(rhs.expired_),refcount(rhs.refcount){
		*refcount = *refcount + 1;
	}
	
	dataBase& operator=(const dataBase& rhs) {
		this->~dataBase();
		refcount = rhs.refcount, data_ = rhs.data_, datatype = rhs.datatype;
		*refcount = *refcount + 1;
	}

	bool operator==(const dataBase& rhs);
	size_t hash();
	size_t hash() const;	
//	virtual dataBase() = 0;
	~dataBase();

	bool expired();
	

	void* data(){
		return data_;
	}	

	void* data() const {
		return data_;
	}
	
	funcMap funcmap;
	size_t* refcount;
	dataType datatype;
	Timestamp expired_;
	void* data_;
	

};

typedef std::shared_ptr<dataBase> dataPtr;
//typedef std::function<bool(const dataBase&,const dataBase&)> equalFunc;
//typedef std::function<size_t(const dataBase&,const dataBase&)> hashFunc;



}

#endif
