// HideDll.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>

DWORD g_isHide = 0;
typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _PEB_LDR_DATA {
    ULONG                   Length;
    BOOLEAN                 Initialized;
    PVOID                   SsHandle;
    LIST_ENTRY              InLoadOrderModuleList;
    LIST_ENTRY              InMemoryOrderModuleList;
    LIST_ENTRY              InInitializationOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _LDR_MODULE
{
    LIST_ENTRY          InLoadOrderModuleList;   //+0x00
    LIST_ENTRY          InMemoryOrderModuleList; //+0x08  
    LIST_ENTRY          InInitializationOrderModuleList; //+0x10
    void* BaseAddress;  //+0x18
    void* EntryPoint;   //+0x1c
    ULONG               SizeOfImage;
    UNICODE_STRING      FullDllName;
    UNICODE_STRING      BaseDllName;
    ULONG               Flags;
    SHORT               LoadCount;
    SHORT               TlsIndex;
    HANDLE              SectionHandle;
    ULONG               CheckSum;
    ULONG               TimeDateStamp;
} LDR_MODULE, * PLDR_MODULE;

void HideDll()	//这个函数是主要的
{
    HMODULE hMod = ::GetModuleHandle("ntdll.dll");
    PLIST_ENTRY Head, Cur;
    PPEB_LDR_DATA ldr;
    PLDR_MODULE ldm;
    __asm
    {
        mov eax, fs: [0x30] // 获取PEB结构
        mov ecx, [eax + 0x0c] //Ldr                     //获取_PEB_LDR_DATA结构
        mov ldr, ecx
    }
    Head = &(ldr->InLoadOrderModuleList);               //获取模块链表地址
    Cur = Head->Flink;                                  //获取指向的结点.
    do
    {
        ldm = CONTAINING_RECORD(Cur, LDR_MODULE, InLoadOrderModuleList); //获取 _LDR_DATA_TABLE_ENTRY结构体地址
        //printf("EntryPoint [0x%X]\n",ldm->BaseAddress);
        if (hMod == ldm->BaseAddress)                                    //判断要隐藏的DLL基址跟结构中的基址是否一样
        {
            g_isHide = 1;                                                //如果进入.则标志置为1,表示已经开始进行隐藏了.
            ldm->InLoadOrderModuleList.Blink->Flink =                    //双向链表. 断开链表
                ldm->InLoadOrderModuleList.Flink;
            ldm->InLoadOrderModuleList.Flink->Blink =
                ldm->InLoadOrderModuleList.Blink;
            ldm->InInitializationOrderModuleList.Blink->Flink =
                ldm->InInitializationOrderModuleList.Flink;
            ldm->InInitializationOrderModuleList.Flink->Blink =
                ldm->InInitializationOrderModuleList.Blink;
            ldm->InMemoryOrderModuleList.Blink->Flink =
                ldm->InMemoryOrderModuleList.Flink;
            ldm->InMemoryOrderModuleList.Flink->Blink =
                ldm->InMemoryOrderModuleList.Blink;
            break;
        }
        Cur = Cur->Flink;
    } while (Head != Cur);
}

int main()
{

    printf("按键开始隐藏\r\n");
    getchar();
    HideDll();

    if (g_isHide == 0)
    {
        printf("没有成功隐藏\r\n");
        system("pause");
        return 0;
    }

    printf("成功隐藏\r\n");
    system("pause");
    return 0;
}