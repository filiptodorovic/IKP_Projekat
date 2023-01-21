#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <WinSock2.h>
#include "worker_communication.h"
#include "queue.h"
#include "hash_map.h"
#include "list.h"
#include <string.h>

#pragma warning(disable:4996)
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define BUFFER_SIZE 256
#define CLIENT_NAME_LEN 10 
#define SERVER_PORT 6069

static int worker_count = 0;

DWORD WINAPI worker_write(LPVOID param) {

    node* new_node = (node*)param;

    SOCKET acceptedSocket = new_node->acceptedSocket;
    HANDLE msgSemaphore = new_node->msgSemaphore;

    //char dataBuffer[BUFFER_SIZE];
    int worker_num = worker_count++;
    int iResult;
    
    while (true) {
        
        //check if we got data from client or EXIT signal
        //OR if we got a message from worker
        
        WaitForSingleObject(msgSemaphore, INFINITE);
        //The queue is full, wait for elements to be dequeued


        messageStruct* msg = new_node->msgStruct;
        char messageBuff[BUFFER_SIZE+1]; // 256+1 for :
        char msgLen;

        memset(messageBuff, 0, BUFFER_SIZE);// zero the buffer
        char message[BUFFER_SIZE + 1];

        sprintf(message, "%s:%s", msg->clientName, msg->bufferNoName);
        msgLen = strlen(msg->clientName) + strlen(msg->bufferNoName) + 1 + 1; // client+message+delimiter+messageLen
        
        memset(messageBuff, msgLen, 1); // first byte is the length of the message
        strcpy(messageBuff + 1, message);

        iResult = send(acceptedSocket, messageBuff, strlen(messageBuff+1)+1, 0); // [messageLen][ClientName][delimiter(:)][message]

        if (iResult != SOCKET_ERROR)
        {
            printf("[WORKER WRITE]: sent: %s.\n", messageBuff);
            if (strstr(msg->bufferNoName, "exit") != NULL) {

                printf("[WORKER WRITE]: Worker process signing off.\n");
                //TerminateThread(GetCurrentThread(), 0);
                //free(new_node);
                break;
            }

        }
        else
        {

            if (WSAGetLastError() == WSAEWOULDBLOCK) { //the recieve would block, continue
                continue;
            }
            else {
                printf("[WORKER WRITE]: send failed with error: %d\n", WSAGetLastError());
                break;
            }

        }
       
    }
    return 0;
}

DWORD WINAPI worker_read(LPVOID param) {
    node* new_node = (node*)param;
    SOCKET acceptedSocket = new_node->acceptedSocket;

    char dataBuffer[BUFFER_SIZE+9+1];
    int worker_num = worker_count++;

    //check if we got data from client or EXIT signal
    //OR if we got a message from worker
    do
    {

        int iResult = recv(acceptedSocket, dataBuffer, BUFFER_SIZE, 0);
        int msgLen = (int)dataBuffer[0];

        if(iResult != SOCKET_ERROR)
        {
            int iResult2 = 0;
            char messagePart[BUFFER_SIZE];//delimiter + success

            while (iResult != msgLen) {

                iResult2 = recv(acceptedSocket, messagePart, BUFFER_SIZE, 0);
                //strcpy(dataBuffer + iResult, dataBuffer2);
                memcpy(dataBuffer + strlen(dataBuffer + 1) + 1, messagePart, (int)strlen(messagePart));

                iResult += iResult2;
            }

            dataBuffer[iResult] = '\0';

            printf("[WORKER READ] Worker sent: %s.\n", dataBuffer);
            if (strstr(dataBuffer+1, ":exit") != NULL) {
                printf("[WORKER READ] Worker sent exit. Worker proccess signing off.\n");
                TerminateThread(new_node->thread_write, 0);
                TerminateThread(GetCurrentThread(), 0);
                delete_node(new_node, busy_workers_list);
                free(new_node->msgStruct);
                free(new_node);
                //return 0;
            }

            char clientName[CLIENT_NAME_LEN];
            memset(clientName, 0, CLIENT_NAME_LEN);
            //strcpy(clientName, strstr(dataBuffer, "Client"));
            int a;
            sscanf(dataBuffer+1, "Success->%[^:]:",clientName);
            printf("%s\n",clientName );

            char bufferForClient[BUFFER_SIZE+CLIENT_NAME_LEN];
            //if(strcmp(dataBuffer, "exit") == 0)
            strcpy(bufferForClient, dataBuffer);

            client_thread* foundClient = lookup_client(clientName);
            if (foundClient) {

                iResult = send(foundClient->acceptedSocket, bufferForClient, (int)strlen(bufferForClient), 0);
                memset(bufferForClient, 0, BUFFER_SIZE);
                memset(clientName, 0, sizeof(clientName));

                if (iResult != SOCKET_ERROR)	// Check if message is successfully received
                {
                    printf("[WORKER]: returned to client: %s\n", dataBuffer);

                    //move_specific_node(free_workers_list, busy_workers_list, new_node);
                    // we will kick out the node from the busy list
                    delete_node(new_node, busy_workers_list);

                    // and isert it to the end of the free worker list
                    insert_last_node(new_node, free_workers_list);

                    continue;
                }
                else	// There was an error during recv
                {

                    if (WSAGetLastError() == WSAEWOULDBLOCK) {
                        continue;
                    }
                    else {
                        printf("[WORKER]: send to client failed with error: %d\n", WSAGetLastError());
                        break;
                    }

                }
            }

            // Send the message to the client...

        }
        else
        {

            if (WSAGetLastError() == WSAEWOULDBLOCK) { //the recieve would block, continue
                continue;
            }
            else {
                printf("[WORKER READ]: recv failed with error: %d\n", WSAGetLastError());
                //TerminateThread(GetCurrentThread(), 0);
                break;
            }

        }

    } while (true);

    return 0;
}

