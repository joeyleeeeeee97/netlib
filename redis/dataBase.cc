#include"../Timestamp.h"
#include"funcTable.h"
#include"dataBase.h"

using namespace netlib;

bool dataBase::expired(){
	return !expired_.valid() || expired_ < Timestamp::now();
}


dataBase::~dataBase(){

	if(refcount == nullptr || *refcount == 0) {
		return ;
	}

	*refcount = *refcount - 1;

	if(*refcount == 0) {
		funcmap.destroy(const_cast<dataBase&>(*this));
		delete refcount;
	}	

}

bool dataBase::operator==(const dataBase& rhs) {
	return funcmap.equal(*this,rhs);
}

size_t dataBase::hash(){
	return funcmap.hash(*this);
}

size_t dataBase::hash() const {
	return funcmap.hash(*this);

}
