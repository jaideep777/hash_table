#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <random>
using namespace std;

#include "hashtable3_dh.h"

int main(){


	// 3. RIGOROUS TESTING 
	
	//srand(time(NULL));
	
	const int len = 512;		// HT size
	float load_factor = 0.4;	// HT load factor
	
	int n = load_factor * len;
	
	// Create a huge queue of random keys to draw from, for inserting into the hashtable etc
	vector <int> all_keys(500000);	
	for (int i=0; i<all_keys.size(); ++i) all_keys[i] = i;
	shuffle(all_keys.begin(), all_keys.end(), default_random_engine());

	vector <int>::iterator it = all_keys.begin();
	
	
	HashNode<int, float> ht[len];	// HT definition
	
	// generate a list of n keys. This list keeps track (separately) of all the keys in the hash table for analytics
	list <int> keys;			


	// initialize first set of keys to go into HT
	for (int i=0; i<n; ++i) keys.push_back(*it++);

//	cout << "All Keys:   ";
//	for (int i=0; i<30; ++i) cout << all_keys[i] << " ";
//	cout << endl;

//	cout << "Keys in HT: ";
//	for (auto key : keys) cout << key << " ";
//	cout << endl;


	// insert the 1st set of keys into the HT
	for (auto key:keys)	hash_insert(key, float(key/10.f), ht, len);


	// LOOP FROM HERE

	// in every iteration, keys at indices [0,del_till) in the above list are removed from the hashtable, and equal number of new keys is inserted. 
	int del_till = 0;			

	// Variables to collect statictics
	int existing_keys_found = 0, existing_keys_tested = 0;
	int non_existing_keys_found = 0, non_existing_keys_tested = 0;
	int existing_keys_not_found = 0;
	int non_existing_keys_not_found = 0;
	int deleted_keys_found = 0, deleted_keys_tested = 0;
	int deleted_keys_not_found = 0;

	int n_probes_insert = 0, n_attempts_insert = 0, n_detects_new = 0;
	int n_probes_insert_dup = 0, n_attempts_insert_dup = 0, n_detects_dup = 0;
	int n_probes_find_existing = 0, n_attempts_find_existing = 0;
	int n_probes_find_non_existing = 0, n_attempts_find_non_existing = 0;


	for (int k=0; k<100; ++k){
		
		// -----------------------------------------------------------------------------------------------------------------
		// 0. Rebuild hash-map if we dont want the probes counter to accummulate (due to repeated deletions and insertions)
		// -----------------------------------------------------------------------------------------------------------------
		hash_clear(ht, len);	// clear map
		for (auto key:keys)	hash_insert(key, float(key/10.f), ht, len);	// insert all keys in list	
		auto it3 = keys.begin();
		for (int i=0; i<del_till; ++i) hash_delete(*it3++, ht, len);	// delete all keys that were deleted in previous round

		
		// -----------------------------------------------------------------------------------------------------------------
		// 1. remove deleted keys and draw equal number of new keys from big set
		// -----------------------------------------------------------------------------------------------------------------
		for (int i=0; i<del_till; ++i)  keys.pop_front();
		for (int i=0; i<del_till; ++i)  keys.push_back(*it++);
		
//		cout << "All Keys:   ";
//		for (int i=0; i<30; ++i) cout << all_keys[i] << " ";
//		cout << endl;

//		cout << "Keys in HT: ";
//		for (auto key : keys) cout << key << " ";
//		cout << endl;


		// -----------------------------------------------------------------------------------------------------------------
		// 2. insert the newly drawn keys into the hashmap
		// -----------------------------------------------------------------------------------------------------------------
		auto it_keys = keys.begin();
		advance(it_keys, n-del_till);	// keys at locations [del_till, n) are newly drawn into queue, to be inserted 
		for (int i=0; i<del_till; ++i){
			int duplicate;
			n_probes_insert += hash_insert(*it_keys, float(*it_keys/10.f), ht, len, &duplicate);
			n_attempts_insert += 1;
			n_detects_new += 1-duplicate;
			++it_keys;
		}
//		hash_print(ht, len);
//		cout << "-----> Inserting Keys (" << n_probes_insert << "/" << n_attempts_insert << "/" << n_detects_new << " = " << float(n_probes_insert)/n_attempts_insert << " %)" << endl; 
		

		// -----------------------------------------------------------------------------------------------------------------
		// 2.1 shuffle keys
		// -----------------------------------------------------------------------------------------------------------------
		vector <int> keys_vec(keys.begin(), keys.end());
		shuffle(keys_vec.begin(), keys_vec.end(), default_random_engine());	// shuffle keys and select 1st 2/3rds to delete
		keys = list<int>(keys_vec.begin(), keys_vec.end());


		// -----------------------------------------------------------------------------------------------------------------
		// 3. Attempt to re-insert ALL the keys currently existing in the hashmap
		// -----------------------------------------------------------------------------------------------------------------
		it_keys = keys.begin();
		for (int i=0; i<del_till; ++i){
			int duplicate;
			n_probes_insert_dup += hash_insert(*it_keys, float(*it_keys/10.f), ht, len, &duplicate);
			n_attempts_insert_dup += 1;
			n_detects_dup += duplicate;
			++it_keys;
		}
//		hash_print(ht, len);
//		cout << "-----> Inserting Duplicate Keys (" << n_probes_insert_dup << "/" << n_attempts_insert_dup << "/" << n_detects_dup << " = " << float(n_probes_insert_dup)/n_attempts_insert_dup << " %)" << endl; 

		
		// -----------------------------------------------------------------------------------------------------------------
		// 4. search keys that are in the HT --- Check if EVERY key in the HT is reachable
		// -----------------------------------------------------------------------------------------------------------------
		for (auto key : keys){
			auto res = hash_find(key, ht, len);
			size_t id = res.id;
			int att = res.attempts;
			if (id == -1) ++existing_keys_not_found;
			else ++existing_keys_found;
			++existing_keys_tested;
			n_probes_find_existing += att;
			n_attempts_find_existing += 1;
		}
//		cout << "-----> Existing keys reported found = " << existing_keys_found << " (" << float(existing_keys_found)/existing_keys_tested*100 << " %)" << endl; 
//		cout << "-----> Existing keys reported not found = " << existing_keys_not_found << " (" << float(existing_keys_not_found)/existing_keys_tested*100 << " %)" << endl; 
		

		// -----------------------------------------------------------------------------------------------------------------
		// 5. search keys that are NOT in the HT 
		// -----------------------------------------------------------------------------------------------------------------
		auto it2 = it;	// it points just beyond the last key that was drawn from the big random keys list 
		for (int i=0; i<10; ++i){
			auto res = hash_find(*it2++, ht, len);
			size_t id = res.id;
			int att = res.attempts;
			if (id == -1) ++non_existing_keys_not_found;
			else ++non_existing_keys_found;
			++non_existing_keys_tested;
			n_probes_find_non_existing += att;
			n_attempts_find_non_existing += 1;
		}
//		cout << "-----> Non-Existing keys reported found = " << non_existing_keys_found << " (" << float(non_existing_keys_found)/non_existing_keys_tested*100 << " %)" << endl; 
//		cout << "-----> Non-Existing keys reported not found = " << non_existing_keys_not_found << " (" << float(non_existing_keys_not_found)/non_existing_keys_tested*100 << " %)" << endl; 
		
		
		// -----------------------------------------------------------------------------------------------------------------
		// 6. choose a random number M of keys to delete (even though range [0,M) in the keys list will be deleted, 
		//       these keys will be random, since keys were shuffled)
		// -----------------------------------------------------------------------------------------------------------------
		del_till = (float(rand())/RAND_MAX) * n;

		auto it1 = keys.begin();
		for (int i=0; i<del_till; ++i) hash_delete(*it1++, ht, len);
//		hash_print(ht, len);


		// -----------------------------------------------------------------------------------------------------------------
		// 7. Search for EVERY deleted key. They should NOT be reported found 
		// -----------------------------------------------------------------------------------------------------------------
		it1 = keys.begin();
		for (int i=0; i<del_till; ++i){
			auto res = hash_find(*it1++, ht, len);
			size_t id = res.id;
			int att = res.attempts;
			if (id == -1) ++deleted_keys_not_found;
			else ++deleted_keys_found;
			++deleted_keys_tested;
			n_probes_find_non_existing += att;
			n_attempts_find_non_existing += 1;
		}
//		cout << "-----> Deleted keys reported found = " << deleted_keys_found << " (" << float(deleted_keys_found)/deleted_keys_tested*100 << " %)" << endl; 
//		cout << "-----> Deleted keys reported not found = " << deleted_keys_not_found << " (" << float(deleted_keys_not_found)/deleted_keys_tested*100 << " %)" << endl; 


		// -----------------------------------------------------------------------------------------------------------------
		// 8. Search for ALL keys that weren't deleted. They SHOULD be reported found 
		// -----------------------------------------------------------------------------------------------------------------
		it1 = keys.begin();			// iterator points to keys.front() 
		advance(it1, del_till);		// advance it to del_till (these are the keys that were NOT deleted)
		for (int i=0; i<n-del_till; ++i){
			auto res = hash_find(*it1++, ht, len);
			size_t id = res.id;
			int att = res.attempts;
			if (id == -1) ++existing_keys_not_found;
			else ++existing_keys_found;
			++existing_keys_tested;
			n_probes_find_existing += att;
			n_attempts_find_existing += 1;
		}
//		cout << "-----> After Delete:" << endl;
//		cout << "-----> Existing keys reported found = " << existing_keys_found << " (" << float(existing_keys_found)/existing_keys_tested*100 << " %)" << endl; 
//		cout << "-----> Existing keys reported not found = " << existing_keys_not_found << " (" << float(existing_keys_not_found)/existing_keys_tested*100 << " %)" << endl; 
		
//		hash_print(ht, len);
		
//		cout << "-------------------x x x---------------------------\n";
	}
	

	cout << "Test HashTable Summary: " << endl;
	cout << " Correct: " << endl;
	cout << "-----> Existing keys reported found = " << existing_keys_found << " (" << float(existing_keys_found)/existing_keys_tested*100 << "%)" << endl; 
	cout << "-----> Non-Existing keys reported not found = " << non_existing_keys_not_found << " (" << float(non_existing_keys_not_found)/non_existing_keys_tested*100 << "%)" << endl; 
	cout << "-----> Deleted keys reported not found = " << deleted_keys_not_found << " (" << float(deleted_keys_not_found)/deleted_keys_tested*100 << "%)" << endl; 
	cout << " Wrong: " << endl;
	cout << "-----> Existing keys reported not found = " << existing_keys_not_found << " (" << float(existing_keys_not_found)/existing_keys_tested*100 << "%)" << endl; 
	cout << "-----> Non-Existing keys reported found = " << non_existing_keys_found << " (" << float(non_existing_keys_found)/non_existing_keys_tested*100 << "%)" << endl; 
	cout << "-----> Deleted keys reported found = " << deleted_keys_found << " (" << float(deleted_keys_found)/deleted_keys_tested*100 << "%)" << endl; 
	cout << " Duplicates:" << endl;
	cout << "-----> Duplicates  detected / tested = " << float(n_detects_dup)/n_attempts_insert_dup*100 << "%" << endl;	
	cout << "-----> New entries detected / tested = " << float(n_detects_new)/n_attempts_insert*100 << "%" << endl;	
	cout << " Performance:" << endl;
	cout << "-----> Probes per insertion = " << float(n_probes_insert)/n_attempts_insert << endl;	
	cout << "-----> Probes per insertion (duplicate keys) = " << float(n_probes_insert_dup)/n_attempts_insert_dup << endl;	
	cout << "-----> Probes per search (key exists)  = " << float(n_probes_find_existing)/n_attempts_find_existing << endl;	
	cout << "-----> Probes per search (key missing) = " << float(n_probes_find_non_existing)/n_attempts_find_non_existing << endl;	


//	hash_print(ht, len);	
	

	return 0;	
}




