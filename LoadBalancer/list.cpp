#include "list.h"
#define  _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

struct list* head = NULL;
struct list* current = NULL;
struct list* tail = NULL;
CRITICAL_SECTION cs;

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