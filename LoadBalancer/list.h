#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <winnt.h>
#include <rpcdcep.h>
#include "conio.h"
#include <windows.h>

struct node {
	HANDLE data;
	int key;
	struct node* next;
};

struct node* head = NULL;
struct node* current = NULL;
struct node* tail = NULL;

void printList();
struct node* deleteFirst();
void insertFirst(int key, HANDLE data);
void insertLast(int key, HANDLE data);
bool isEmpty();
struct node* find(int key);
struct node* deleteElem(int key);


#pragma once
