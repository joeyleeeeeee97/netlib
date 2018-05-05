#ifndef _CACHED_H
#define _CACHED_H


#include<iostream>
#include<mutex>
#include<thread>
#include<memory>
#include<unordered_map>
#include<set>
#include <iterator>

namespace netlib {
template<typename K, typename V>
struct Data {
	explicit Data(const K& k, const V& v) :key(k), val(v) {

	}
	K key;
	V val;
};

using namespace std;
template<typename K, typename V>
struct Item {
	typedef Item<K, V>* Itemptr;
	typedef shared_ptr<const Data<K, V>> MData;
	//for head node
	Item() {}

	explicit Item(const K& k, const V& v) : nxt(nullptr), pre(nullptr) {
		data = make_shared<Data<K, V>>(k, v);
	}
	//this should be a light weighted copy method since all its elems are ptr_type
	Item(const Item& rhs) = default;
	Item& operator=(const Item& rhs) = default;
	//删除该元素
	void detachFromList() {
		Itemptr this_pre = pre, this_nxt = nxt;
		this_pre->nxt = this_nxt;
		this_nxt->pre = this_pre;
		pre = nxt = nullptr; //In case this Item is reused
	}
	//加到该节点后面
	void appendAftHead(Itemptr head) {
		head->nxt->pre = this;
		nxt = head->nxt;
		head->nxt = this;
		pre = head;
	}
	//for light copy and concurency
	shared_ptr<const Data<K, V>> data;
	Itemptr nxt;
	Itemptr pre;
};

template<class T>
struct Hash {
	size_t operator()(const T&) const;
};

template<class T>
struct Equal {
	bool operator()(const T& lhs, const T& rhs) const;
};

template<>
struct Hash<string> {
	size_t operator()(const string& rhs) const {
		size_t n = 0;
		for (auto i : rhs) {
			n += i;
		}
		return n % 6537;
	}
};

template<>
struct Equal<string> {
	bool operator()(const string& lhs,const string& rhs) const {
		return lhs == rhs;
	}
};

template<class K, class V>
class Cache {
public:
	typedef Item<K, V> MItem;
	typedef shared_ptr<const Data<K, V>> MData;
	typedef shared_ptr<const Data<K, V>> Dataptr;
	typedef unordered_map<K, MItem, Hash<K>, Equal<K>> Table;
	//对头节点初始化
	explicit Cache(size_t capacity) :table(), head(), siz(0), cap(capacity) {
		head.nxt = &head, head.pre = &head;
	}

	//禁止拷贝
	Cache(const Cache&) = delete;
	Cache& operator=(const Cache&) = delete;

	std::pair<bool, Dataptr> get(const K& key) {
		std::lock_guard<std::mutex> lk(mtx);
		auto it = table.find(key);
		if (it != table.end()) {
			auto val = it->second.data->val;
			it->second.detachFromList();
			it->second.appendAftHead(&head);//调整到LRU首端
			return { true, it->second.data };
		}
		else {
			return { false, Dataptr() };
		}
	}
	void put(const K& key, const V& val) {
		std::lock_guard<std::mutex> lk(mtx);
		auto it = table.find(key);
		if (it != table.end()) {
			it->second.detachFromList();
			table.erase(it);
			auto p = table.insert({ key, MItem(key, val) });
			p.first->second.appendAftHead(&head);
		}
		else {
			if (siz == cap) {
				deleteLru();
			}
			auto p = table.insert({ key, MItem(key, val) });            //insert
			p.first->second.appendAftHead(&head);
			siz++;
		}
	}

	bool del(const K& key) {
		auto it = table.find(key);
		if (it == table.end()) {
			return false;
		}
		else {
			it->second.detachFromList();
			table.erase(it);
			siz--;
			return true;
		}
	}
private:
	//delete least recently used item
	void deleteLru() {
		MItem* lru = head.pre;
		if (lru != &head) {
			del(lru->data->key);
		}
	}
	size_t cap;
	size_t siz;
	Table table;
	MItem head;
	std::mutex mtx;
};
}


#endif
