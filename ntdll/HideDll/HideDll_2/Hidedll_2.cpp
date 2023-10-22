#include<stdio.h>
#include<Windows.h>

typedef struct _UNICODE_STRING
{
    USHORT Length;        //�ַ�������
    USHORT MaximumLength; //�ַ�����󳤶�
    PWSTR Buffer;         //˫�ֽ��ַ���ָ��
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _PEB_LDR_DATA
{
    ULONG Length;
    BOOLEAN Initialized;
    PVOID SsHandle;
    LIST_ENTRY InLoadOrderModuleList;           //��������˳�򹹳ɵ�ģ���б�
    LIST_ENTRY InMemoryOrderModuleList;			//�����ڴ�˳�򹹳ɵ�ģ���б�
    LIST_ENTRY InInitializationOrderModuleList; //������ʼ��˳�򹹳ɵ�ģ������
}PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY InLoadOrderModuleList;  //��������˳�򹹳ɵ�ģ���б�
    LIST_ENTRY InMemoryOrderModuleList;    //�����ڴ�˳�򹹳ɵ�ģ���б�
    LIST_ENTRY InInitializeationOrderModuleList; //������ʼ��˳�򹹳ɵ�ģ������
    PVOID DllBase;     //��ģ��Ļ���ַ
    PVOID EntryPoint;  //��ģ������
    ULONG SizeOfImage; //��ģ���Ӱ���С
    UNICODE_STRING FullDllName;    //ģ�������·��
    UNICODE_STRING BaseDllName;    //ģ����
    ULONG Flags;
    SHORT LoadCount;
    SHORT TlsIndex;
    HANDLE SectionHandle;
    ULONG CheckSum;
    ULONG TimeDataStamp;
}LDR_MODULE, * PLDR_MODULE;

PEB_LDR_DATA* g_pPebLdr = NULL;
LDR_MODULE* g_pLdrModule = NULL;
LIST_ENTRY* g_pInLoadOrderModule;
LIST_ENTRY* g_pInMemoryOrderModule;
LIST_ENTRY* g_pInInitializeationOrderModule;

void ring3BrokenChains(HMODULE hModule)
{
    LIST_ENTRY* pHead = g_pInLoadOrderModule;
    LIST_ENTRY* pCur = pHead;

    do {
        pCur = pCur->Blink;
        g_pLdrModule = (PLDR_MODULE)pCur;  // ����Ϊʲô����ֱ�ӽ�pCurתΪPLDR_MODULE��������������

        // ������Ϊ pCur ָ�� _LIST_ENTRY �ṹ�壬ָ��ĵ�ַ�պ���_LDR_DATA_TABLE_ENTRY���׵�ַ����˶������ڴ��ϸպ��Ƕ���ġ�
        // CONTAINING_RECORD����귵�س�Ա�������ڽṹ��Ļ�ַ��ldte == g_pLdrModule
        // PLDR_MODULE ldte = CONTAINING_RECORD(pCur, _LDR_DATA_TABLE_ENTRY, InLoadOrderModuleList);


        if (hModule == g_pLdrModule->DllBase)
        {
            g_pLdrModule->InLoadOrderModuleList.Blink->Flink = g_pLdrModule->InLoadOrderModuleList.Flink;
            g_pLdrModule->InLoadOrderModuleList.Flink->Blink = g_pLdrModule->InLoadOrderModuleList.Blink;

            g_pLdrModule->InInitializeationOrderModuleList.Blink->Flink = g_pLdrModule->InInitializeationOrderModuleList.Flink;
            g_pLdrModule->InInitializeationOrderModuleList.Flink->Blink = g_pLdrModule->InInitializeationOrderModuleList.Blink;

            g_pLdrModule->InMemoryOrderModuleList.Blink->Flink = g_pLdrModule->InMemoryOrderModuleList.Flink;
            g_pLdrModule->InMemoryOrderModuleList.Flink->Blink = g_pLdrModule->InMemoryOrderModuleList.Blink;
            break;
        }
    } while (pHead != pCur);
}

int main(int argc, char* argv[])
{
    __asm
    {
        mov eax, fs: [0x30] ;  // PPEB
        mov ecx, [eax + 0xC];  // ldr
        mov g_pPebLdr, ecx;

        mov ebx, ecx;
        add ebx, 0xC;
        mov g_pInLoadOrderModule, ebx;				// ��1������

        mov ebx, ecx;
        add ebx, 0x14;
        mov g_pInMemoryOrderModule, ebx;			// ��2������

        mov ebx, ecx;
        add ebx, 0x1C;
        mov g_pInInitializeationOrderModule, ebx;	// ��3������
    }

    printf("�����ⰴ����ʼ����");
    getchar();
    ring3BrokenChains(GetModuleHandleA("kernel32.dll"));
    printf("�����ɹ�\n");
    getchar();
    return 0;
}