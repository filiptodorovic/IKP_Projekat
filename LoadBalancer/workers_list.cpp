#include "workers_list.h"

void insert_last(DWORD id) {
    struct worker_node* new_node = (struct worker_node*)malloc(sizeof(struct worker_node));
    new_node->processId = id;
    new_node->next = NULL;

    if (head == NULL) {
        head = new_node;
        return;
    }

    struct worker_node* temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }

    temp->next = new_node;
}

void shutdown_all() {
    struct worker_node* temp = head;
    while (temp != NULL) {
        
        temp = temp->next;
    }
}

void delete_list() {
    struct worker_node* temp = head;
    while (temp != NULL) {
        struct worker_node* next = temp->next;
        free(temp);
        temp = next;
    }
    head = NULL;
}