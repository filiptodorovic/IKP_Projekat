#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "conio.h"
#include <windows.h>

/*
struct list {
	HANDLE data;
	int key;
	struct list* next;
	struct list* head;
	struct list* current;
	struct list* tail;
};

void printList(struct list* ptr);   //pointer on head
struct list* deleteFirst();
struct list* insertFirst(int key, HANDLE data);
struct list* insertLast(int key, HANDLE data);
bool isEmpty();
struct list* find(int key);
struct list* deleteElem(int key);
*/

typedef struct node {
	HANDLE thread_handle;
	struct node* next;
} node_t;

void createList(node_t **head, node_t **tail);
void insertLastNode(HANDLE data, node_t** head, node_t** tail);
void insertFirstNode(HANDLE data, node_t** head, node_t** tail);
void deleteNode(HANDLE data, node_t** head, node_t** tail);
void print_list(node_t* head);


