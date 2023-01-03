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

DWORD WINAPI read_write_thread(LPVOID param) {
    SOCKET acceptedSocket = (SOCKET)param;
    char dataBuffer[BUFFER_SIZE];
    int client_num = client_count++;


    //check if we got data from client or EXIT signal
    //OR if we got a message from worker
    do
    {
        // Receive data until the client shuts down the connection
        int iResult = recv(acceptedSocket, dataBuffer, BUFFER_SIZE, 0);

        if (iResult > 0)	// Check if message is successfully received
        {
            dataBuffer[iResult] = '\0';

            if (strcmp(dataBuffer, "exit") == 0) {
                // Connection was closed successfully
                printf("Connection with client %d closed.\n",client_num);
                closesocket(acceptedSocket);
                break;
            }

            // Log message text
            printf("Client %d sent: %s.\n",client_num, dataBuffer);
            enqueue(q, dataBuffer);

        }
        else if (iResult == 0)	// Check if shutdown command is received
        {
            printf("Connection with client closed.\n");
            closesocket(acceptedSocket);
            break;
        }
        else	// There was an error during recv
        {

            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(acceptedSocket);
            break;
        }

    } while (true);
}
DWORD WINAPI worker_thread(LPVOID param) {

    return 0;
}


DWORD WINAPI listenerClient(LPVOID param) {

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

    printf("Server socket is set to listening mode. Waiting for new connection requests.\n");
    
    do
    {
        // Struct for information about connected client
        sockaddr_in clientAddr;

        int clientAddrSize = sizeof(struct sockaddr_in);

        // Accept new connections from clients 
        acceptedSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

        // Check if accepted socket is valid 
        if (acceptedSocket == INVALID_SOCKET)
        {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        printf("\nNew client request accepted. Client address: %s : %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        //create a new thread for a new client connected
        HANDLE hClient;
        DWORD ClientID;
        hClient = CreateThread(NULL, 0, &read_write_thread, (LPVOID)acceptedSocket, 0, &ClientID);

        //add it to the hash table
        client_thread* newCli = (client_thread*)malloc(sizeof(client_thread));
        sprintf(clientName,"Client%d", client_count);
        strcpy(newCli->clientName, clientName);
        newCli->clientThread = hClient;
        newCli->finished = false;

        
        insert_client(newCli);
        //print_table();


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
DWORD WINAPI listenerWorker(LPVOID param) {

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
    serverAddress.sin_port = htons(LISTENER_WORKER_PORT);	// Use specific port


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

    printf("Server socket is set to listening mode. Waiting for new connection requests.\n");

    do
    {
        // Struct for information about connected client
        sockaddr_in clientAddr;

        int clientAddrSize = sizeof(struct sockaddr_in);

        // Accept new connections from clients 
        acceptedSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

        // Check if accepted socket is valid 
        if (acceptedSocket == INVALID_SOCKET)
        {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return 1;
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

