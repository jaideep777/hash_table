#ifndef HASHTABLE_CPU_GPU
#define HASHTABLE_CPU_GPU
#include <iostream>
#include <cassert>
using namespace std;

//#include "../utils/simple_math.h"

#define DEBUG_HT 

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
		count = 0;			// number of keys that have ever hashed at this node (the hashtable is just an array of HashNodes, so node=slot)
		isEmpty = true;		
		isDeleted = false;	
	}

	void print(){
		if (isDeleted) cout << "---<" << key << "," << value << ">--- " << " [" << count << " probes]" << endl;
		else if (!isEmpty) cout << "<" << key << ", " << value << "> " << " [" << count << " probes]" << endl;
		else cout << "-----------" << " [" << count << " probes]" << endl;
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
//	return 1;
	return 2*((key*677)%50)+17;	// should return an odd number, as table size is 2^m
}


//template <typename Key>
//int increment(Key key, int count){
////	return (id+11)%length;
//	return count*hash2(key);
////	return (key)%677;
//}

struct HashFindResult{
	size_t id;
//	size_t firstEmptySlot;
	int    attempts;
};


template <typename Key, typename Value>
HashFindResult hash_find(Key key, HashNode<Key,Value>* ht, int length){//, int* attempts=NULL){
	
	HashFindResult res;
//	res.firstEmptySlot = -1;
	
	size_t hash = hash3D(key, length);
	size_t id;
	int count = 0; 
//	if (attempts == NULL) attempts = new int; // DEBUG ONLY
	DEBUG_HT cout << "Find: (" << key << "): |";// << "*, ";
	do { 	// start at expected location and probe until: slot is filled AND the key matches. (i.e., dont stop at empty slot)
		id = (hash + count*hash2(key))%length;
		++count;

//		if (ht[id].isEmpty) res.firstEmptySlot = min(res.firstEmptySlot, id);

		DEBUG_HT cout << id;
//		DEBUG_HT if (id == res.firstEmptySlot) cout << "__";
		DEBUG_HT cout << ", ";

		if ((ht[id].key == key && !ht[id].isEmpty)) break;	// loop breaks as soon as key is found AND the slot was not marked empty
		
	 	// if probe reaches this point on the count'th try, the key cannot be in the table, because it it were, loop would've exited on the previous statement. (Only 'count' no. of entries were ever stored for a key hasshing to here)
		if (count >= ht[hash].count) { id = -1; break; }
	
		assert(count <= length);	// if count exeeds length, something is wrong!
	} while (1);
//	if (attempts != NULL) *attempts = count+1;

	res.attempts = count;
	res.id = id;
	
	// DEBUG ONLY
	DEBUG_HT if (id != size_t(-1)) cout << "|   <" << key << ", " << ht[id].value << "> [" << res.attempts << " tries]" << ((key != ht[id].value*10)? " * FAIL *":"") << endl;
	DEBUG_HT else cout << "|   <" << key << ", " << "- " << "> [" << res.attempts << " tries]" << endl;
//	if (attempts == NULL) delete attempts;

//	cout << "RES (find): " << (int)res.id << " " << (int)res.firstEmptySlot << " " << res.attempts << endl;

	return res;
}


// Insertions will happen on CPU only (Dont want headache of thread locks)
template <typename Key, typename Value>
int hash_insert(Key key, Value value, HashNode<Key,Value>* ht, int length, int* final_id=NULL){
	cout << "Insert: <" << key << ", " << value << "> : trying ";
	size_t hash = hash3D(key, length);	// get the hash of the specifed key

	size_t id;// = hash;					// the location where this <key, value> will be stored equals hash, but will be incremented if not empty 
	size_t firstEmptySlot = -1;
	int    count_firstEmptySlot = length+1;	// set to index beyond the table's range
//	cout << hash << "*, ";
	int count = 0;						// number of collisions before empty slot was found (defaults to 0, i.e. no increments were required)
	do { 	
		id = (hash + count*hash2(key))%length;			// increment slot (using double hashing) until empty or deleted slot found 
		++count;						// counter is not incremented if inserting into previously deleted slot, because keys that have been mapped past the deleted slot stay intact

		if (ht[id].isEmpty && firstEmptySlot == -1){	// if found an empty slot and firstEmptySlot is still invalid
			firstEmptySlot = id;
			count_firstEmptySlot = count;
		}
		
		DEBUG_HT cout << id;
		DEBUG_HT if (id == firstEmptySlot) cout << "__";
		DEBUG_HT cout << ", ";

	 	// if probe exceeds count, the key cannot be in the table, and we have reached a slot beyond count
		if (count > ht[hash].count) break;
		
		DEBUG_HT cout.flush();
		assert(count <= length);
	} while (!(ht[id].key == key && !ht[id].isEmpty));

	// If key was found in HT, we simply need to replace the associated value and exit
	if (ht[id].key == key && !ht[id].isEmpty){	
		DEBUG_HT cout << "(count = " << count << ") --> Replacing @ [" << id << "]: <" << key << "," << ht[id].value << "> ==> <" << key << "," << value << ">" << endl;
		ht[id].value = value; 
		if (final_id != NULL) *final_id = 1;	// final_id is used as a duplicate detection flag
		return count;
	}

	cout << "... ";
	// Otherwise (if key was not found) continue searching for an empty slot (loop wont be entered if empty slot was already found, i.e. firstEmptySlot != -1)
	while (!ht[id].isEmpty && firstEmptySlot == -1){	
		assert(count <= length);
		id = (hash + count*hash2(key))%length;			// increment slot (using double hashing) until empty or deleted slot found 
		++count;										// in this loop, counter must be incremented AFTER testing the ID, because previous loop exited after incrementing the counter. counter is not incremented if inserting into previously deleted slot, because keys that have been mapped past the deleted slot stay intact
		DEBUG_HT cout << id << ", ";
	}
	cout << "(count = " << count << ")";
	
	// If empty slot was detected, set ID and count to that of the empty slot 
	if (firstEmptySlot != -1){
		id = firstEmptySlot;
		count = count_firstEmptySlot;
	}
	
	ht[id].value = value;				// once empty slot found, store the <key,value> in the slot
	ht[id].key = key;
	ht[id].isEmpty = false;
	ht[id].isDeleted = false;
	ht[hash].count = max(ht[hash].count, count);	// when insertion happens in a deleted slot, we dont want to lose count of how many valid entries existed beyond the deleted slot. Hence, count must be maximum of previous count and result of current insertion
	DEBUG_HT cout << " --> stored @ [" << id << "]" << endl;
	if (final_id != NULL) *final_id = 0;	// final_id is used as a duplicate detection flag
//	if (final_id != NULL) *final_id = id;
	return count;

}


template <typename Key, typename Value>
int hash_delete(Key key, HashNode<Key,Value>* ht, int length){
	size_t hash = hash3D(key, length);
	auto res = hash_find(key, ht, length);
	size_t id = res.id;
	int count = res.attempts;
	if (id == size_t(-1)) return 1;	// if key is not found, nothing to be done
	ht[id].isEmpty = true;
	ht[id].isDeleted = true;
	DEBUG_HT cout << "Delete: <" << key << "," << ht[id].value << "> (count, attempts-1) = " << ht[hash].count << ", " << count-1 << ")" << endl;	// TODO: Can reduce count by one when the last number is deleted, but that may well have been the only number inserted, in which case count should have gone to zero. But theres no way to keep track of this! When creating the hashmap for interpolator, upon interval refinement, dont delete the interval. Only change the value, and add a new interval. 
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






