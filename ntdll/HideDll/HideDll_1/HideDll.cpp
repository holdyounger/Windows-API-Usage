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

void HideDll()	//�����������Ҫ��
{
    HMODULE hMod = ::GetModuleHandle("ntdll.dll");
    PLIST_ENTRY Head, Cur;
    PPEB_LDR_DATA ldr;
    PLDR_MODULE ldm;
    __asm
    {
        mov eax, fs: [0x30] // ��ȡPEB�ṹ
        mov ecx, [eax + 0x0c] //Ldr                     //��ȡ_PEB_LDR_DATA�ṹ
        mov ldr, ecx
    }
    Head = &(ldr->InLoadOrderModuleList);               //��ȡģ�������ַ
    Cur = Head->Flink;                                  //��ȡָ��Ľ��.
    do
    {
        ldm = CONTAINING_RECORD(Cur, LDR_MODULE, InLoadOrderModuleList); //��ȡ _LDR_DATA_TABLE_ENTRY�ṹ���ַ
        //printf("EntryPoint [0x%X]\n",ldm->BaseAddress);
        if (hMod == ldm->BaseAddress)                                    //�ж�Ҫ���ص�DLL��ַ���ṹ�еĻ�ַ�Ƿ�һ��
        {
            g_isHide = 1;                                                //�������.���־��Ϊ1,��ʾ�Ѿ���ʼ����������.
            ldm->InLoadOrderModuleList.Blink->Flink =                    //˫������. �Ͽ�����
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

    printf("������ʼ����\r\n");
    getchar();
    HideDll();

    if (g_isHide == 0)
    {
        printf("û�гɹ�����\r\n");
        system("pause");
        return 0;
    }

    printf("�ɹ�����\r\n");
    system("pause");
    return 0;
}