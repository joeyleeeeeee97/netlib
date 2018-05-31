#include"sds.h"
#include"dataBase.h"
#include"dictEntry.h"
#include"dictData.h"


using namespace netlib;

dictEntry::dictEntry(const dataBase& db):data(db), next(nullptr){}
dictEntry::dictEntry(const dictEntry& de):data(de.data), next(de.next){}




dataBase dictEntry::makeDictEntry(const StringPiece& k,const  StringPiece& v){
        return dataBase(new Data(k,v), dataBase::dataType::DICTENT);
}




size_t dictEntry::hash(){
	return data.hash();	
}


