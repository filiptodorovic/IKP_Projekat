#include <stdlib.h>
#include <stdio.h>
#include "hash_map.h"
#include "queue.h"
#include "client_communication.h"
#include "worker_communication.h"
#include "list.h"
#include "testing.h"

#define WORKER_IP_ADDRESS "127.0.0.1"
#define WORKER_PORT 6069

#pragma warning(disable:4996)

queue* q;
list* free_workers_list;
list* busy_workers_list;


DWORD WINAPI check_percentage(LPVOID param) {
    while (true) {
        Sleep(3000);
        int fullfillness = ((float)get_current_size(q) / (float)get_capacity(q)) * 100;
        printf("Queue is at %d%%\n", fullfillness);
        if (fullfillness < 30) {
            //shut down worker threads
        }
        else if (fullfillness > 70) {
            // open new worker processes
            //CreateProcess();
        }
    }
}

DWORD WINAPI dispatcher(LPVOID param) {

    char* message;

    while (true) {
        Sleep(3000);

        if (is_empty(q)) {
            printf("D: Queue is empty.\n");
        }
        else {
            int i = q->front;
            message = q->messageArray[i];
            node* first = free_workers_list->head;

            //insert_first_node(first->thread_handle, busy_workers_list);
            //delete_node(first->thread_handle, free_workers_list);

            //ovde treba uzeti port sa first node elementa
            
            // Socket used to communicate with server
            SOCKET connectSocket = INVALID_SOCKET;
            // Variable used to store function return value
            int iResult;
            WSADATA wsaData;

            // Initialize windows sockets library for this process
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            {
                printf("WSAStartup failed with error: %d\n", WSAGetLastError());
                return 1;
            }

            // create a socket
            connectSocket = socket(AF_INET,
                SOCK_STREAM,
                IPPROTO_TCP);

            if (connectSocket == INVALID_SOCKET)
            {
                printf("socket failed with error: %ld\n", WSAGetLastError());
                WSACleanup();
                return 1;
            }

            //WORKER_PORT preuzeti sa first node elementa iz liste
            sockaddr_in serverAddress;
            serverAddress.sin_family = AF_INET;								// IPv4 protocol
            serverAddress.sin_addr.s_addr = inet_addr(WORKER_IP_ADDRESS);	// ip address of server
            serverAddress.sin_port = htons(WORKER_PORT);					// server port

            // Connect to server specified in serverAddress and socket connectSocket
            if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
            {
                printf("Unable to connect to server.\n");
                closesocket(connectSocket);
                WSACleanup();
                return 1;
            }

            u_long non_blocking = 1;
            ioctlsocket(connectSocket, FIONBIO, &non_blocking);

            iResult = send(connectSocket, message, (int)strlen(message), 0);

            if (iResult == SOCKET_ERROR)
            {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(connectSocket);
                WSACleanup();
                return 1;
            }

            printf("Message successfully sent. Total bytes: %ld\n", iResult);
        }
    }
    

    return 0;
}

void create_new_worker_process() {
    STARTUPINFO startup_info;
    PROCESS_INFORMATION process_info;
    memset(&startup_info, 0, sizeof(STARTUPINFO));
    startup_info.cb = sizeof(STARTUPINFO);
    memset(&process_info, 0, sizeof(PROCESS_INFORMATION));
    TCHAR buff[100];
    GetCurrentDirectory(100, buff);
    wcscat(buff, L"\\..\\Debug\\Worker.exe");

    if (!CreateProcess(
        buff,          // LPCTSTR lpApplicationName
        NULL,                // LPTSTR lpCommandLine
        NULL,                // LPSECURITY_ATTRIBUTES lpProcessAttributes
        NULL,                // LPSECURITY_ATTRIBUTES lpThreadAttributes
        FALSE,               // BOOL bInheritHandles
        CREATE_NO_WINDOW,    // DWORD dwCreationFlags
        NULL,                // LPVOID lpEnvironment
        NULL,                // LPCTSTR lpCurrentDirectory
        &startup_info,       // LPSTARTUPINFO lpStartupInfo
        &process_info        // LPPROCESS_INFORMATION lpProcessInformation
    )) {

        printf("CreateProcess failed (%d).\n", GetLastError());
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



    hPercentage = CreateThread(NULL, 0, &check_percentage, (LPVOID)0, 0, &percentageID);
    hListenerClient = CreateThread(NULL, 0, &client_listener, (LPVOID)q, 0, &listenerClientID);
    hListenerWorker = CreateThread(NULL, 0, &worker_listener, (LPVOID)0, 0, &listenerWorkerID);
    hDispatcher = CreateThread(NULL, 0, &dispatcher, (LPVOID)0, 0, &dispatcherID);

    create_new_worker_process();
    create_new_worker_process();

    //wait for listener to finish
    if (hListenerClient)
        WaitForSingleObject(hListenerClient, INFINITE);
    if (hListenerWorker)
        WaitForSingleObject(hListenerWorker, INFINITE);
    if (hPercentage)
        WaitForSingleObject(hPercentage, INFINITE);
    if (hDispatcher)
        WaitForSingleObject(hDispatcher, INFINITE);

    delete_hashtable();
    delete_list(free_workers_list);
    delete_list(busy_workers_list);
    delete_queue(q);
	return 0;
}

