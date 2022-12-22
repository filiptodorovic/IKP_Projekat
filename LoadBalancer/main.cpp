#include "queue.h"

DWORD WINAPI producer(LPVOID param) {
	queue* q = (queue*)param;
	char data_a[10] = "ABC";
	while (true) {
		enqueue(q, data_a);
		Sleep(300);
	}
}

DWORD WINAPI consumer(LPVOID param) {
	queue* q = (queue*)param;
	while (true) {
		char buffer[10];
		Sleep(10000);
		dequeue(q,buffer);
	}
}



int main() {
	queue* q = create_queue(7);

	HANDLE hProducer;
	HANDLE hConsumer;
	DWORD ProducerID;
	DWORD ConsumerID;
	
	hProducer = CreateThread(NULL, 0, &producer, (LPVOID)q, 0, &ProducerID);
	hConsumer = CreateThread(NULL, 0, &consumer, (LPVOID)q, 0, &ConsumerID);


	if (hConsumer)
		WaitForSingleObject(hConsumer, INFINITE);
	if (hProducer)
		WaitForSingleObject(hProducer, INFINITE);
	/*
	char data_a[10] = "ABC";
	char data_b[10] = "BCD";
	char data_c[10] = "EFG";
	char data_d[10] = "KLM";

	

	enqueue(q,data_a);
	enqueue(q,data_b);
	enqueue(q,data_c);
	enqueue(q,data_d);

	print_queue(q);


	printf("%s -> dequeued from queue\n\n", dequeue(q));
	printf("%s -> dequeued from queue\n\n", dequeue(q));
	printf("%s -> dequeued from queue\n\n", dequeue(q));

	print_queue(q);

	printf("%s -> dequeued from queue\n\n", dequeue(q));

	print_queue(q);

	enqueue(q, data_b);
	enqueue(q, data_c);
	enqueue(q, data_d);
	enqueue(q, data_b);
	enqueue(q, data_c);
	enqueue(q, data_d);
	enqueue(q, data_d);

	print_queue(q);


	printf("%s -> dequeued from queue\n\n", dequeue(q));
	printf("%s -> dequeued from queue\n\n", dequeue(q));


	print_queue(q);
	*/
	return 0;
}

//#include <stdio.h> 
//#include <conio.h> 
//#include <windows.h> 
//
//#pragma warning(disable:4996)
//HANDLE hSemaphore;
//int counter;
///* Telo niti. */
//DWORD WINAPI ThreadProc(LPVOID lpParam)
//{
//	while (true)
//	{
//		/* Cekaj na signal da je pritisnut taster. */
//		WaitForSingleObject(hSemaphore, INFINITE);
//		/* Povecaj brojac i ispisi vrednost. */
//		counter++;
//		printf("\r counter = %d", counter);
//	}
//}
//void main(void)
//{
//	DWORD threadID;
//	HANDLE hThread;
//
//	/* Inicijalizacija sistema. */
//	hSemaphore = CreateSemaphore(0, 0, 1, NULL);
//	hThread = CreateThread(NULL, 0, &ThreadProc, NULL, 0,
//		&threadID);
//	/* Glavna petlja programa, zavrsava se pritiskom na q. */
//	while (true)
//	{
//		/* Da li je pritisnut taster? */
//		if (kbhit()) {
//			/* Ukoliko jeste, preuzmi ga. Ako je preuzeti znak q,
//			zavrsi program. */
//			if (getch() == 'q') break;
//			/* Obavesti nit da je pritisnu taster. */
//			ReleaseSemaphore(hSemaphore, 1, NULL);
//		}
//		/* Pauza, da procesor ne bi konstantno bio zauzet
//		ispitivanjem da li je pritisnut taster. */
//		Sleep(100);
//	}
//
//	/* Oslobadjanje zauzetig resursa. */
//	CloseHandle(hThread);
//	CloseHandle(hSemaphore);
//}