#ifndef _SKIPLISTNODE_H
#define _SKIPLISTNODE_H

#include<vector>

namespace netlib{

typedef int keyType;
typedef int valType;

class skipListNode{
public:
//	skipListNode(int n, keyType k, valType v):next(n + 1,nullptr), key_(k), val_(v){}

	skipListNode(int n,skipListNode* ptr, keyType k, valType v):next(n + 1,ptr), key_(k), val_(v){}

	std::vector<skipListNode*> next;
	keyType key_;
	valType val_;

};




}




#endif
