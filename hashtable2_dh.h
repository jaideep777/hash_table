#ifndef HASHTABLE_CPU_GPU
#define HASHTABLE_CPU_GPU
#include <iostream>
#include <cassert>
using namespace std;

//#include "../utils/simple_math.h"

template <typename Key>
struct KeyHasher{
	int operator()(const Key key) const {
		return (key.z*377771 + key.y*677 + key.x);	// 2.15, 64
	}
};


// NOTE: Using count to keep track of number of collisions happening at that slot will only work with fixed increments of collision resolution. 
// It will NOT work with double hashing, since the number of collisions will depend on the key! But can still keep track of max number of collisions ever recorded there

// HASH TABLE IMPLEMENTATION 

template <typename Key, typename Value>
class HashNode{
	public:
	Key key;
	Value value;
	int count;
	bool isEmpty;
	bool isDeleted;
	
	HashNode(){
		key = Key();		// Value-initialization for Key and Value. This will be 0 for built-in types and constructor for user defined types
		value = Value();
		count = 0;			// number of collisions that have happend at this node (the hashtable is just an array of HashNodes, so node=slot)
		isEmpty = true;		
		isDeleted = false;	
	}

	void print(){
		if (isDeleted) cout << "---<" << key << "," << value << ">--- " << " [" << count << " collisions]" << endl;
		else if (!isEmpty) cout << "<" << key << ", " << value << "> " << " [" << count << " collisions]" << endl;
		else cout << "-----------" << endl;
	}
};


template <typename Key>
int hash3D(Key key, int length){
	return ((unsigned long long)key*377771) % length;
//	return ((unsigned long long)key*677) % length;
//	return (key.z*377771 + key.y*677 + key.x) % length;	// 2.15, 64
//	return (key.z*377771 + key.y*133337 + key.x) % length; // 1.7, 25
//	return (key.z*497771 + key.y*133337 + key.x) % length; // 30, 173
//	return ((long long int)key.z*497771 + key.y*787 + key.x) % length; // 1.5, 28
}


template <typename Key>
int hash2(Key key){
	return 1;
	//return 2*((key*1)%50)+1;	// should return an odd number, as table size is 2^m
}


//template <typename Key>
//int increment(Key key, int count){
////	return (id+11)%length;
//	return count*hash2(key);
////	return (key)%677;
//}


template <typename Key, typename Value>
size_t hash_find(Key key, HashNode<Key,Value>* ht, int length, int* attempts=NULL){
	size_t hash = hash3D(key, length);
	int id = hash;
	int count = 0; 
	if (attempts == NULL) attempts = new int; // DEBUG ONLY
	if (attempts != NULL) *attempts = count+1;
	cout << "Find: (" << key << "):" << id << "*, ";
	while (!(ht[id].key == key && !ht[id].isEmpty)){ 	// start at expected location and probe until: slot is filled AND the key matches. (i.e., dont stop at empty slot)
		++count;
		assert(count <= length);
		id = (hash + count*hash2(key))%length;
		cout << id << ", ";
		if (count > ht[hash].count) {id = -1; break;} 	// if probe attempts exceeds count, key is not in the table (because only 'count' no. of entries were ever stored for the same key)
	}
	if (attempts != NULL) *attempts = count+1;
	
	// DEBUG ONLY
	if (id >= 0) cout << "<" << key << ", " << ht[id].value << "> [" << *attempts << " tries]" << ((key != ht[id].value*10)? " * FAIL *":"") << endl;
	else cout << "<" << key << ", " << "- " << "> [" << *attempts << " tries]" << endl;
	if (attempts == NULL) delete attempts;
	
	return id;
}


template <typename Key, typename Value>
int hash_insert(Key key, Value value, HashNode<Key,Value>* ht, int length, int* final_id=NULL){
	cout << "Insert: <" << key << ", " << value << "> : trying ";
	size_t hash = hash3D(key, length);	// get the hash of the specifed key
	size_t id = hash;					// the location where this <key, value> will be stored equals hash, but will be incremented if not empty 
	cout << hash << "*, ";
	int count = 0;						// number of collisions before empty slot was found (defaults to 0, i.e. no increments were required)
	while (!ht[id].isEmpty){ 	
		++count;						// counter is not incremented if inserting into previously deleted slot, because keys that have been mapped past the deleted slot stay intact
		assert(count <= length);
		id = (hash + count*hash2(key))%length;			// increment slot (using double hashing) until empty or deleted slot found 
		cout << id << ", ";
	}
	ht[id].value = value;				// once empty slot found, store the <key,value> in the slot
	ht[id].key = key;
	ht[id].isEmpty = false;
	ht[id].isDeleted = false;
	ht[hash].count = max(ht[hash].count, count);	// when insertion happens in a deleted slot, we dont want to lose count of how many valid entries existed beyond the deleted slot. Hence, count must be maximum of previous count and result of current insertion
	cout << " --> stored @ [" << id << "]" << endl;
	if (final_id != NULL) *final_id = id;
	return count;

}


template <typename Key, typename Value>
int hash_delete(Key key, HashNode<Key,Value>* ht, int length){
	size_t hash = hash3D(key, length);
	int count;
	size_t id = hash_find(key, ht, length, &count);
	if (id == -1) return 1;	// if key is not found, nothing to be done
	ht[id].isEmpty = true;
	ht[id].isDeleted = true;
	cout << "Delete: <" << key << "," << ht[id].value << "> (count, attempts-1) = " << ht[hash].count << ", " << count-1 << ")" << endl;	// TODO: Can reduce count by one when the last number is deleted, but that may well have been the only number inserted, in which case count should have gone to zero. But theres no way to keep track of this! When creating the hashmap for interpolator, upon interval refinement, dont delete the interval. Only change the value, and add a new interval. 
	return 0;
}


template <typename Key, typename Value>
void hash_clear(HashNode<Key,Value>* ht, int length){
	for (int i=0; i<length; ++i) ht[i] = HashNode<Key,Value>();
}


template <typename Key, typename Value>
void hash_print(HashNode<Key,Value>* ht, int length){
	cout << "~~~ HASH TABLE ~~~~~~~~~~~" << endl;
	for (int i=0; i<length; ++i){
		cout << i << ": "; ht[i].print();
	}	
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
}




#endif






