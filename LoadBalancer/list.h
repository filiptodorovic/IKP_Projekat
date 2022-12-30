#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "conio.h"
#include <windows.h>

struct list {
	HANDLE data;
	int key;
	struct list* next;
};

void printList(struct list* ptr);   //pointer on head
struct list* deleteFirst();
struct list* insertFirst(int key, HANDLE data);
struct list* insertLast(int key, HANDLE data);
bool isEmpty();
struct list* find(int key);
struct list* deleteElem(int key);



