#include <stdio.h>
#include <windows.h>

#define BUFFER_SIZE 256
#define CLIENT_NAME_LEN 20
#define SERVER_PORT 5060

DWORD WINAPI workerThread(LPVOID param) {

    SOCKET acceptedSocket = (SOCKET)param;

    char message[] = "hello worker!";

    int iResult;

    // Buffer used for storing incoming data
    char dataBuffer[BUFFER_SIZE];

    memcpy(dataBuffer, message, (int)strlen(message));

    iResult = send(acceptedSocket, message, (int)strlen(dataBuffer), 0);

    return 0;
}


DWORD WINAPI workerListener(LPVOID param) {

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

    printf("Server socket is set to listening mode. Waiting for new connection requests.\n"); //server listens to requests from opened workers

    do {

        // Struct for information about connected client
        sockaddr_in workerAddr;

        int workerAddrSize = sizeof(struct sockaddr_in);

        // Accept new connections from clients 
        acceptedSocket = accept(listenSocket, (struct sockaddr*)&workerAddr, &workerAddrSize);

        // Check if accepted socket is valid 
        if (acceptedSocket == INVALID_SOCKET)
        {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        printf("\nNew worker request accepted. Worker address: %s : %d\n", inet_ntoa(workerAddr.sin_addr), ntohs(workerAddr.sin_port));

        HANDLE workerTr;
        DWORD WorkerID;
        workerTr = CreateThread(NULL, 0, &workerThread, (LPVOID)acceptedSocket, 0, &WorkerID);
        //insertFirst(WorkerID, workerTr);              //PP thread does this??

    } while (1);

}



