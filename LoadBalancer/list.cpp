#include "list.h"
#pragma warning(disable:4996)

void initList(list* l) {
	l->head = NULL;
	l->tail = NULL;
	InitializeCriticalSection(&l->cs);
}

void insertFirstNode(HANDLE data, list *l) {
	EnterCriticalSection(&l->cs);
	node* new_node = (node*)malloc(sizeof(node));
	new_node->thread_handle = data;
	new_node->next = l->head;

	// If the list is empty, set the tail to the new node
	if (l->tail == NULL) {
		l->tail = new_node;
	}

	// Set the head to the new node
	l->head = new_node;
	LeaveCriticalSection(&l->cs);
}

void insertLastNode(HANDLE data, list *l) {
	EnterCriticalSection(&l->cs);
	node* new_node = (node*)malloc(sizeof(node));

	new_node->thread_handle = data;
	new_node->next = NULL;

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

void deleteNode(HANDLE data, list *l) {
	EnterCriticalSection(&l->cs);
	node* current = l->head;
	node* previous = NULL;

	// Search for the node to delete
	while (current != NULL && current->thread_handle != data) {
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
	node* current = l->head;
	while (current != NULL) {

		WCHAR* thread_name = NULL;
		GetThreadDescription(current->thread_handle, &thread_name);
		printf("Thread name: %ls\n", thread_name);
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