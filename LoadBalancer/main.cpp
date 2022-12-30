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
#define BUFFER_SIZE 256

#pragma warning(disable:4996)

static int client_count=0;

// TCP server that use blocking sockets
int main_tcp()
{
    return 0;
}

void test_ht();
void test_hashing();
void test_dynamic_enqueue_dequeue();
void test_list();
DWORD WINAPI producer(LPVOID param);
DWORD WINAPI consumer(LPVOID param);


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


}

void test_ht() {
    client_thread cl1, cl2, cl3, cl4, cl5, cl6, cl7, cl8, cl9, cl10;
    strcpy(cl1.clientName, "Client1");
    strcpy(cl2.clientName, "Client2");
    strcpy(cl3.clientName, "Client3");
    strcpy(cl4.clientName, "Client4");
    strcpy(cl5.clientName, "Client5");
    strcpy(cl6.clientName, "Client6");
    strcpy(cl7.clientName, "Client7");
    strcpy(cl8.clientName, "Client8");
    strcpy(cl9.clientName, "Client9");
    strcpy(cl10.clientName, "Client10");

    init_hash_table();

    insert_client(&cl1);
    insert_client(&cl2);
    insert_client(&cl3);
    insert_client(&cl4);
    insert_client(&cl5);
    insert_client(&cl6);
    insert_client(&cl7);
    insert_client(&cl8);
    insert_client(&cl9);
    insert_client(&cl10);

    print_table();
}
void test_hashing() {
    char str[6];
    strcpy(str, "CL10");
    printf("ABC10 => %d\n", hash(str));
    strcpy(str, "ABC11");
    printf("ABC11 => %d\n", hash(str));
    strcpy(str, "GF12");
    printf("ABC12 => %d\n", hash(str));
    strcpy(str, "GSDA");
    printf("ABC13 => %d\n", hash(str));
    strcpy(str, "GDASG");
    printf("ABC14 => %d\n", hash(str));
}
DWORD WINAPI producer(LPVOID param) {
    queue* q = (queue*)param;
    char data_a[10] = "ABC";
    while (true) {
        enqueue(q, data_a);
        Sleep(300);
    }
}
DWORD WINAPI consumer(LPVOID param) {
    queue* q = (queue*)param;
    while (true) {
        char buffer[10];
        Sleep(500);
        dequeue(q, buffer);
    }
}
void test_dynamic_enqueue_dequeue(){
    queue* q = create_queue(7);

    HANDLE hProducer;
    HANDLE hConsumer;
    DWORD ProducerID;
    DWORD ConsumerID;

    hProducer = CreateThread(NULL, 0, &producer, (LPVOID)q, 0, &ProducerID);
    hConsumer = CreateThread(NULL, 0, &consumer, (LPVOID)q, 0, &ConsumerID);


    if (hConsumer)
        WaitForSingleObject(hConsumer, INFINITE);
    if (hProducer)
        WaitForSingleObject(hProducer, INFINITE);
}