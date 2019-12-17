#include <iostream>
using namespace std;

#include "hashtable2_dh.h"

int main(){
	const int len = 16;	// better be a prime numnber		
	HashNode<int, float> ht[len];
	
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
	
	hash_clear(ht, len);
	hash_print(ht, len);

	for (auto key : {0,11,22,33,44}){
		hash_insert(key, float(key/10.f), ht, len);
	}

	hash_print(ht, len);
	
	for (auto key : {1,12,23,34,45}){
		hash_insert(key, float(key/10.f), ht, len);
	}

	hash_print(ht, len);

	for (auto key : {9}){
		hash_insert(key, float(key/10.f), ht, len);
	}

	hash_print(ht, len);

	for (auto key : {0,22,44}){
		hash_delete(key, ht, len);
	}

	hash_print(ht, len);
	
	for (auto key : {0,11,22,33,44,55}){
		int id = hash_find(key, ht, len);
	}

	for (auto key : {0}){
		hash_insert(key, float(key/10.f), ht, len);
	}

	hash_print(ht, len);
	
	for (auto key : {1,2,3,4,5,6,7,8,9,10}){
		hash_find(key, ht, len);
	}

	hash_print(ht, len);

	
	for (auto key : {44,22}){
		hash_insert(key, float(key/10.f), ht, len);
	}

	hash_print(ht, len);


	for (auto key : {9,12,34,11,33,45}){
		hash_delete(key, ht, len);
	}

	hash_print(ht, len);


	for (auto key : {33, 77, 99, 13}){
		hash_insert(key, float(key/10.f), ht, len);
	}

	hash_print(ht, len);

	for (auto key : {99, 110}){
		hash_find(key, ht, len);
	}


	


	return 0;	
}






