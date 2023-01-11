#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#pragma warning(disable:4996)


#define SERVER_IP_ADDRESS "127.0.0.1"
#define SERVER_PORT 6069
#define BUFFER_SIZE 512

typedef struct messageStruct {
    char clientName[10];
    char messageBuffer[256];
}messageStruct;

// TCP client that use blocking sockets
int main()
{
    // Socket used to communicate with server
    SOCKET connectSocket = INVALID_SOCKET;

    // Variable used to store function return value
    int iResult;

    // Buffer we will use to store message
    char dataBuffer[BUFFER_SIZE];

    // WSADATA data structure that is to receive details of the Windows Sockets implementation
    WSADATA wsaData;

    // Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("[WORKER]: WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    // create a socket
    connectSocket = socket(AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP);

    if (connectSocket == INVALID_SOCKET)
    {
        printf("[WORKER]: socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Create and initialize address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;								// IPv4 protocol
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);	// ip address of server
    serverAddress.sin_port = htons(SERVER_PORT);					// server port

    // Connect to server specified in serverAddress and socket connectSocket
    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("[WORKER]: Unable to connect to server.\n");
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    u_long non_blocking = 1;
    ioctlsocket(connectSocket, FIONBIO, &non_blocking);
    char answerToLB[256];

    while (true) {

        iResult = recv(connectSocket, dataBuffer, (int)strlen(dataBuffer), 0);
        if (iResult != SOCKET_ERROR)	// Check if message is successfully received
        {
            dataBuffer[iResult] = '\0';

            messageStruct* recievedMessage = (messageStruct*)dataBuffer;


            // Log message text
            printf("[WORKER]: %s sent: %s.\n",recievedMessage->clientName, recievedMessage->messageBuffer);
            // Send message to server using connected socket

            while (true) {
                //sprintf(answerToLB.messageBuffer, "Success writing->%s",recievedMessage->messageBuffer);
                //strcpy(answerToLB.clientName, recievedMessage->clientName);
                sprintf(answerToLB, "Success! %s: %s ", recievedMessage->clientName, recievedMessage->messageBuffer);
                iResult = send(connectSocket, answerToLB, strlen(answerToLB), 0);
                if (iResult != SOCKET_ERROR)	// Check if message is successfully received
                {
                    printf("[WORKER]: returned to %s: %s\n", recievedMessage->clientName,recievedMessage->messageBuffer);
                    break;
                }
                else	// There was an error during recv
                {

                    if (WSAGetLastError() == WSAEWOULDBLOCK) {
                        continue;
                    }
                    else {
                        printf("[WORKER]: send failed with error: %d\n", WSAGetLastError());
                        break;
                    }

                }
            }

            // Check result of send function
            if (iResult == SOCKET_ERROR)
            {
                printf("[WORKER]: send failed with error: %d\n", WSAGetLastError());
                closesocket(connectSocket);
                WSACleanup();
                return 1;
            }

        }
        else	// There was an error during recv
        {

            if (WSAGetLastError() == WSAEWOULDBLOCK) {
                continue;
            }
            else {
                printf("[WORKER]: recv failed with error: %d\n", WSAGetLastError());
                break;
            }

        }

    }

    // Shutdown the connection since we're done
    iResult = shutdown(connectSocket, SD_BOTH);

    // Check if connection is succesfully shut down.
    if (iResult == SOCKET_ERROR)
    {
        printf("[WORKER]: Shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    // For demonstration purpose
    printf("\nPress any key to exit: ");
    _getch();


    // Close connected socket
    closesocket(connectSocket);

    // Deinitialize WSA library
    WSACleanup();

    return 0;
}
