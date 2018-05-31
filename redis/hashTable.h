#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include<vector>
#include<utility>
#include<cstddef>

namespace netlib{

class dataBase;
class StringPiece;
class dictEntry;

typedef dictEntry* entryPtr;
typedef std::vector<entryPtr> bucketVec;

class hashTable{

public:
	hashTable();
	hashTable(size_t);

	//@Param   dictEntry
	bool insert(const dataBase&);
	std::pair<bool, dataBase> get(const dataBase&);

	//@Param   StringPiece
	bool erase(const dataBase&);
	
	bool rehash_flag();

	size_t size(){ return size_; }

	size_t bucket_size();

	dictEntry* at(int pos);
	void transfer(dictEntry*);
private:
	bucketVec buckets;
	size_t size_;
	size_t used_;
	size_t size_mask_;
 	
};


}
#endif
