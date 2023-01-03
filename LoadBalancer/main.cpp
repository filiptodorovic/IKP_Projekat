#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"
#include "hash_map.h"
#include "queue.h"
#include "ClientCommunication.h"
#include "list.h"


#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define SERVER_PORT 5059
#define LISTENER_WORKER_PORT 6069
#define BUFFER_SIZE 256

#pragma warning(disable:4996)

static int client_count=0;
queue* q;
node* free_workers_list;
node* busy_workers_list;

// TCP server that use blocking sockets
int main_tcp()
{
    return 0;
}



int main() {

    //test_list();

    // Create a listener client thread which handles incoming connections
    HANDLE hListenerClient;
    DWORD listenerClientID;
    init_hash_table();
    hListenerClient = CreateThread(NULL, 0, &listenerClient, (LPVOID)0, 0, &listenerClientID);


    //wait for listener to finish
    if (hListenerClient)
        WaitForSingleObject(hListenerClient, INFINITE);


	return 0;
}

DWORD WINAPI test(LPVOID param) {

    //printf("created thread for lists");
    return 0;

}

void test_list() {

    HANDLE hSlobodni1, hSlobodni2, hSlobodni3, hZauzeti1, hZauzeti2, hZauzeti3;
    DWORD hSlobodni1ID, hSlobodni2ID, hSlobodni3ID, hZauzeti1ID, hZauzeti2ID, hZauzeti3ID;

    hSlobodni1 = CreateThread(NULL, 0, &test, NULL, 0, &hSlobodni1ID);
    hSlobodni2 = CreateThread(NULL, 0, &test, NULL, 0, &hSlobodni2ID);
    hSlobodni3 = CreateThread(NULL, 0, &test, NULL, 0, &hSlobodni3ID);
    SetThreadDescription(hSlobodni1, L"Slobodni Thread 1");
    SetThreadDescription(hSlobodni2, L"Slobodni Thread 2");
    SetThreadDescription(hSlobodni3, L"Slobodni Thread 3");

    hZauzeti1 = CreateThread(NULL, 0, &test, NULL, 0, &hZauzeti1ID);
    hZauzeti2 = CreateThread(NULL, 0, &test, NULL, 0, &hZauzeti2ID);
    hZauzeti3 = CreateThread(NULL, 0, &test, NULL, 0, &hZauzeti3ID);
    SetThreadDescription(hZauzeti1, L"Zauzeti Thread 1");
    SetThreadDescription(hZauzeti2, L"Zauzeti Thread 2");
    SetThreadDescription(hZauzeti3, L"Zauzeti Thread 3");

    node_t* listSlobodni_head = NULL;
    node_t* listSlobodni_tail = NULL;

    node_t* listZauzeti_head = NULL;
    node_t* listZauzeti_tail = NULL;

    createList(&listSlobodni_head, &listSlobodni_tail);
    createList(&listZauzeti_head, &listZauzeti_tail);
    
    insertLastNode(hSlobodni1, &listSlobodni_head, &listSlobodni_tail);
    insertLastNode(hSlobodni2, &listSlobodni_head, &listSlobodni_tail);
    insertFirstNode(hSlobodni3, &listSlobodni_head, &listSlobodni_tail);


    insertLastNode(hZauzeti1, &listZauzeti_head, &listZauzeti_tail);
    insertLastNode(hZauzeti2, &listZauzeti_head, &listZauzeti_tail);
    insertLastNode(hZauzeti3, &listZauzeti_head, &listZauzeti_tail);

    printf("\nLista slobodnih: ");
    print_list(listSlobodni_head);
    deleteNode(hSlobodni2, &listSlobodni_head, &listSlobodni_tail);
    
    printf("\nLista slobodnih nakon brisanja: ");
    print_list(listSlobodni_head);

    deleteNode(hSlobodni1, &listSlobodni_head, &listSlobodni_tail);
    printf("\nLista slobodnih nakon brisanja: ");
    print_list(listSlobodni_head);

    deleteNode(hSlobodni3, &listSlobodni_head, &listSlobodni_tail);
    deleteNode(hSlobodni1, &listSlobodni_head, &listSlobodni_tail);

    printf("\nLista zauzetih: ");
    print_list(listZauzeti_head);

        // Accept new connections from clients 
        acceptedSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

}

        printf("\nNew worker accepted. Worker address: %s : %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        //create a new thread for a new client connected
        HANDLE hWorker;
        DWORD workerID;
        hWorker = CreateThread(NULL, 0, &worker_thread, (LPVOID)acceptedSocket, 0, &workerID);


    } while (true);

    // Shutdown the connection since we're done
    iResult = shutdown(acceptedSocket, SD_BOTH);

    // Check if connection is succesfully shut down.
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(acceptedSocket);
        WSACleanup();
        return 1;
    }

    //Close listen and accepted sockets
    closesocket(listenSocket);
    closesocket(acceptedSocket);

    // Deinitialize WSA library
    WSACleanup();

    return 0;
}
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

    // Create a listener client thread which handles incoming connections
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
    hListenerClient = CreateThread(NULL, 0, &checkPercentage, (LPVOID)0, 0, &percentageID);
    hListenerClient = CreateThread(NULL, 0, &listenerClient, (LPVOID)0, 0, &listenerClientID);



    //wait for listener to finish
    if (hListenerClient)
        WaitForSingleObject(hListenerClient, INFINITE);
	return 0;
}

