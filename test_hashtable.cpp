#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <random>
using namespace std;

#include "hashtable2_dh.h"

int main(){
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


//	size_t a = -1;
//	cout << "-1 size_t = " << a << endl;

//	const int len = 16;	// better be a prime numnber		
//	HashNode<int, float> ht[len];
//	
//	for (auto key : {0,16,32,64}){
//		hash_insert(key, float(key/10.f), ht, len);
//	}

//	hash_print(ht, len);
//	
//	for (auto key : {0,16,32,64,65,66}){
//		int id = hash_find(key, ht, len).id;
//	}

////	for (auto key : {32}){
////		hash_delete(key, ht, len);
////	}

//	hash_print(ht, len);
//	
//	for (auto key : {0,16,32,64,128}){
//		int id = hash_find(key, ht, len).id;
//	}


	// RIGOROUS TESTING
	
	srand(time(NULL));
	
	const int len = 512;
	float load_factor = 0.4;
	
	int n = load_factor * len;
	
	vector <int> all_keys(500000);
	for (int i=0; i<all_keys.size(); ++i) all_keys[i] = i;
	shuffle(all_keys.begin(), all_keys.end(), default_random_engine());

	vector <int>::iterator it = all_keys.begin();
	
	
	HashNode<int, float> ht[len];
	
	list <int> keys;			// generate n keys

	int existing_keys_found = 0, existing_keys_tested = 0;
	int non_existing_keys_found = 0, non_existing_keys_tested;
	int existing_keys_not_found = 0;
	int non_existing_keys_not_found = 0;
	int deleted_keys_found = 0, deleted_keys_tested = 0;
	int deleted_keys_not_found = 0;

	int n_collisions_insert = 0, n_attempts_insert = 0;
	int n_att_find_existing = 0, n_oper_find_existing = 0;
	int n_att_find_non_existing = 0, n_oper_find_non_existing = 0;

	int del_till = 0;

	// initialize first set of keys to go into HT
	for (int i=0; i<n; ++i) keys.push_back(*it++);
	
	cout << "All Keys:   ";
	for (int i=0; i<30; ++i) cout << all_keys[i] << " ";
	cout << endl;

	cout << "Keys in HT: ";
	for (auto key : keys) cout << key << " ";
	cout << endl;

	for (auto key:keys)	hash_insert(key, float(key/10.f), ht, len);


	// LOOP FROM HERE

	del_till = 0;

	for (int k=0; k<100; ++k){
	
		// 1. remove deleted keys and draw equal number of new keys from big set
		for (int i=0; i<del_till; ++i)  keys.pop_front();
		for (int i=0; i<del_till; ++i)  keys.push_back(*it++);
		
		cout << "All Keys:   ";
		for (int i=0; i<30; ++i) cout << all_keys[i] << " ";
		cout << endl;

		cout << "Keys in HT: ";
		for (auto key : keys) cout << key << " ";
		cout << endl;

		// 2. insert the newly drawn keys into the hashmap
		auto it_keys = keys.begin();
		advance(it_keys, n-del_till);	// last n-del_till keys are newly drawn into queue, to be inserted 
		for (int i=0; i<del_till; ++i){
			n_collisions_insert += hash_insert(*it_keys, float(*it_keys/10.f), ht, len);
			n_attempts_insert += 1;
			++it_keys;
		}
		hash_print(ht, len);
		
		// 3. shuffle keys
		vector <int> keys_vec(keys.begin(), keys.end());
		shuffle(keys_vec.begin(), keys_vec.end(), default_random_engine());	// shuffle keys and select 1st 2/3rds to delete
		keys = list<int>(keys_vec.begin(), keys_vec.end());
		
		// 4. search keys that are in the HT
		for (auto key : keys){
//			int att=0;
			auto res = hash_find(key, ht, len);
			size_t id = res.id;
			int att = res.attempts;
			if (id == -1) ++existing_keys_not_found;
			else ++existing_keys_found;
			++existing_keys_tested;
			n_att_find_existing += att;
			n_oper_find_existing += 1;
		}
		cout << "-----> Existing keys reported found = " << existing_keys_found << " (" << float(existing_keys_found)/existing_keys_tested*100 << " %)" << endl; 
		cout << "-----> Existing keys reported not found = " << existing_keys_not_found << " (" << float(existing_keys_not_found)/existing_keys_tested*100 << " %)" << endl; 
		
		// 5. search keys that are NOT in the HT
		auto it2 = it;
		for (int i=0; i<10; ++i){
//			int att=0;
			auto res = hash_find(*it2++, ht, len);
			size_t id = res.id;
			int att = res.attempts;
			if (id == -1) ++non_existing_keys_not_found;
			else ++non_existing_keys_found;
			++non_existing_keys_tested;
			n_att_find_non_existing += att;
			n_oper_find_non_existing += 1;
		}
		cout << "-----> Non-Existing keys reported found = " << non_existing_keys_found << " (" << float(non_existing_keys_found)/non_existing_keys_tested*100 << " %)" << endl; 
		cout << "-----> Non-Existing keys reported not found = " << non_existing_keys_not_found << " (" << float(non_existing_keys_not_found)/non_existing_keys_tested*100 << " %)" << endl; 
		
		
		// 6. choose 1st m keys to delete (these will be random, since keys were shuffled)
		del_till = (float(rand())/RAND_MAX) * n;

		cout << "Keys in HT: ";
		for (auto key : keys) cout << key << " ";
		cout << endl;
		auto it1 = keys.begin();
		for (int i=0; i<del_till; ++i) hash_delete(*it1++, ht, len);
		hash_print(ht, len);

//		deleted_keys_found = 0;
//		deleted_keys_not_found = 0;
		it1 = keys.begin();
		for (int i=0; i<del_till; ++i){
//			int att=0;
			auto res = hash_find(*it1++, ht, len);
			size_t id = res.id;
			int att = res.attempts;
			if (id == -1) ++deleted_keys_not_found;
			else ++deleted_keys_found;
			++deleted_keys_tested;
			n_att_find_non_existing += att;
			n_oper_find_non_existing += 1;
		}
		cout << "-----> Deleted keys reported found = " << deleted_keys_found << " (" << float(deleted_keys_found)/deleted_keys_tested*100 << " %)" << endl; 
		cout << "-----> Deleted keys reported not found = " << deleted_keys_not_found << " (" << float(deleted_keys_not_found)/deleted_keys_tested*100 << " %)" << endl; 

//		existing_keys_found = 0;
//		non_existing_keys_found = 0;
//		existing_keys_not_found = 0;
//		non_existing_keys_not_found = 0;
		it1 = keys.begin();			// iterator points to keys.front() 
		advance(it1, del_till);		// advance it to del_till (these are the keys that were NOT deleted)
		for (int i=0; i<n-del_till; ++i){
//			int att=0;
			auto res = hash_find(*it1++, ht, len);
			size_t id = res.id;
			int att = res.attempts;
			if (id == -1) ++existing_keys_not_found;
			else ++existing_keys_found;
			++existing_keys_tested;
			n_att_find_existing += att;
			n_oper_find_existing += 1;
		}
		cout << "-----> After Delete:" << endl;
		cout << "-----> Existing keys reported found = " << existing_keys_found << " (" << float(existing_keys_found)/existing_keys_tested*100 << " %)" << endl; 
		cout << "-----> Existing keys reported not found = " << existing_keys_not_found << " (" << float(existing_keys_not_found)/existing_keys_tested*100 << " %)" << endl; 
		
		hash_print(ht, len);
		
		cout << "-------------------x x x---------------------------\n";
	}
	

	cout << "Summary: " << endl;
	cout << " Correct: " << endl;
	cout << "-----> Existing keys reported found = " << existing_keys_found << " (" << float(existing_keys_found)/existing_keys_tested*100 << " %)" << endl; 
	cout << "-----> Non-Existing keys reported not found = " << non_existing_keys_not_found << " (" << float(non_existing_keys_not_found)/non_existing_keys_tested*100 << " %)" << endl; 
	cout << "-----> Deleted keys reported not found = " << deleted_keys_not_found << " (" << float(deleted_keys_not_found)/deleted_keys_tested*100 << " %)" << endl; 
	cout << " Wrong: " << endl;
	cout << "-----> Existing keys reported not found = " << existing_keys_not_found << " (" << float(existing_keys_not_found)/existing_keys_tested*100 << " %)" << endl; 
	cout << "-----> Non-Existing keys reported found = " << non_existing_keys_found << " (" << float(non_existing_keys_found)/non_existing_keys_tested*100 << " %)" << endl; 
	cout << "-----> Deleted keys reported found = " << deleted_keys_found << " (" << float(deleted_keys_found)/deleted_keys_tested*100 << " %)" << endl; 
	cout << " Performance:" << endl;
	cout << "-----> Attempts per insertion = " << 1+float(n_collisions_insert)/n_attempts_insert << endl;	
	cout << "-----> Attempts per search (key exists)  = " << float(n_att_find_existing)/n_oper_find_existing << endl;	
	cout << "-----> Attempts per search (key missing) = " << float(n_att_find_non_existing)/n_oper_find_non_existing << endl;	


	cout << " Table and keys after all operations:\n";
	for (int i=0; i<del_till; ++i)  keys.pop_front();
	cout << "Keys in HT: ";
	for (auto key : keys) cout << key << " ";
	cout << endl;
	
	hash_print(ht, len);	
	

	return 0;	
}






