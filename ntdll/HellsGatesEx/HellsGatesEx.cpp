// HellsGatesEx.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>

DWORD64 djb2(PBYTE str)
{
    DWORD64 dwHash = 0x7734773477347734;

    int c;
    while (c = *str++)
        dwHash = ((dwHash << 0x5) + dwHash) + c;

    return dwHash;
}

void* GetNtdllBase()
{
#if defined(_WIN64)
    ULONG64 peb = __readgsqword(0x60);
#else
    ULONG64 peb = __readfsdword(0x30);
#endif

    ULONG64 ldr = *(ULONG64*)(peb + 0x18);
    PLIST_ENTRY modlist = *(PLIST_ENTRY*)(ldr+0x10); // 第二个加载的 dll 就是 ntdll
    return *(void**)((ULONG64)modlist->Flink + 0x30); // 获取第二个 listEntry 之后，再获取
}

int GetSystemCallIndex(DWORD64 hash)
{
    BYTE* ntdllBase = (BYTE*)GetNtdllBase();
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)ntdllBase;
    PIMAGE_FILE_HEADER pFile = (PIMAGE_FILE_HEADER)(ntdllBase + pDos->e_lfanew + 4); // +4 跳过 50 45
    PIMAGE_OPTIONAL_HEADER pOptional = (PIMAGE_OPTIONAL_HEADER)((BYTE*)pFile+IMAGE_SIZEOF_FILE_HEADER);
    PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)(ntdllBase + pOptional->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);



    DWORD numberOfFunc = pExport->NumberOfFunctions;
    DWORD numberOfname = pExport->NumberOfNames;

    DWORD* pEAT = (DWORD*)(ntdllBase + pExport->AddressOfFunctions);
    DWORD* pENT = (DWORD*)(ntdllBase + pExport->AddressOfNames);
    WORD* pEIT = (WORD*)(ntdllBase + pExport->AddressOfNameOrdinals);

    for (size_t i = 0; i < numberOfFunc; i++)
    {
        for (size_t j = 0; j < numberOfname; j++)
        {
            if (i == pEIT[j])
            {
                BYTE* fnName = (BYTE*)(ntdllBase + pENT[j]);
                if (hash == djb2(fnName))
                {
                    return *(DWORD*)(ntdllBase + pEAT[i] + 4);
                }
            }
        }
    }

    return -1;
}

using namespace std;

extern "C" VOID MySyscallWrapper(DWORD id);
extern "C" VOID MySyscall(...);
int g_nSum = 0;

void MyAdd()
{
    g_nSum++;
};


void MyThread1()
{
    HANDLE* hThread;
    int i = 0;
    while (i++ < 1000)
    {
        MySyscall(&hThread, PROCESS_ALL_ACCESS, 0, GetCurrentProcess(), MyAdd, 0, 0, 0, 0, 0, 0);
    }
}
    
void MyThread2()
{
    while (true)
    {
        // MySyscallWrapper(0);
    }
}

int main()
{

#if 0
    if (GetModuleHandleA("ntdll.dll") == GetNtdllBase())
    {
        std::cout << std::hex << GetModuleHandleA("ntdll.dll") << std::endl;
    }
#endif // 0
    std::cout << "Hello World!\n";

    // 718cca1f5291f6e7 NtOpenProcess
    // 64dc7db288c5015f NtCreateThreadEx
    std::cout << std::hex << djb2((BYTE*)"NtCreateThreadEx") << std::endl;

    DWORD id = 0;
    HANDLE hThread;

    std::cout << std::hex << (id = GetSystemCallIndex(0x64dc7db288c5015f)) << endl;

    // call wrapper 要考虑多线程问题
    MySyscallWrapper(id);
    MySyscall(&hThread, PROCESS_ALL_ACCESS, 0, GetCurrentProcess(), MyThread1, 0 ,0, 0, 0, 0, 0);
    MySyscall(&hThread, PROCESS_ALL_ACCESS, 0, GetCurrentProcess(), MyThread2, 0 ,0, 0, 0, 0, 0);

    Sleep(1000);

    cout << std::dec << g_nSum << endl;

    getchar();

    return 0;
}