// OTHER TESTS

//	const int len = 16;	// better be a prime numnber		
//	HashNode<int, float> ht[len];
	
//	for (auto key : {0,1,7,8,9,14,21,22,6}){
//		hash_insert(key, float(key/10.f), ht, len);
//	}

//	hash_print(ht, len);
//	
//	for (auto key : {0,1,7,14,21,22,28,42,6}){
//		int id = hash_find(key, ht, len);
//	}

//	for (auto key : {14,21,22}){
//		hash_delete(key, ht, len);
//	}

//	hash_print(ht, len);
//	
//	for (auto key : {11,33,44,55,66}){
//		hash_insert(key, float(key/10.f), ht, len);
//	}

//	hash_print(ht, len);

//	for (auto key : {11,33,44,55,66}){
//		hash_delete(key, ht, len);
//	}

//	hash_print(ht, len);


//	for (auto key : {44}){
//		hash_insert(key, float(key/10.f), ht, len);
//	}

//	hash_print(ht, len);


	// SYSTEMATICALLY TEST DELETE
	
//	hash_clear(ht, len);
//	hash_print(ht, len);

//	for (auto key : {0,11,22,33,44}){
//		hash_insert(key, float(key/10.f), ht, len);
//	}

//	hash_print(ht, len);
//	
//	for (auto key : {1,12,23,34,45}){
//		hash_insert(key, float(key/10.f), ht, len);
//	}

