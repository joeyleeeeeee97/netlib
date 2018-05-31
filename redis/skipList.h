#ifndef  _SKIPLIST_H
#define _SKIPLIST_H
#include"skipListNode.h"

namespace netlib{


class skipList {

public:
	skipList();
	~skipList();

	skipList(const skipList&) = delete;
	skipList& operator=(const skipList&) = delete;

	bool get(const keyType&,valType&);
	bool insert(const keyType&, const valType&);
	bool erase(const keyType&);
	
	int randomLevel();

	int size(){ return size_;}
	int size()const { return size_;}

private:
	
	static skipListNode NIL_;
	
	int size_;
	int level_;
	skipListNode* header_;

};


}








#endif
