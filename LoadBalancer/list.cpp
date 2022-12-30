#define  _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#include "list.h"

node_t* head = NULL;
node_t* tail = NULL;

void createList(node_t** head, node_t** tail) {
	*head = NULL;
	*tail = NULL;
}

void insertFirstNode(HANDLE data, node_t** head, node_t** tail) {

	node_t* new_node = (node_t*)malloc(sizeof(node_t));
	new_node->thread_handle = data;
	new_node->next = *head;

	// If the list is empty, set the tail to the new node
	if (*tail == NULL) {
		*tail = new_node;
	}

	// Set the head to the new node
	*head = new_node;

}

void insertLastNode(HANDLE data, node_t** head, node_t** tail) {
	node_t* new_node = (node_t *)malloc(sizeof(node_t));

	new_node->thread_handle = data;
	new_node->next = NULL;

	// If the list is empty, set the head and tail to the new node
	if (*head == NULL) {
		*head = new_node;
		*tail = new_node;
	}
	else {
		// Otherwise, add the new node to the end of the list
		(*tail)->next = new_node;
		*tail = new_node;
	}

}

void deleteNode(HANDLE data, node_t** head, node_t** tail) {
	node_t* current = *head;
	node_t* previous = NULL;

	// Search for the node to delete
	while (current != NULL && current->thread_handle != data) {
		previous = current;
		current = current->next;
	}


	// If the node was found, delete it
	if (current != NULL) {
		// If the node to delete is the head of the list, update the head pointer
		if (current == *head) {
			*head = current->next;
		}
		// If the node to delete is the tail of the list, update the tail pointer
		if (current == *tail) {
			*tail = previous;
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


}

void print_list(node_t* head) {
	node_t* current = head;
	while (current != NULL) {

		WCHAR* thread_name = NULL;
		GetThreadDescription(current->thread_handle, &thread_name);
		printf("Thread name: %ls\n", thread_name);
		current = current->next;
	}
	printf("\n");
}

/*
void printList(struct list* ptr) {
	//struct list* ptr = head;
	printf("\n[ ");

	//start from the beginning
	while (ptr != NULL) {
		printf("(thread %d) ", ptr->key);
		ptr = ptr->next;
	}

	printf(" ]");
}

//insert link at the first location
struct list* insertFirst(int key, HANDLE data) {


	struct list* link = (struct list*)malloc(sizeof(struct list));

	//create a link
	//EnterCriticalSection(&cs);

	link -> key = key;
	link -> data = data;

	//point it to old first node
	link -> next = head;

	//point first to new first node
	head = link;

	//LeaveCriticalSection(&cs);

	return head;
}

struct list* insertLast(int key, HANDLE data) {

	struct list* link = (struct list*)malloc(sizeof(struct list));

	EnterCriticalSection(&cs);

	link->key = key;
	link->data = data;

	tail->next = link;

	tail = link;

	LeaveCriticalSection(&cs);

	return tail;

}

//delete first item
struct list* deleteFirst() {

	//save reference to first link
	struct list* tempLink = head;

	EnterCriticalSection(&cs);

	//mark next to first link as first 
	head = head->next;

	LeaveCriticalSection(&cs);

	//return the deleted link
	return tempLink;
}

//is list empty
bool isEmpty() {
	return head == NULL;
}

int length() {
	int length = 0;
	struct list* current;

	for (current = head; current != NULL; current = current->next) {
		length++;
	}

	return length;
}

//find a link with given key
struct list* find(int key) {

	//start from the first link
	struct list* current = head;

	//if list is empty
	if (head == NULL) {
		return NULL;
	}

	//navigate through list
	while (current->key != key) {

		//if it is last node
		if (current->next == NULL) {
			return NULL;
		}
		else {
			//go to next link
			current = current->next;
		}
	}

	//if data found, return the current Link
	return current;
}

//delete a link with given key
struct list* deleteElem(int key) {

	//start from the first link
	struct list* current = head;
	struct list* previous = NULL;

	EnterCriticalSection(&cs);

	//if list is empty
	if (head == NULL) {
		return NULL;
	}

	//navigate through list
	while (current->key != key) {

		//if it is last node
		if (current->next == NULL) {
			return NULL;
		}
		else {
			//store reference to current link
			previous = current;
			//move to next link
			current = current->next;
		}
	}

	//found a match, update the link
	if (current == head) {
		//change first to point to next link
		head = head->next;
	}
	else {
		//bypass the current link
		previous->next = current->next;
	}


	LeaveCriticalSection(&cs);

	return current;
}

*/