#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BUFFER_SIZE 256

DWORD WINAPI clientFunc(LPVOID lpParam) {

	int iResult;
	char dataBuffer[BUFFER_SIZE];

	SOCKET listenSocket = *(SOCKET*)lpParam;

	SOCKET acceptedSocket = accept(listenSocket, NULL, NULL);

	if (acceptedSocket == INVALID_SOCKET)
	{
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	
	do {
		iResult = recv(acceptedSocket, dataBuffer, BUFFER_SIZE, 0);

		if (iResult > 0)	// Check if message is successfully received
		{
			dataBuffer[iResult] = '\0';

			// Log message text
			printf("Client sent: %s.\n", dataBuffer);

		}
		else if (iResult == 0)	// Check if shutdown command is received
		{
			// Connection was closed successfully
			printf("Connection with client closed.\n");
			closesocket(acceptedSocket);
		}
		else	// There was an error during recv
		{

			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(acceptedSocket);
		}

	} while (dataBuffer !=  "terminate");


	return 0;
}
