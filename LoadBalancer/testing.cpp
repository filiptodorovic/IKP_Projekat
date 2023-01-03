#include "testing.h"
#include "queue.h"
#include "hash_map.h"
#pragma warning(disable:4996)


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
void test_dynamic_enqueue_dequeue() {
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