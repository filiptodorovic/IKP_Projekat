#include <stdlib.h>
#include <stdio.h>
#include "hash_map.h"
#include "queue.h"
#include "client_communication.h"
#include "list.h"
#include "testing.h"

#pragma warning(disable:4996)

queue* q;
list* free_workers_list;
list* busy_workers_list;


DWORD WINAPI checkPercentage(LPVOID param) {
    while (true) {
        Sleep(3000);
        int fullfillness = ((float)get_current_size(q) / (float)get_capacity(q)) * 100;
        printf("Queue is at %d%%\n", fullfillness);
        if (fullfillness < 30) {
            //shut down worker threads
        }
        else if (fullfillness > 70) {
            // open new worker processes
        }
    }
}

int main() {

    HANDLE hListenerClient;
    HANDLE hListenerWorker;
    HANDLE hPercentage;
    HANDLE hDispatcher;

    DWORD listenerClientID;
    DWORD percentageID;
    DWORD listenerWorkerID;
    DWORD dispatcherID;

    init_hash_table();
    q = create_queue(8);
    init_list(&free_workers_list);
    init_list(&busy_workers_list);



    hPercentage = CreateThread(NULL, 0, &checkPercentage, (LPVOID)0, 0, &percentageID);
    hListenerClient = CreateThread(NULL, 0, &listenerClient, (LPVOID)q, 0, &listenerClientID);



    //wait for listener to finish
    if (hListenerClient)
        WaitForSingleObject(hListenerClient, INFINITE);

    delete_hashtable();
    delete_list(free_workers_list);
    delete_list(busy_workers_list);
    delete_queue(q);
	return 0;
}

