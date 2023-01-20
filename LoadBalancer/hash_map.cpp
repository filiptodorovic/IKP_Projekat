#include <stdio.h>
#include "hash_map.h"

#define  _CRT_SECURE_NO_WARNINGS

#pragma warning(disable:4996)

static client_thread* hash_table_clt[MAX_ELEM];
static CRITICAL_SECTION hashTableCS;

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
	printf("====START===\n");
	for (int i = 0; i < MAX_ELEM;i++) {
		if (hash_table_clt[i] == NULL) {
			printf("\t%d\t----\n",i);
		}
		else {
			printf("\t%d\t", i);
			client_thread* tmp = hash_table_clt[i];
			while (tmp != NULL) {
				printf("%s -> ", tmp->clientName);
				tmp = tmp->next;
			}
			printf("\n");
		}
	}
	printf("====END====\n");
}

bool init_hash_table() {
	for (int i = 0; i < MAX_ELEM; i++) {
		hash_table_clt[i] = NULL;
	}
	InitializeCriticalSection(&hashTableCS);
	return true;
}

bool insert_client(client_thread* ct) {
	EnterCriticalSection(&hashTableCS);
	if (ct == NULL) return false;
	int index = hash(ct->clientName);
	if (hash_table_clt[index] == NULL) {
		hash_table_clt[index] = ct;
		hash_table_clt[index]->next = NULL;
	}
	else {
		ct->next = hash_table_clt[index];
		hash_table_clt[index] = ct;
	}
	if (hash_table_clt[index] == NULL) {
		LeaveCriticalSection(&hashTableCS);
		return false;
	}
	else {
		LeaveCriticalSection(&hashTableCS);
		return true;
	}

}

client_thread* lookup_client(char* name) {
	int index = hash(name);
	EnterCriticalSection(&hashTableCS);
	client_thread* tmp = hash_table_clt[index];
	while (tmp != NULL && strcmp(tmp->clientName, name) != 0)
		tmp = tmp->next;
	LeaveCriticalSection(&hashTableCS);
	return tmp;
}

client_thread* delete_client(char* name) {
	int index = hash(name);
	EnterCriticalSection(&hashTableCS);
	client_thread* tmp = hash_table_clt[index];
	client_thread* prev = NULL;
	while (tmp != NULL && strcmp(tmp->clientName, name) != 0) {
		prev = tmp;
		tmp = tmp->next;
	}
	if (tmp == NULL) { 
		LeaveCriticalSection(&hashTableCS); 
		return NULL; 
	} //we did not find client thread
	if (prev == NULL) { //deleting the head
		hash_table_clt[index] = tmp->next;
	}
	else {
		prev->next = tmp->next;
	}
	LeaveCriticalSection(&hashTableCS);
	return tmp;
}

void delete_hashtable() {
	for (int i = 0; i < MAX_ELEM; i++) {
		client_thread* head = hash_table_clt[i];
		while (head != NULL) {
			client_thread* prev = head;
			head = head->next;
			free(prev);
		}
	}
	DeleteCriticalSection(&hashTableCS);
}



