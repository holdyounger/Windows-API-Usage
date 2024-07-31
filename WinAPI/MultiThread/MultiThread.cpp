#include <stdio.h>
#include <Windows.h>
#include <process.h>
#include <vld.h>

#define NUM_THREAD		50

unsigned WINAPI threadInc(void* arg);	// 减一操作
unsigned WINAPI threadDes(void* arg);	// 加一操作

// 操作的全局变量
long long num = 0;

#define NUM_THREAD 50

int main(void) {
	// 创建n个线程句柄
	HANDLE *tHandles = new HANDLE[NUM_THREAD];

	printf("sizeof long long :%d\n", sizeof(long long));

	// 循环n次，创建n个线程
	for (int i = 0; i < NUM_THREAD; i++) {
		if (i % 2) {
			tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, threadInc, NULL, 0, NULL);

		}
		else {
			tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, threadDes, NULL, 0, NULL);
		}
	}

	// 阻塞多个线程句柄，直到子线程运行完毕，主线程才会往下走
	WaitForMultipleObjects(NUM_THREAD, tHandles, TRUE, INFINITE);

	for (int i = 0; i < NUM_THREAD; i++) 
	{
		// CloseHandle(tHandles[i]);
	}

	printf("result:%lld\n", num);

	delete[] tHandles;

	return 0;
}


// 对全局变量加一操作
unsigned WINAPI threadInc(void* arg) {
		num += 1;

	return 0;
}


// 对全局变量减一操作
unsigned WINAPI threadDes(void* arg) {
		num -= 1;

	return 0;
}
