#ifndef _DICT_H
#define _DICT_H

//#include"dataBase.h"
//#include"sds.h"
//#include"hashTable.h"
//#include"funcTable.h"
#include<memory>
#include <utility>

namespace netlib{

class dataBase;
class StringPiece;
class hashTable;

class dict{
public:
	dict();
	~dict();
	
	dict(const dict&) = delete;
	dict& operator=(const dict&) = delete;
		
	std::pair<bool,dataBase> get(const dataBase&);


	bool insert(const dataBase&);
	bool erase(const dataBase&);


	void rehash_step();

	bool rehashing(){
		return rehash_index != -1;
	}
	

private:
	typedef hashTable* tablePtr;

	tablePtr ht[2];
	int rehash_index;
	

};





}

#endif
