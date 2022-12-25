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

#define SERVER_PORT 5059
#define BUFFER_SIZE 256


//#include "queue.h"
//#include "hash_map.h"
//#pragma warning(disable:4996)

/*
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
		dequeue(q,buffer);
	}
}
*/

static int client_count=0;




DWORD WINAPI read_write_thread(LPVOID param) {
    SOCKET acceptedSocket = (SOCKET)param;
    char dataBuffer[BUFFER_SIZE];
    int client_num = client_count++;
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



// TCP server that use blocking sockets
int main_tcp()
{
    // Socket used for listening for new clients 
    SOCKET listenSocket = INVALID_SOCKET;

    // Socket used for communication with client
    SOCKET acceptedSocket = INVALID_SOCKET;

    // Variable used to store function return value
    int iResult;

    // Buffer used for storing incoming data
    char dataBuffer[BUFFER_SIZE];

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

        HANDLE hProducer;
        HANDLE hConsumer;
        DWORD ProducerID;
        DWORD ConsumerID;

        hProducer = CreateThread(NULL, 0, &read_write_thread, (LPVOID)acceptedSocket, 0, &ProducerID);

        // Here is where server shutdown loguc could be placed

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



int main() {

	main_tcp();





	/*char str[6];
	strcpy(str, "CL10");
	printf("ABC10 => %d\n", hash(str));
	strcpy(str, "ABC11");
	printf("ABC11 => %d\n", hash(str));
	strcpy(str, "GF12");
	printf("ABC12 => %d\n", hash(str));
	strcpy(str, "GSDA");
	printf("ABC13 => %d\n", hash(str));
	strcpy(str, "GDASG");
	printf("ABC14 => %d\n", hash(str));*/
	
	
	
	/*
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
	*/



	/*
	char data_a[10] = "ABC";
	char data_b[10] = "BCD";
	char data_c[10] = "EFG";
	char data_d[10] = "KLM";

	

	enqueue(q,data_a);
	enqueue(q,data_b);
	enqueue(q,data_c);
	enqueue(q,data_d);

	print_queue(q);


	printf("%s -> dequeued from queue\n\n", dequeue(q));
	printf("%s -> dequeued from queue\n\n", dequeue(q));
	printf("%s -> dequeued from queue\n\n", dequeue(q));

	print_queue(q);

	printf("%s -> dequeued from queue\n\n", dequeue(q));

	print_queue(q);

	enqueue(q, data_b);
	enqueue(q, data_c);
	enqueue(q, data_d);
	enqueue(q, data_b);
	enqueue(q, data_c);
	enqueue(q, data_d);
	enqueue(q, data_d);

	print_queue(q);


	printf("%s -> dequeued from queue\n\n", dequeue(q));
	printf("%s -> dequeued from queue\n\n", dequeue(q));


	print_queue(q);
	*/
	return 0;
}

//#include <stdio.h> 
//#include <conio.h> 
//#include <windows.h> 
//
//#pragma warning(disable:4996)
//HANDLE hSemaphore;
//int counter;
///* Telo niti. */
//DWORD WINAPI ThreadProc(LPVOID lpParam)
//{
//	while (true)
//	{
//		/* Cekaj na signal da je pritisnut taster. */
//		WaitForSingleObject(hSemaphore, INFINITE);
//		/* Povecaj brojac i ispisi vrednost. */
//		counter++;
//		printf("\r counter = %d", counter);
//	}
//}
//void main(void)
//{
//	DWORD threadID;
//	HANDLE hThread;
//
//	/* Inicijalizacija sistema. */
//	hSemaphore = CreateSemaphore(0, 0, 1, NULL);
//	hThread = CreateThread(NULL, 0, &ThreadProc, NULL, 0,
//		&threadID);
//	/* Glavna petlja programa, zavrsava se pritiskom na q. */
//	while (true)
//	{
//		/* Da li je pritisnut taster? */
//		if (kbhit()) {
//			/* Ukoliko jeste, preuzmi ga. Ako je preuzeti znak q,
//			zavrsi program. */
//			if (getch() == 'q') break;
//			/* Obavesti nit da je pritisnu taster. */
//			ReleaseSemaphore(hSemaphore, 1, NULL);
//		}
//		/* Pauza, da procesor ne bi konstantno bio zauzet
//		ispitivanjem da li je pritisnut taster. */
//		Sleep(100);
//	}
//
//	/* Oslobadjanje zauzetig resursa. */
//	CloseHandle(hThread);
//	CloseHandle(hSemaphore);
//}