#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <windows.h> 

// A structure to represent a queue
typedef struct {
	CRITICAL_SECTION cs;
	int front, rear, currentSize;
	int capacity;
	char** messageArray;
}queue;

queue* create_queue(int capacity);
int is_full(queue* q);
int is_empty(queue* q);
void enqueue(queue* q, char* message);
void dequeue(queue* q,char* message);
void print_queue(queue* q);
int current_size(queue* q);
