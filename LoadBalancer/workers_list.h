#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include "basic_types.h"

typedef struct worker_node {

	DWORD processId;
	struct worker_node* next;

} worker_node;

struct worker_node* head = NULL;

void insert_last(DWORD id);
void shutdown_all();
void delete_list();
