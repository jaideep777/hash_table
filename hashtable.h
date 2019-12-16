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
		count = 0;			// number of collisions that have happend at this slot
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
	return (key*377771) % length;
//	return (key.z*377771 + key.y*677 + key.x) % length;	// 2.15, 64
//	return (key.z*377771 + key.y*133337 + key.x) % length; // 1.7, 25
//	return (key.z*497771 + key.y*133337 + key.x) % length; // 30, 173
//	return ((long long int)key.z*497771 + key.y*787 + key.x) % length; // 1.5, 28
}


int hash2(int id, int length){
//	return (id+11)%length;
	return (id+677)%length;
}



template <typename Key, typename Value>
int hash_insert(Key key, Value value, HashNode<Key,Value>* ht, int length, int* final_id=NULL){
	cout << "Insert: <" << key << ", " << value << "> : trying ";
	size_t hash = hash3D(key, length);	// get the hash of the specifed key
	size_t id = hash;					// the location where this <key, value> will be stored equals hash, but will be incremented if not empty 
	cout << hash << "*, ";
	int count = 0;						// number of attemts required before empty slot was found (defaults to one, i.e. no increments were required)
	while (!ht[id].isEmpty){ 	
		id = hash2(id, length);					// increment slot (using double hashing) until empty or deleted slot found 
		/*if (!ht[id].isDeleted)*/ ++count;			// counter is not incremented if inserting into previously deleted slot, because keys that have been mapped past the deleted slot stay intact
		cout << id << ", ";
	}
	ht[id].value = value;				// once empty slot found, store the <key,value> in the slot, and 
	ht[id].key = key;
	ht[id].isEmpty = false;
	ht[id].isDeleted = false;
	ht[hash].count = max(ht[hash].count, count);	// when insertion happens in a deleted slot, we dont want to lose information that something existed beyond the deleted slot. Hence, count must be maximum of previous count and result of current insertion
	cout << " --> stored @ [" << id << "]" << endl;
	if (final_id != NULL) *final_id = id;
	return count;

}


template <typename Key, typename Value>
int hash_delete(Key key, HashNode<Key,Value>* ht, int length){
	size_t hash = hash3D(key, length);
	int count;
	size_t id = hash_find(key, ht, length, &count);
	ht[id].isEmpty = true;
	ht[id].isDeleted = true;
	cout << "Delete: <" << key << "," << ht[id].value << "> (count, attempts-1) = " << ht[hash].count << ", " << count-1 << ")" << endl;	// TODO: Can reduce count by one when the last number is deleted, but that may well have been the only number inserted, in which case count should have gone to zero. But theres no way to keep track of this! When creating the hashmap for interpolator, upon interval refinement, dont delete the interval. Only change the value, and add a new interval. 
}


template <typename Key, typename Value>
size_t hash_find(Key key, HashNode<Key,Value>* ht, int length, int* attempts=NULL){
	size_t hash = hash3D(key, length);
	int id = hash;
	int count = 0; 
	if (attempts == NULL) attempts = new int; // DEBUG ONLY
	if (attempts != NULL) *attempts = count+1;
	cout << "Find: " << id << "*, ";
	while (!(ht[id].key == key && !ht[id].isEmpty)){ 	// start at expected locaiton and probe until key matches
		id = hash2(id, length);
		++count;
		cout << id << ", ";
		if (attempts != NULL) *attempts = count+1;
		if (count > ht[hash].count) {id = -1; break;} // return -1;			// if probe attempts exceeds count, key is not in the table (because only 'count' no. of entries were ever stored for the same key)
	}
	
	if (id >= 0) cout << "<" << key << ", " << ht[id].value << "> [" << *attempts << " tries]" << ((key != ht[id].value*10)? " * FAIL *":"") << endl;
	else cout << "<" << key << ", " << "- " << "> [" << *attempts << " tries]" << endl;

	if (attempts == NULL) delete attempts;
	
	return id;
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






