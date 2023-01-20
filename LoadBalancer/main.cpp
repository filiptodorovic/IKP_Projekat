#include <stdlib.h>
#include <stdio.h>
#include "hash_map.h"
#include "queue.h"
#include "client_communication.h"
#include "worker_communication.h"
#include "list.h"
#include "testing.h"

#define WORKER_IP_ADDRESS "127.0.0.1"
#define WORKER_PORT 6069
//#define DEBUG_LIST
#define DEBUG_QUEUE

#pragma warning(disable:4996)

list* busy_workers_list;
list* free_workers_list;
queue* q;
HANDLE semaphoreEnd;
static unsigned int worker_process_count = 0;

STARTUPINFO startup_info;
PROCESS_INFORMATION process_info;


void create_new_worker_process() {

    memset(&startup_info, 0, sizeof(STARTUPINFO));
    startup_info.cb = sizeof(STARTUPINFO);
    startup_info.dwFlags = STARTF_USESHOWWINDOW;
    startup_info.wShowWindow = SW_SHOW;

    memset(&process_info, 0, sizeof(PROCESS_INFORMATION));
    TCHAR buff[100];
    GetCurrentDirectory(100, buff);
    wcscat(buff, L"\\..\\Debug\\Worker.exe");
    //wcscat(buff, L"\\..\\x64\\Debug\\Worker.exe");
    TCHAR cmd[] = L"Worker.exe";
    if (!CreateProcess(
        buff,          // LPCTSTR lpApplicationName
        cmd,                // LPTSTR lpCommandLine
        NULL,                // LPSECURITY_ATTRIBUTES lpProcessAttributes
        NULL,                // LPSECURITY_ATTRIBUTES lpThreadAttributes
        FALSE,               // BOOL bInheritHandles
        NORMAL_PRIORITY_CLASS,    // DWORD dwCreationFlags
        NULL,                // LPVOID lpEnvironment
        NULL,                // LPCTSTR lpCurrentDirectory
        &startup_info,       // LPSTARTUPINFO lpStartupInfo
        &process_info        // LPPROCESS_INFORMATION lpProcessInformation
    )) {

        printf("CreateProcess failed (%d).\n", GetLastError());
    }

    worker_process_count++;
    ShowWindow(0, SW_SHOW);
}

CRITICAL_SECTION globalCs;

DWORD WINAPI check_percentage(LPVOID param) {
    while (true) {
        if (WaitForSingleObject(semaphoreEnd, 10) == WAIT_OBJECT_0)
            break;
        Sleep(3000);
        int fullfillness = ((float)get_current_size_queue() / (float)get_capacity_queue()) * 100;
        printf("Queue is at %d%%\n", fullfillness);
        if (fullfillness < 30 && worker_process_count>1) {
            //shut down worker threads
            node* first_elem = delete_first_node(free_workers_list);
           

            if (first_elem != NULL && worker_process_count>1) {

                //EnterCriticalSection(&globalCs);

                strcpy(first_elem->msgStruct->bufferNoName, "exit");
                ReleaseSemaphore(first_elem->msgSemaphore, 1, NULL);

                /*
                //wait for worker read and write to finish
                if (first_elem->thread_read) {
                    //WaitForSingleObject(first_elem->thread_read, INFINITE);
                    CloseHandle(first_elem->thread_read);
                }
              
                if (first_elem->thread_write) {
                    //WaitForSingleObject(first_elem->thread_write, INFINITE);
                    CloseHandle(first_elem->thread_write);
                }
                */
                    
                //free(first_elem);
                worker_process_count--;

                //LeaveCriticalSection(&globalCs);
            }
            

           
        }
        else if (fullfillness > 70) {
            // open new worker processes
            //CreateProcess();
            create_new_worker_process();
        }
    }
    return 0;
}

DWORD WINAPI dispatcher(LPVOID param) {

    messageStruct* dequeuedMessageStruct = NULL;

    while (true) {
        if (WaitForSingleObject(semaphoreEnd, 10) == WAIT_OBJECT_0)
            break;
        Sleep(1000);

        if (!is_queue_empty()){

            //EnterCriticalSection(&globalCs);

            node* first = free_workers_list->head;

            //EnterCriticalSection(&free_workers_list->cs);

            if (free_workers_list->head != NULL)
            {

#ifdef DEBUG_LIST
                printf("Free Worker ");
                print_list(free_workers_list);
#endif
                dequeue(&dequeuedMessageStruct);

                EnterCriticalSection(&free_workers_list->cs);
                first->msgStruct=dequeuedMessageStruct;
                ReleaseSemaphore(first->msgSemaphore, 1, NULL);
                LeaveCriticalSection(&free_workers_list->cs);

                move_first_node(busy_workers_list, free_workers_list);
#ifdef DEBUG_LIST
                printf("Busy Worker ");
                print_list(busy_workers_list);
#endif
            }

            //LeaveCriticalSection(&free_workers_list->cs);
            //LeaveCriticalSection(&globalCs);
        }
    }
    return 0;
}



//WaitForSingleObject(process_info.hProcess, INFINITE);
//CloseHandle(process_info.hProcess);
//CloseHandle(process_info.hThread);


int main() {

    HANDLE hListenerClient;
    HANDLE hListenerWorker;
    HANDLE hPercentage;
    HANDLE hDispatcher;

    DWORD listenerClientID;
    DWORD percentageID;
    DWORD listenerWorkerID;
    DWORD dispatcherID;

    init_hash_table();
    create_queue(8);
    init_list(&free_workers_list);
    init_list(&busy_workers_list);
    semaphoreEnd= CreateSemaphore(0, 0, 4, NULL);

    //InitializeCriticalSection(&globalCs);


    hPercentage = CreateThread(NULL, 0, &check_percentage, (LPVOID)0, 0, &percentageID);
    hListenerClient = CreateThread(NULL, 0, &client_listener, (LPVOID)0, 0, &listenerClientID);
    hListenerWorker = CreateThread(NULL, 0, &worker_listener, (LPVOID)0, 0, &listenerWorkerID);
    hDispatcher = CreateThread(NULL, 0, &dispatcher, (LPVOID)0, 0, &dispatcherID);

    create_new_worker_process();

    printf("Press any key to exit:\n");
    char input[2];
    gets_s(input,2);

    ReleaseSemaphore(semaphoreEnd, 4, NULL);

    //wait for listener to finish
    if (hPercentage)
        WaitForSingleObject(hPercentage, INFINITE);
    if (hDispatcher)
        WaitForSingleObject(hDispatcher, INFINITE);
    if (hListenerClient)
        WaitForSingleObject(hListenerClient, INFINITE);
    if (hListenerWorker)
        WaitForSingleObject(hListenerWorker, INFINITE);

    delete_hashtable();
    delete_list(free_workers_list);
    delete_list(busy_workers_list);
    delete_queue();
    
    //DeleteCriticalSection(&globalCs);

	return 0;
}

