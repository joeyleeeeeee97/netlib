#include"sds.h"
#include"dataBase.h"
#include"hashTable.h"
#include"funcTable.h"
#include"dictData.h"
using namespace netlib;


void funcMap::destroy(dataBase& db) {
	if(db.datatype == dataBase::dataType::SDS) {
		StringPiece* data = static_cast<StringPiece*>(db.data());
		data->~StringPiece();
	}
	else if(db.datatype == dataBase::dataType::DICTENT) {
		Data* data = static_cast<Data*>(db.data());
		data->~Data();
	}

}

bool funcMap::equal(const dataBase& l,const dataBase& r){
	if(l.datatype != r.datatype){
		if(l.datatype == dataBase::dataType::SDS && 
			r.datatype == dataBase::dataType::DICTENT) {
			StringPiece* lhs = static_cast<StringPiece*>(l.data_);
			Data* rhs = static_cast<Data*>(r.data_);
			return  *lhs == rhs->key;
		}
		else if(l.datatype == dataBase::dataType::DICTENT &&
			r.datatype == dataBase::dataType::SDS) {
			Data* lhs = static_cast<Data*>(l.data_);
			StringPiece* rhs = static_cast<StringPiece*>(r.data_);
			return lhs->key == *rhs;		
		}	

		return false;
	}
	else {
		if(l.datatype == dataBase::dataType::SDS){
			StringPiece* lhs = static_cast<StringPiece*>(l.data_);
			StringPiece* rhs = static_cast<StringPiece*>(r.data_);
			return *lhs == *rhs;
		
		}
		else if(l.datatype == dataBase::dataType::DICTENT){
			Data* lhs = static_cast<Data*>(l.data_);
			Data* rhs = static_cast<Data*>(r.data_);
			return lhs->key == rhs->key;
		}
		
	}
}

size_t funcMap::hash(const dataBase& l){
	if(l.datatype == dataBase::dataType::SDS) {
		StringPiece* data = static_cast<StringPiece*>(l.data_);
		return data->hash();
	}
	else if(l.datatype == dataBase::dataType::DICTENT) {
		Data* data = static_cast<Data*>(l.data_);
		return data->key.hash();		
	}
}

size_t funcMap::hash(const dataBase& l) const{
	return const_cast<funcMap*>(this)->hash(l);
}




