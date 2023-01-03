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

void initList(list* l);
void insertFirstNode(HANDLE data, list* l);
void insertLastNode(HANDLE data, list* l);
void deleteNode(HANDLE data, list* l);
void print_list(list *l);
