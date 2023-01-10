#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>

typedef struct node{
	HANDLE thread_read;
	HANDLE thread_write;
	HANDLE msgSemaphore;
	SOCKET acceptedSocket;
	char* msgBuffer;
	struct node* next;
}node;

typedef struct {
	node* head;
	node* current;
	node* tail;
	CRITICAL_SECTION cs;
}list;

list* free_workers_list;
list* busy_workers_list;

void init_list(list** l);
void insert_first_node(node* new_node, list* l);
void insert_last_node(node* new_node, list* l);
void delete_node(node* new_node, list* l);
void print_list(list *l);
void delete_list(list* l);
