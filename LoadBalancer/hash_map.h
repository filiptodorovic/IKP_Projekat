#include <windows.h>
#define MAX_ELEM 5
#define CLIENT_NAME_LEN 10

typedef struct client_thread {
	HANDLE clientThread; // handle to the thread
	char clientName[CLIENT_NAME_LEN]; //We will have maximum of 1000 clients
	bool finished = false;
	struct client_thread* next;
}client_thread;

extern client_thread* hash_table_clt[MAX_ELEM];
unsigned int hash(char* name);
bool init_hash_table();
bool insert_client(client_thread* ct);
client_thread* lookup_client(char* name);
client_thread* delete_client(char* name);
void print_table();