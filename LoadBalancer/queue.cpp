#include "queue.h"
#define  _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#include "hash_map.h"

HANDLE hSemaporeQueueEmpty;
HANDLE hSemaporeQueueFull;

#define DEBUG
#define BUFFER_SIZE 256


void create_queue(int capacity) {
	queue* newQueue = (queue*)malloc(sizeof(queue));
	newQueue->front = 0;
	newQueue->rear = capacity-1;
	newQueue->capacity = capacity;
	newQueue->currentSize = 0;

	//create array of messages
	newQueue->messageArray = (char**)malloc(sizeof(char*) * capacity);

	for (int i = 0; i < capacity; i++) {
		newQueue->messageArray[i] = NULL;
	}

	InitializeCriticalSection(&newQueue->cs);
	hSemaporeQueueEmpty = CreateSemaphore(0, capacity, capacity, NULL); //empty slots
	hSemaporeQueueFull = CreateSemaphore(0, 0, capacity, NULL); //taken slots

	q = newQueue;
}

int is_queue_full()
{
	return (q->currentSize == q->capacity);
}

int is_queue_empty()
{
	return (q->currentSize == 0);
}

int get_current_size_queue() {
	return q->currentSize;
}

int get_capacity_queue() {
	return q->capacity;
}

void enqueue(char* message) {
	while (is_queue_full()) {
		if (WaitForSingleObject(hSemaporeQueueEmpty, INFINITE) == WAIT_OBJECT_0 + 1)
			break;
	}
	{
		EnterCriticalSection(&q->cs);
		//The front and rear will go around like in a circular buffer

		int message_size = CLIENT_NAME_LEN + strlen(message + CLIENT_NAME_LEN)+1;

		q->rear = (q->rear + 1) % q->capacity;
		q->messageArray[q->rear] = (char*)malloc(message_size);

		//Write at that place the new message
		memcpy(q->messageArray[q->rear],message, message_size);
		q->currentSize++;
		#ifdef DEBUG
		print_queue();
		#endif
		ReleaseSemaphore(hSemaporeQueueFull, 1, NULL); // increase the number of filled spots
		LeaveCriticalSection(&q->cs);
	}

}

void dequeue(char* message) {
	while (q->currentSize == 0) {
		if (WaitForSingleObject(hSemaporeQueueFull, INFINITE) == WAIT_OBJECT_0 + 1)
			break;//The queue is full, wait for elements to be dequeued
	}

	{
		EnterCriticalSection(&q->cs);

		int message_size = CLIENT_NAME_LEN + strlen(q->messageArray[q->front] + CLIENT_NAME_LEN) + 1;

		memcpy(message, q->messageArray[q->front],message_size);

		free(q->messageArray[q->front]);
		q->messageArray[q->front] = NULL;
		q->front = (q->front + 1) % q->capacity;
		q->currentSize--;
		if (q->currentSize == 0) {
			q->front = 0;
			q->rear = q->capacity - 1;
		}
		#ifdef DEBUG
		print_queue();
		#endif
		ReleaseSemaphore(hSemaporeQueueEmpty, 1, NULL); // increase the number of empty
		LeaveCriticalSection(&q->cs);
	}
}

void delete_queue() {
	for (int i = q->front; i <= q->rear; i++) {
		free(q->messageArray[i]);
	}
	free(q->messageArray);
	DeleteCriticalSection(&q->cs);
	free(q);
	CloseHandle(hSemaporeQueueEmpty);
	CloseHandle(hSemaporeQueueFull);
}

void print_queue() {
	printf("\n QUEUE: ");
	for (int i = q->front; q->messageArray[i]!=NULL;) {
		if (i == q->capacity)
			i = i % q->capacity;
		char clientName[CLIENT_NAME_LEN];
		char message[BUFFER_SIZE];

		memcpy(clientName, q->messageArray[i], CLIENT_NAME_LEN);

		printf(" %s->%s", clientName, (q->messageArray[i] + CLIENT_NAME_LEN));
		if (i == q->rear)break;
		++i;
	}
	printf("\n");
}