#include <Windows.h>
#include <stdio.h>


DWORD WINAPI Thread_1(LPVOID param);
DWORD WINAPI Thread_2(LPVOID param);
DWORD WINAPI Thread_3(LPVOID param);

HANDLE hSM_1;
HANDLE hSM_2;
HANDLE hSM_3;

HANDLE hThread_1;
HANDLE hThread_2;
HANDLE hThread_3;

int main(int argc, const char* argv[])
{

	//创建三个信号量

	hSM_1 = CreateSemaphore(NULL, 1, 1, L"A");//开始为signal状态
	hSM_2 = CreateSemaphore(NULL, 0, 1, L"B");//开始为unsignal状态，等待hSM_1释放
	hSM_3 = CreateSemaphore(NULL, 0, 1, L"C");//开始为unsignal状态，等待hSM_2

	//创建三个线程

	hThread_1 = CreateThread(NULL, 0, Thread_1, NULL, 0, NULL);
	hThread_2 = CreateThread(NULL, 0, Thread_2, NULL, 0, NULL);
	hThread_3 = CreateThread(NULL, 0, Thread_3, NULL, 0, NULL);
	//等待三个线程都执行完
	WaitForSingleObject(hThread_1, INFINITE);
	WaitForSingleObject(hThread_2, INFINITE);
	WaitForSingleObject(hThread_3, INFINITE);

	//三个线程都执行完
	printf("\n\n\t main end \n");
	//关闭句柄
	CloseHandle(hThread_1);
	CloseHandle(hThread_2);
	CloseHandle(hThread_3);
	CloseHandle(hSM_1);
	CloseHandle(hSM_2);
	CloseHandle(hSM_3);
	return 0;
}

DWORD WINAPI Thread_1(LPVOID param)
{
	for (int i = 0; i < 10; i++)
	{
		DWORD dwWait = WaitForSingleObject(hSM_1, INFINITE);

		//每一个wait过后信号量的数量自动减1，这样就达到了控制同步

		printf("A");
		ReleaseSemaphore(hSM_2, 1, NULL);
	}
	return 0;
}

DWORD WINAPI Thread_2(LPVOID param)
{
	for (int i = 0; i < 10; i++)
	{
		WaitForSingleObject(hSM_2, INFINITE);
		printf("B");
		ReleaseSemaphore(hSM_3, 1, NULL);
	}
	return 0;
}

DWORD WINAPI Thread_3(LPVOID param)
{
	for (int i = 0; i < 10; i++)
	{
		WaitForSingleObject(hSM_3, INFINITE);
		printf("C ");
		ReleaseSemaphore(hSM_1, 1, NULL);
	}
	return 0;
}