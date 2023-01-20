#pragma once

#define CLIENT_NAME_LEN 10
#define BUFFER_WITHOUT_NAME 246
#define BUFFER_SIZE CLIENT_NAME_LEN+BUFFER_WITHOUT_NAME

typedef struct messageStruct {
	char clientName[CLIENT_NAME_LEN];
	char bufferNoName[BUFFER_WITHOUT_NAME];
}messageStruct;

//HANDLE semaphoreEnd;