//	hash_print(ht, len);

//	for (auto key : {9}){
//		hash_insert(key, float(key/10.f), ht, len);
//	}

//	hash_print(ht, len);

//	for (auto key : {0,22,44}){
//		hash_delete(key, ht, len);
//	}

//	hash_print(ht, len);
//	
//	for (auto key : {0,11,22,33,44,55}){
//		int id = hash_find(key, ht, len);
//	}

//	for (auto key : {0}){
//		hash_insert(key, float(key/10.f), ht, len);
//	}

//	hash_print(ht, len);
//	
//	for (auto key : {1,2,3,4,5,6,7,8,9,10}){
//		hash_find(key, ht, len);
//	}

//	hash_print(ht, len);

//	
//	for (auto key : {44,22}){
//		hash_insert(key, float(key/10.f), ht, len);
//	}

//	hash_print(ht, len);


//	for (auto key : {9,12,34,11,33,45}){
//		hash_delete(key, ht, len);
//	}

//	hash_print(ht, len);


//	for (auto key : {33, 77, 99, 13}){
//		hash_insert(key, float(key/10.f), ht, len);
//	}

//	hash_print(ht, len);

//	for (auto key : {99, 110}){
//		hash_find(key, ht, len);
//	}

	// 2. TEST DUPLICATE KEYS INSERTION

//	size_t a = -1;
//	cout << "-1 size_t = " << a << endl;

//	const int len = 16;	// better be a prime numnber		
//	HashNode<int, float> ht[len];
//	
//	for (auto key : {0,16,32,64}){
//		hash_insert(key, float(key/10.f), ht, len);
////		hash_print(ht, len);
//	}

//	hash_print(ht, len);
//	
//	for (auto key : {0,16,32,64,65,66}){
//		int id = hash_find(key, ht, len).id;
//	}

//	for (auto key : {16}){
//		hash_delete(key, ht, len);
//	}

//	hash_print(ht, len);
//	
//	for (auto key : {0,16,32,64,128,256}){
//		int id = hash_find(key, ht, len).id;
//	}


//	for (auto key : {32, 128, 256, 256}){
//		hash_insert(key, float(key/100.f), ht, len);
////		hash_print(ht, len);
//	}

//	hash_print(ht, len);


