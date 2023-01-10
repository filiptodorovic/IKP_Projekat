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

extern queue* q;

void create_queue(int capacity);
int is_queue_full();
int is_queue_empty();
void enqueue(char* message);
void dequeue(char* message);
void print_queue();
int get_current_size_queue();
int get_capacity_queue();
void delete_queue();
