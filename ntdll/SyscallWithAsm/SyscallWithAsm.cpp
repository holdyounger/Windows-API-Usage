// SyscallWithAsm.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <windows.h>  
#include <tlhelp32.h>  
#include <tchar.h>  
#include <iostream>  
#include <vector>  

using namespace std;

// Function to check if a string starts with another string
bool startsWith(const std::wstring& fullString, const std::wstring& startString) {
    return fullString.rfind(startString, 0) == 0;
}

vector<DWORD> EnumProcesses()
{
    vector<DWORD> dwPIDs{};
    // Define the target process name pattern
    std::wstring targetNamePattern = L"CGE";

    // Initialize required structures
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Check for valid snapshot handle
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create snapshot of processes." << std::endl;
        return dwPIDs;
    }

    // Iterate through processes
    if (Process32First(hSnapshot, &pe32)) {
        do {
            std::wstring processName(pe32.szExeFile);

            // Check if the process name starts with the target pattern
            if (startsWith(processName, targetNamePattern)) {
                std::wcout << "Process ID: " << pe32.th32ProcessID << " | Name: " << processName << std::endl;
                dwPIDs.emplace_back(pe32.th32ProcessID);
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    // Clean up
    CloseHandle(hSnapshot);

    return dwPIDs;
}

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

EXTERN_C NTSTATUS NtTerminateProcess(
    HANDLE ProcessHandle,
    NTSTATUS ExitStatus
);


typedef NTSTATUS(NTAPI* pNtTerminateProcess)(
    HANDLE ProcessHandle,
    NTSTATUS ExitStatus
    );

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
    // NtCreateThreadEx(&hthread, GENERIC_EXECUTE, nullptr, hproc, ThreadProc, nullptr, FALSE, 0, 0, 0, nullptr);

    DWORD dwTargetID = 0;
    cin >> dwTargetID;


    vector<DWORD> dwPids = EnumProcesses();
    while (dwPids.size())
    {
        for (auto pid : dwPids)
        {
            HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, true, pid);
            if (handle)
            {
                NTSTATUS status = NtTerminateProcess(handle, 1);
                printf("Kill[%d], NTSTATUS:%d\n", pid, status);
            }
            else
            {
                printf("GetLastError(0x%x)\n", GetLastError());
            }

            if (handle)
            {
                CloseHandle(handle);
            }
        }

        Sleep(2000);

        dwPids = EnumProcesses();
    }
    system("pause");
    return 0;
}