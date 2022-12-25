#include <windows.h>
#define MAX_ELEM 20
#define CLIENT_NAME_LEN 11

typedef struct {
	HANDLE clientThread; // handle to the thread
	char clientName[CLIENT_NAME_LEN]; //We will have maximum of 1000 clients
}client_threads;

client_threads* hash_table_clt[MAX_ELEM];
unsigned int hash(char* name);
bool init_hash_table();