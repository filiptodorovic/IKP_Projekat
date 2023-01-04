#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>

typedef struct node{
	HANDLE thread_handle;
	struct node* next;
}node;

typedef struct {
	node* head;
	node* current;
	node* tail;
	CRITICAL_SECTION cs;
}list;

void init_list(list** l);
void insert_first_node(HANDLE data, list* l);
void insert_last_node(HANDLE data, list* l);
void delete_node(HANDLE data, list* l);
void print_list(list *l);
void delete_list(list* l);
