#include <iostream>
using namespace std;

#include "hashtable.h"

int main(){
	const int len = 17;
	HashNode<int, float> ht[len];
	
//	int keys[] = {50, 700, 76, 85, 92, 73, 101}; 
	int keys[] = {0,1,7,14,21,6}; 
	for (auto key : keys){
		hash_insert(key, float(key/10.f), ht, len);
	}

	hash_print(ht, len);
	
	int keys_find[] = {0,1,7,14,21,28,42,6}; 
	for (auto key : keys_find){
		int attempts;
		int id = hash_find(key, ht, len, &attempts);
		if (id >= 0) cout << "<" << key << ", " << ht[id].value << "> [" << attempts << " tries]" << ((key != ht[id].value*10)? " * FAIL *":"") << endl;
		else cout << "<" << key << ", " << "- " << "> [" << attempts << " tries]" << endl;
	}
}


