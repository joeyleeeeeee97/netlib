#include"skipList.h"
#include"skipListNode.h"
#include<vector>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LEVEL 64

using namespace netlib;

skipListNode skipList::NIL_(1,&skipList::NIL_,0x7fffffff,0);


skipList::skipList():level_(0), header_(new skipListNode(MAX_LEVEL,&skipList::NIL_, 0x7fffffff,0)), size_(0){
	srand(time(0));
}

skipList::~skipList(){
	skipListNode *p = header_, *nxt;
	while(p != &NIL_) {
		nxt = p->next[0];
		delete p;
		p = nxt;
	}

}


bool skipList::get(const keyType& k, valType& v){
	skipListNode* p = header_;
	
	for(int i = level_; i >= 0; i--){
		while(p->next[i]->key_ < k) {
			p = p->next[i];
		}
	}

	p = p->next[0];
	if(p->key_ == k) {
		v = p->val_;
		return true;
	}
	else {
		return false;
	}
}


bool skipList::insert(const keyType& k, const valType& v){
	skipListNode* update[MAX_LEVEL];
	skipListNode* p = header_;

	for(int i = level_;i >= 0; i--){
		while(p->next[i]->key_ < k) {
			p = p->next[i];
		}
		update[i] = p;
	}
//	p = p->next[0];
	
	if(p->key_ == k ) {
		p->val_ = v;
		return false;
	}
	else {
		int level = randomLevel();

		if(level > level_) {
			level = ++level_;
			update[level] = header_;
		}
		
		skipListNode* newNode = new skipListNode(level,&NIL_, k, v);

		for(int i = level; i >= 0; i--) {
			p = update[i];
			newNode->next[i] = p->next[i];
			p->next[i] = newNode;
		}

	}
	size_++;
	return true;
}


bool skipList::erase(const keyType& k){
	
	skipListNode* update[MAX_LEVEL];
        skipListNode* p = header_;

        for(int i = level_;i >= 0; i--){
                while(p->next[i]->key_ < k) {
                        p = p->next[i];
                }

		update[i] = p;
        }
        p = p->next[0];

        if(p->key_ != k ) {
		return false;
	}
	else {
		for(int i = 0; i < level_; i++) {
			if(update[i]->next[i] != p) {
				break;
			}
			update[i]->next[i] = p->next[i];
		}
		
		delete p;
		--size_;
		while(level_ > 0 && header_->next[level_] == nullptr) {
			--level_;
		}	
		return true;
	}
}
	

int skipList::randomLevel() {
	int level = 1;
	while(rand() % 2) level++;
	if( level > MAX_LEVEL) { level = MAX_LEVEL; }
	return level;
	

}

