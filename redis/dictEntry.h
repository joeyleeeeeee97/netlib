#ifndef _DICT_ENTRY_H
#define _DICT_ENTRY_H

#include<memory>
#include"dataBase.h"
#include"sds.h"

namespace netlib{

class dataBase;
class StringPiece;

class dictEntry{
public:
        dictEntry(const dataBase&);
        dictEntry(const dictEntry&);

        size_t hash();
        struct dictEntry* next;
        dataBase data;
        
	static dataBase makeDictEntry(const StringPiece& key, const StringPiece& val);
};

//typedef dictEntry* entryPtr;
//typedef std::vector<entryPtr> bucketVec;


}

#endif