//dva nova threada - 1 koji prima (receive - recv od workera i salje odg klijentu) i 1 koji salje poruke na worker (send - ceka na notify od dispatchera, cita iz msgBuffera i salje na worker)
DWORD WINAPI worker_listener(LPVOID param) {
    // Socket used for listening for new clients 
    SOCKET listenSocket = INVALID_SOCKET;

    // Socket used for communication with client
    SOCKET acceptedSocket = INVALID_SOCKET;

    // Variable used to store function return value
    int iResult;

    // Buffer used for storing incoming data
    char dataBuffer[BUFFER_SIZE];
    char clientName[CLIENT_NAME_LEN];

    // WSADATA data structure that is to receive details of the Windows Sockets implementation
    WSADATA wsaData;

    // Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }


    // Initialize serverAddress structure used by bind
    sockaddr_in serverAddress;
    memset((char*)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;				// IPv4 address family
    serverAddress.sin_addr.s_addr = INADDR_ANY;		// Use all available addresses
    serverAddress.sin_port = htons(SERVER_PORT);	// Use specific port


    // Create a SOCKET for connecting to server
    listenSocket = socket(AF_INET,      // IPv4 address family
        SOCK_STREAM,  // Stream socket
        IPPROTO_TCP); // TCP protocol

// Check if socket is successfully created
    if (listenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket - bind port number and local address to socket
    iResult = bind(listenSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    // Check if socket is successfully binded to address and port from sockaddr_in structure
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Set listenSocket in listening mode
    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("WORKER socket is set to listening mode. Waiting for new connection requests.\n");
    static int worker_thread_cnt = 0;

    unsigned long nb_listen = 1;
    ioctlsocket(listenSocket, FIONBIO, &nb_listen);

    do
    {
        if (WaitForSingleObject(semaphoreEnd, 10) == WAIT_OBJECT_0)
            break;
        // Struct for information about connected client
        sockaddr_in clientAddr;

        int clientAddrSize = sizeof(struct sockaddr_in);

        // Accept new connections from clients 
        acceptedSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

        if (acceptedSocket != SOCKET_ERROR) {
            // Neblokirajuca operacija koja je uspesno izvrsena

            printf("\n[WORKER THREAD]: New Worker accepted. Worker address: %s : %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

            //create a new thread for a new client connected
            //create thread za workere prima ceo element liste ne samo accepted socket!!!
            HANDLE hWorkerWrite, hWorkerRead;
            DWORD workerWID, workerRID;

            unsigned long non_blocking = 1;
            ioctlsocket(acceptedSocket, FIONBIO, &non_blocking);

            node* new_node = (node*)malloc(sizeof(node));
            new_node->msgSemaphore = CreateSemaphore(0, 0, 1, NULL);
            new_node->msgStruct = (messageStruct*)malloc(sizeof(messageStruct));
            new_node->acceptedSocket = acceptedSocket;
            new_node->thread_write = CreateThread(NULL, 0, &worker_write, (LPVOID)new_node, 0, &workerWID);
            new_node->thread_read = CreateThread(NULL, 0, &worker_read, (LPVOID)new_node, 0, &workerRID);
            new_node->next = NULL;

            TCHAR workerReadDescription[20];
            TCHAR workerWriteDescription[20];
            wsprintfW(workerReadDescription, L"WRR %d", worker_thread_cnt);
            wsprintfW(workerWriteDescription, L"WRW %d", worker_thread_cnt);

            SetThreadDescription(new_node->thread_read, workerReadDescription);
            SetThreadDescription(new_node->thread_write, workerWriteDescription);

            insert_last_node(new_node, free_workers_list);
            worker_thread_cnt++;

            //Put accepted socket/thread in the FREE LIST
        }
        else
        {
            if (WSAGetLastError() == WSAEWOULDBLOCK) {
                // U pitanju je blokirajuca operacija koja zbog rezima
                // soketa nece biti izvrsena 
                continue;
            }
            else {
                printf("[WORKER]: accept failed with error: %d\n", WSAGetLastError());
                break;
                // Desila se neka druga greska prilikom poziva operacije
            }
        }


    } while (true);

    // Shutdown the connection since we're done
    iResult = shutdown(acceptedSocket, SD_BOTH);

    //// Check if connection is succesfully shut down.
    //if (iResult == SOCKET_ERROR)
    //{
    //    printf("shutdown failed with error: %d\n", WSAGetLastError());
    //    closesocket(acceptedSocket);
    //    WSACleanup();
    //    return 1;
    //}

    //Close listen and accepted sockets
    closesocket(listenSocket);
    closesocket(acceptedSocket);

    // Deinitialize WSA library
    WSACleanup();

    return 0;
}