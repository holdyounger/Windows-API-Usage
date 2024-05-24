// SyscallWithAsm.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <Windows.h>
#include <iostream>

EXTERN_C NTSTATUS NtCreateThreadEx(
    OUT PHANDLE ThreadHandle,
    IN ACCESS_MASK DesiredAccess,
    IN LPVOID ObjectAttributes OPTIONAL,
    IN HANDLE ProcessHandle,
    IN PVOID StartRoutine,
    IN PVOID Argument OPTIONAL,
    IN ULONG CreateFlags,
    IN SIZE_T ZeroBits,
    IN SIZE_T StackSize,
    IN SIZE_T MaximumStackSize,
    IN LPVOID AttributeList OPTIONAL);

DWORD WINAPI ThreadProc(LPVOID prarm)
{
    std::cout << "thead id:" << GetCurrentThreadId() << std::endl;

    return 0;
}

int main()
{
    HANDLE hproc = GetCurrentProcess();
    HANDLE hthread = nullptr;
    // hthread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    NtCreateThreadEx(&hthread, GENERIC_EXECUTE, nullptr, hproc, ThreadProc, nullptr, FALSE, 0, 0, 0, nullptr);

    WaitForSingleObject(hthread, INFINITE);
    CloseHandle(hthread);

    system("pause");
    return 0;
}