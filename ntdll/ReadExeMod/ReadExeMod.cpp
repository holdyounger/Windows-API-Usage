// GetPEB.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include <stdio.h>
#include <windows.h>

#define NT_SUCCESS(x) ((x) >= 0)
#define ProcessBasicInformation 0
#define ProcessWow64Information 26

typedef NTSTATUS(NTAPI* pfnNtWow64QueryInformationProcess64)(
    IN HANDLE ProcessHandle,
    IN ULONG ProcessInformationClass,
    OUT PVOID ProcessInformation,
    IN ULONG ProcessInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    );


typedef NTSTATUS(NTAPI* pfnNtWow64ReadVirtualMemory64)(
    IN HANDLE ProcessHandle,
    IN PVOID64 BaseAddress,
    OUT PVOID Buffer,
    IN ULONG64 Size,
    OUT PULONG64 NumberOfBytesRead
    );

typedef
NTSTATUS(WINAPI* pfnNtQueryInformationProcess)
(HANDLE ProcessHandle, ULONG ProcessInformationClass,
    PVOID ProcessInformation, UINT32 ProcessInformationLength,
    UINT32* ReturnLength);

typedef struct _PROCESS_BASIC_INFORMATION32 {
    NTSTATUS ExitStatus;
    UINT32 PebBaseAddress;
    UINT32 AffinityMask;
    UINT32 BasePriority;
    UINT32 UniqueProcessId;
    UINT32 InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION32;

typedef struct _UNICODE_STRING32
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING32, * PUNICODE_STRING32;

typedef struct _PEB32
{
    UCHAR InheritedAddressSpace;
    UCHAR ReadImageFileExecOptions;
    UCHAR BeingDebugged;
    UCHAR BitField;
    ULONG Mutant;
    ULONG ImageBaseAddress;
    ULONG Ldr;
    ULONG ProcessParameters;
    ULONG SubSystemData;
    ULONG ProcessHeap;
    ULONG FastPebLock;
    ULONG AtlThunkSListPtr;
    ULONG IFEOKey;
    ULONG CrossProcessFlags;
    ULONG UserSharedInfoPtr;
    ULONG SystemReserved;
    ULONG AtlThunkSListPtr32;
    ULONG ApiSetMap;
} PEB32, * PPEB32;

typedef struct _PEB_LDR_DATA32
{
    ULONG Length;
    BOOLEAN Initialized;
    ULONG SsHandle;
    LIST_ENTRY32 InLoadOrderModuleList;
    LIST_ENTRY32 InMemoryOrderModuleList;
    LIST_ENTRY32 InInitializationOrderModuleList;
    ULONG EntryInProgress;
} PEB_LDR_DATA32, * PPEB_LDR_DATA32;

typedef struct _LDR_DATA_TABLE_ENTRY32
{
    LIST_ENTRY32 InLoadOrderLinks;
    LIST_ENTRY32 InMemoryOrderModuleList;
    LIST_ENTRY32 InInitializationOrderModuleList;
    ULONG DllBase;
    ULONG EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING32 FullDllName;
    UNICODE_STRING32 BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    union
    {
        LIST_ENTRY32 HashLinks;
        ULONG SectionPointer;
    };
    ULONG CheckSum;
    union
    {
        ULONG TimeDateStamp;
        ULONG LoadedImports;
    };
    ULONG EntryPointActivationContext;
    ULONG PatchInformation;
} LDR_DATA_TABLE_ENTRY32, * PLDR_DATA_TABLE_ENTRY32;

typedef struct _PROCESS_BASIC_INFORMATION64 {
    NTSTATUS ExitStatus;
    UINT32 Reserved0;
    UINT64 PebBaseAddress;
    UINT64 AffinityMask;
    UINT32 BasePriority;
    UINT32 Reserved1;
    UINT64 UniqueProcessId;
    UINT64 InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION64;
typedef struct _PEB64
{
    UCHAR InheritedAddressSpace;
    UCHAR ReadImageFileExecOptions;
    UCHAR BeingDebugged;
    UCHAR BitField;
    ULONG64 Mutant;
    ULONG64 ImageBaseAddress;
    ULONG64 Ldr;
    ULONG64 ProcessParameters;
    ULONG64 SubSystemData;
    ULONG64 ProcessHeap;
    ULONG64 FastPebLock;
    ULONG64 AtlThunkSListPtr;
    ULONG64 IFEOKey;
    ULONG64 CrossProcessFlags;
    ULONG64 UserSharedInfoPtr;
    ULONG SystemReserved;
    ULONG AtlThunkSListPtr32;
    ULONG64 ApiSetMap;
} PEB64, * PPEB64;

typedef struct _PEB_LDR_DATA64
{
    ULONG Length;
    BOOLEAN Initialized;
    ULONG64 SsHandle;
    LIST_ENTRY64 InLoadOrderModuleList;
    LIST_ENTRY64 InMemoryOrderModuleList;
    LIST_ENTRY64 InInitializationOrderModuleList;
    ULONG64 EntryInProgress;
} PEB_LDR_DATA64, * PPEB_LDR_DATA64;

typedef struct _UNICODE_STRING64
{
    USHORT Length;
    USHORT MaximumLength;
    ULONG64 Buffer;
} UNICODE_STRING64, * PUNICODE_STRING64;

typedef struct _LDR_DATA_TABLE_ENTRY64
{
    LIST_ENTRY64 InLoadOrderLinks;
    LIST_ENTRY64 InMemoryOrderModuleList;
    LIST_ENTRY64 InInitializationOrderModuleList;
    ULONG64 DllBase;
    ULONG64 EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING64 FullDllName;
    UNICODE_STRING64 BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    union
    {
        LIST_ENTRY64 HashLinks;
        ULONG64 SectionPointer;
    };
    ULONG CheckSum;
    union
    {
        ULONG TimeDateStamp;
        ULONG64 LoadedImports;
    };
    ULONG64 EntryPointActivationContext;
    ULONG64 PatchInformation;
} LDR_DATA_TABLE_ENTRY64, * PLDR_DATA_TABLE_ENTRY64;

int main()
{
    DWORD dwPid = 4480;
    HANDLE m_ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
    BOOL bTarget = FALSE;
    BOOL bSource = FALSE;

    IsWow64Process(GetCurrentProcess(), &bSource);
    IsWow64Process(m_ProcessHandle, &bTarget);

    SYSTEM_INFO si;
    GetSystemInfo(&si);

    if (bTarget == FALSE && bSource == TRUE)
    {
        HMODULE NtdllModule = GetModuleHandle(L"ntdll.dll");
        pfnNtWow64QueryInformationProcess64 NtWow64QueryInformationProcess64 = (pfnNtWow64QueryInformationProcess64)GetProcAddress(NtdllModule, "NtWow64QueryInformationProcess64");
        pfnNtWow64ReadVirtualMemory64 NtWow64ReadVirtualMemory64 = (pfnNtWow64ReadVirtualMemory64)GetProcAddress(NtdllModule, "NtWow64ReadVirtualMemory64");
        PROCESS_BASIC_INFORMATION64 pbi64 = { 0 };
        if (NT_SUCCESS(NtWow64QueryInformationProcess64(m_ProcessHandle, ProcessBasicInformation, &pbi64, sizeof(pbi64), NULL)))
        {
            DWORD64 Ldr64 = 0;
            LIST_ENTRY64 ListEntry64 = { 0 };
            LDR_DATA_TABLE_ENTRY64 LDTE64 = { 0 };
            wchar_t ProPath64[256];
            if (NT_SUCCESS(NtWow64ReadVirtualMemory64(m_ProcessHandle, (PVOID64)(pbi64.PebBaseAddress + offsetof(PEB64, Ldr)), &Ldr64, sizeof(Ldr64), NULL)))
            {
                if (NT_SUCCESS(NtWow64ReadVirtualMemory64(m_ProcessHandle, (PVOID64)(Ldr64 + offsetof(PEB_LDR_DATA64, InLoadOrderModuleList)), &ListEntry64, sizeof(LIST_ENTRY64), NULL)))
                {
                    if (NT_SUCCESS(NtWow64ReadVirtualMemory64(m_ProcessHandle, (PVOID64)(ListEntry64.Flink), &LDTE64, sizeof(_LDR_DATA_TABLE_ENTRY64), NULL)))
                    {
                        while (1)
                        {
                            if (LDTE64.InLoadOrderLinks.Flink == ListEntry64.Flink) break;
                            if (NT_SUCCESS(NtWow64ReadVirtualMemory64(m_ProcessHandle, (PVOID64)LDTE64.FullDllName.Buffer, ProPath64, sizeof(ProPath64), NULL)))
                            {
                                printf("模块基址:0x%llX\n模块大小:0x%X\n模块路径:%ls\n", LDTE64.DllBase, LDTE64.SizeOfImage, ProPath64);
                            }
                            if (!NT_SUCCESS(NtWow64ReadVirtualMemory64(m_ProcessHandle, (PVOID64)LDTE64.InLoadOrderLinks.Flink, &LDTE64, sizeof(_LDR_DATA_TABLE_ENTRY64), NULL))) break;
                        }
                    }
                }
            }
        }

    }
    else if (bTarget == TRUE && bSource == TRUE || si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_AMD64 ||
        si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_IA64)
    {
        HMODULE NtdllModule = GetModuleHandle(L"ntdll.dll");
        pfnNtQueryInformationProcess NtQueryInformationProcess = (pfnNtQueryInformationProcess)GetProcAddress(NtdllModule, "NtQueryInformationProcess");
        PROCESS_BASIC_INFORMATION32 pbi32 = { 0 };
        if (NT_SUCCESS(NtQueryInformationProcess(m_ProcessHandle, ProcessBasicInformation, &pbi32, sizeof(pbi32), NULL)))
        {
            DWORD Ldr32 = 0;
            LIST_ENTRY32 ListEntry32 = { 0 };
            LDR_DATA_TABLE_ENTRY32 LDTE32 = { 0 };
            wchar_t ProPath32[256];
            if (ReadProcessMemory(m_ProcessHandle, (PVOID)(pbi32.PebBaseAddress + offsetof(PEB32, Ldr)), &Ldr32, sizeof(Ldr32), NULL))
            {
                if (ReadProcessMemory(m_ProcessHandle, (PVOID)(Ldr32 + offsetof(PEB_LDR_DATA32, InLoadOrderModuleList)), &ListEntry32, sizeof(LIST_ENTRY32), NULL))
                {
                    if (ReadProcessMemory(m_ProcessHandle, (PVOID)(ListEntry32.Flink), &LDTE32, sizeof(_LDR_DATA_TABLE_ENTRY32), NULL))
                    {
                        while (1)
                        {
                            if (LDTE32.InLoadOrderLinks.Flink == ListEntry32.Flink) break;
                            if (ReadProcessMemory(m_ProcessHandle, (PVOID)LDTE32.FullDllName.Buffer, ProPath32, sizeof(ProPath32), NULL))
                            {
                                printf("模块基址:0x%X\n模块大小:0x%X\n模块路径:%ls\n", LDTE32.DllBase, LDTE32.SizeOfImage, ProPath32);
                            }
                            if (!ReadProcessMemory(m_ProcessHandle, (PVOID)LDTE32.InLoadOrderLinks.Flink, &LDTE32, sizeof(_LDR_DATA_TABLE_ENTRY32), NULL)) break;
                        }
                    }
                }
            }
        }
    }
    CloseHandle(m_ProcessHandle);
    getchar();
}
