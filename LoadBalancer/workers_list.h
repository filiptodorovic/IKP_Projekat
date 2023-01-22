#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include "basic_types.h"

typedef struct worker_node {

	STARTUPINFO startup_info;
	PROCESS_INFORMATION process_info;
	struct worker_node* next;

} worker_node;

extern worker_node* worker_process_head;

void insert_worker_node(worker_node* node);
void delete_worker_list_and_shutdown();
