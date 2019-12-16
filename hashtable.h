#ifndef HASHTABLE_CPU_GPU
#define HASHTABLE_CPU_GPU
#include <iostream>
using namespace std;

//#include "../utils/simple_math.h"

template <typename Key>
struct KeyHasher{
	int operator()(const Key key) const {
		return (key.z*377771 + key.y*677 + key.x);	// 2.15, 64
	}
};


// HASH TABLE IMPLEMENTATION OF GRID

template <typename Key, typename Value>
class HashNode{
	public:
	Key key;
	Value value;
	int count;
	bool isEmpty;
	bool isDeleted;
	
	HashNode(){
		key = Key();		// Value-initialization for Key and Value. This will be 0 for built-in types and conbstructor for user defined types
		value = Value();
		count = 0;
		isEmpty = true;
		isDeleted = false;
	}
	
	void print(){
		if (!isEmpty) cout << "<" << key << ", " << value << "> " << " [" << count-1 << " collisions]" << endl;
		else cout << "-----------" << endl;
	}
};



template <typename Key>
int hash3D(Key key, int length){
	return (key*377771) % length;
//	return (key.z*377771 + key.y*677 + key.x) % length;	// 2.15, 64
//	return (key.z*377771 + key.y*133337 + key.x) % length; // 1.7, 25
//	return (key.z*497771 + key.y*133337 + key.x) % length; // 30, 173
//	return ((long long int)key.z*497771 + key.y*787 + key.x) % length; // 1.5, 28
}




template <typename Key, typename Value>
int hash_insert(Key key, Value value, HashNode<Key,Value>* ht, int length, int* final_id=NULL){
	cout << "Insert: <" << key << ", " << value << "> : trying ";
	size_t hash = hash3D(key, length);	// get the hash of the specifed key
	size_t id = hash;					// the location where this <key, value> will be stored equals hash, but will be incremented if not empty 
	cout << hash << "(hash), ";
	int count = 1;						// number of attemts required before empty slot was found (defaults to one, i.e. no increments were required)
	while (!ht[id].isEmpty){ 	
		id = (id+11)%length;			// increment slot (using double hashing) until empty slot found 
		++count;
		cout << id << ", ";
	}
	ht[id].value = value;				// once empty slot found, store the <key,value> in the slot, and 
	ht[id].key = key;
	ht[id].isEmpty = false;
	ht[hash].count = count; //max(counts[id], count);
	cout << " --> stored @ [" << id << "]" << endl;
	if (final_id != NULL) *final_id = id;
	return count;

}


template <typename Key, typename Value>
size_t hash_find(Key key, HashNode<Key,Value>* ht, int length, int* attempts=NULL){
	size_t hash = hash3D(key, length);
	int id = hash;
	int count = 1; 
	if (attempts != NULL) *attempts = count;
	while (!(ht[id].key == key && !ht[id].isEmpty)){ 	// start at expected locaiton and probe until key matches
		id = (id+11)%length;
		++count;
		if (attempts != NULL) *attempts = count;
		if (count > ht[hash].count) return -1;			// if probe attempts exceeds count, key is not in the table (because only 'count' no. of entries were ever stored for the same key)
	}
	return id;
}


template <typename Key, typename Value>
void hash_print(HashNode<Key,Value>* ht, int length){
	for (int i=0; i<length; ++i){
		cout << i << ":"; ht[i].print();
	}	
}

#endif






