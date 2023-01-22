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
#define CLIENT_NAME_LEN 10
#define BUFFER_WITHOUT_NAME 246
#define BUFFER_SIZE CLIENT_NAME_LEN+BUFFER_WITHOUT_NAME
#define RET_BUFFER_SIZE BUFFER_SIZE+9
#define CLIENT_NAME_LEN 10

bool is_socket_ready(SOCKET socket, bool isRead) {
    FD_SET set;
    timeval tv;

    FD_ZERO(&set);
    FD_SET(socket, &set);

    tv.tv_sec = 0;
    tv.tv_usec = 50;

    int iResult;

    if (isRead) { //see if socket is ready for READ
        iResult = select(0, &set, NULL, NULL, &tv);
    }
    else {	//see if socket is ready for WRITE
        iResult = select(0, NULL, &set, NULL, &tv);
    }

    if (iResult <= 0)
        return false;
    else
        return true;
}

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
    const char* successStr= "Success->";

    while (true) {

        while (!is_socket_ready(connectSocket, true)) {

        }

        iResult = recv(connectSocket, dataBuffer, BUFFER_SIZE, 0);
        if (iResult != SOCKET_ERROR)	// Check if message is successfully received
        {
            

            int msgLen = (int)dataBuffer[0];
            char dataBuffer2[RET_BUFFER_SIZE]; //for Success
            memset(dataBuffer2, 0, RET_BUFFER_SIZE);
            int iResult2 = 0;

            while (iResult != msgLen) {

                iResult2 = recv(connectSocket, dataBuffer2, (int)strlen(dataBuffer2), 0);
                //strcpy(dataBuffer + iResult, dataBuffer2);
                memcpy(dataBuffer+strlen(dataBuffer+1)+1, dataBuffer2, (int)strlen(dataBuffer2));

                iResult += iResult2;

            }

            dataBuffer[iResult] = '\0';

            // Log message text
            printf("[WORKER]: load balancer sent: %s.\n", dataBuffer+1);// move to the actual message
            // Send message to server using connected socket

            memset(dataBuffer2, 0, RET_BUFFER_SIZE);

            //while (true) {
                
                strcpy(dataBuffer2 + 1, successStr);
                strcpy(dataBuffer2+1 + strlen(dataBuffer2+1), dataBuffer+1);
                char messageLen = strlen(dataBuffer2 + 1)+1;
                memset(dataBuffer2, messageLen, 1);//put the message size on the first byte 9+message_len
                
                while (!is_socket_ready(connectSocket, false)) {

                }

                iResult = send(connectSocket, dataBuffer2, (int)strlen(dataBuffer2), 0);
                if (iResult != SOCKET_ERROR)	// Check if message is successfully received
                {
                    //printf("[WORKER]: returned to load balancer %s\n", dataBuffer2);

                    if(strstr(dataBuffer2, ":exit") != NULL)
                        break;
                }
                else	// There was an error during recv
                {

                    printf("[WORKER]: send failed with error: %d\n", WSAGetLastError());
                    closesocket(connectSocket);
                    WSACleanup();
                    return 1;
                    
                }
            //}

            

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
    //_getch();


    // Close connected socket
    closesocket(connectSocket);

    // Deinitialize WSA library
    WSACleanup();

    return 0;
}
