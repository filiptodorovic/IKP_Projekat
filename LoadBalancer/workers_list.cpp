#include "workers_list.h"

void insert_worker_node(worker_node* node) {
    if (worker_process_head == NULL) {
        node->next = NULL;
        worker_process_head = node;
    }
    else {
        node->next = worker_process_head;
        worker_process_head = node;
    }
}

void delete_worker_list_and_shutdown() {
    worker_node* tmp = worker_process_head;
    while (tmp!=NULL) {
        worker_node* prev = tmp;
        WaitForSingleObject(tmp->process_info.hProcess, 100);
        CloseHandle(tmp->process_info.hProcess);
        CloseHandle(tmp->process_info.hThread);
        tmp = tmp->next;
        free(prev);
    }

}