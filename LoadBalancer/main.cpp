#include <stdlib.h>
#include <stdio.h>
#include "hash_map.h"
#include "queue.h"
#include "client_communication.h"
#include "worker_communication.h"
#include "list.h"
#include "testing.h"

#pragma warning(disable:4996)

queue* q;
list* free_workers_list;
list* busy_workers_list;



DWORD WINAPI dispatcher(LPVOID param) {
    return 0;
}

void create_new_worker_process() {
    STARTUPINFO startup_info;
    PROCESS_INFORMATION process_info;
    memset(&startup_info, 0, sizeof(STARTUPINFO));
    startup_info.cb = sizeof(STARTUPINFO);
    memset(&process_info, 0, sizeof(PROCESS_INFORMATION));
    TCHAR buff[100];
    GetCurrentDirectory(100, buff);
    wcscat(buff, L"\\..\\Debug\\Worker.exe");

    if (!CreateProcess(
        buff,          // LPCTSTR lpApplicationName
        NULL,                // LPTSTR lpCommandLine
        NULL,                // LPSECURITY_ATTRIBUTES lpProcessAttributes
        NULL,                // LPSECURITY_ATTRIBUTES lpThreadAttributes
        FALSE,               // BOOL bInheritHandles
        CREATE_NO_WINDOW,    // DWORD dwCreationFlags
        NULL,                // LPVOID lpEnvironment
        NULL,                // LPCTSTR lpCurrentDirectory
        &startup_info,       // LPSTARTUPINFO lpStartupInfo
        &process_info        // LPPROCESS_INFORMATION lpProcessInformation
    )) {

        printf("CreateProcess failed (%d).\n", GetLastError());
    }

}


DWORD WINAPI check_percentage(LPVOID param) {
    while (true) {
        Sleep(3000);
        int fullfillness = ((float)get_current_size(q) / (float)get_capacity(q)) * 100;
        printf("Queue is at %d%%\n", fullfillness);
        if (fullfillness < 30) {
            //shut down worker threads
        }
        else if (fullfillness > 70) {
            // open new worker processes
            create_new_worker_process();
        }
    }
}

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
    q = create_queue(8);
    init_list(&free_workers_list);
    init_list(&busy_workers_list);



    hPercentage = CreateThread(NULL, 0, &check_percentage, (LPVOID)0, 0, &percentageID);
    hListenerClient = CreateThread(NULL, 0, &client_listener, (LPVOID)q, 0, &listenerClientID);
    hListenerWorker = CreateThread(NULL, 0, &worker_listener, (LPVOID)0, 0, &listenerWorkerID);

    //wait for listener to finish
    if (hListenerClient)
        WaitForSingleObject(hListenerClient, INFINITE);
    if (hListenerWorker)
        WaitForSingleObject(hListenerWorker, INFINITE);
    if (hPercentage)
        WaitForSingleObject(hPercentage, INFINITE);

    delete_hashtable();
    delete_list(free_workers_list);
    delete_list(busy_workers_list);
    delete_queue(q);
	return 0;
}

