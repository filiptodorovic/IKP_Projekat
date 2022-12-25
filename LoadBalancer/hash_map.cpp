#include "hash_map.h"
#include <stdio.h>
#define  _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

unsigned int hash(char* name) {
	int length = strnlen(name, CLIENT_NAME_LEN);
	unsigned int hash_val = 0;
	for (int i = 0; i < length; i++) {
		hash_val += name[i];
		hash_val = (hash_val * name[i])% MAX_ELEM;
	}
	return hash_val;
}

void print_table() {
	for (int i = 0; i < MAX_ELEM;i++) {
		if (hash_table_clt[i] == NULL) {
			printf("\t%d\t----\n",i);
		}
		else {
			printf("\t%d\t%d\n", i,hash_table_clt[i]);
		}
	}
}

bool init_hash_table() {
	for (int i = 0; i < MAX_ELEM; i++) {
		hash_table_clt[i] = NULL;
	}
	return true;
}

bool hash_table_insert(client_threads* ct) {
	if (ct == NULL) return false;
	int index = hash(ct->clientName);
	hash_table_clt[index] = ct;
	return true;
}


