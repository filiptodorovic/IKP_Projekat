#define  _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#include "list.h"

void init_list(list** l) {
	*l = (list*)malloc(sizeof(list));
	(*l)->head = NULL;
	(*l)->current = NULL;
	(*l)->tail = NULL;
	InitializeCriticalSection(&(*l)->cs);
}

void insert_first_node(node* new_node, list* l) {
	EnterCriticalSection(&l->cs);

	// If the list is empty, set the tail to the new node
	if (l->tail == NULL) {
		l->tail = new_node;
	}

	// Set the head to the new node
	l->head = new_node;
	LeaveCriticalSection(&l->cs);
}

void insert_last_node(node* new_node, list* l) {
	EnterCriticalSection(&l->cs);

	// If the list is empty, set the head and tail to the new node
	if (l->head == NULL) {
		l->head = new_node;
		l->tail = new_node;
	}
	else {
		// Otherwise, add the new node to the end of the list
		l->tail->next = new_node;
		l->tail = new_node;
	}
	LeaveCriticalSection(&l->cs);

}

void delete_node(node* new_node, list *l) {
	EnterCriticalSection(&l->cs);
	node* current = l->head;
	node* previous = NULL;

	// Search for the node to delete
	while (current != NULL && current != new_node) {
		previous = current;
		current = current->next;
	}

	// If the node was found, delete it
	if (current != NULL) {
		// If the node to delete is the head of the list, update the head pointer
		if (current == l->head) {
			l->head = current->next;
		}
		// If the node to delete is the tail of the list, update the tail pointer
		if (current == l->tail) {
			l->tail = previous;
		}
		// Update the next pointer of the previous node to skip the deleted node
		if (previous != NULL) {
			previous->next = current->next;
		}
		// Free the memory used by the node
		free(current);
	}
	else {
		printf("Node is not in list.");
	}

	LeaveCriticalSection(&l->cs);
}

void print_list(list* l) {
	EnterCriticalSection(&l->cs);
	printf("LIST: \n");
	node* current = l->head;
	while (current != NULL) {
		WCHAR* thread_name_READ = NULL;
		WCHAR* thread_name_WRITE = NULL;
		GetThreadDescription(current->thread_read, &thread_name_READ);
		printf("[%ls]->", thread_name_READ);

		GetThreadDescription(current->thread_write, &thread_name_WRITE);
		printf("[%ls]->", thread_name_WRITE);
		current = current->next;
	}
	printf("\n");
	LeaveCriticalSection(&l->cs);
}

void delete_list(list* l) {
	EnterCriticalSection(&l->cs);
	while (l->head != l->tail) {
		node* prev = l->head;
		l->head = l->head->next;
		free(prev);
	}
	l->head = NULL;
	l->tail = NULL;
	LeaveCriticalSection(&l->cs);
	DeleteCriticalSection(&l->cs);
}