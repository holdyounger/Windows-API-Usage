#include<stdio.h>
#include<Windows.h>

typedef struct _UNICODE_STRING
{
    USHORT Length;        //字符串长度
    USHORT MaximumLength; //字符串最大长度
    PWSTR Buffer;         //双字节字符串指针
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _PEB_LDR_DATA
{
    ULONG Length;
    BOOLEAN Initialized;
    PVOID SsHandle;
    LIST_ENTRY InLoadOrderModuleList;           //代表按加载顺序构成的模块列表
    LIST_ENTRY InMemoryOrderModuleList;			//代表按内存顺序构成的模块列表
    LIST_ENTRY InInitializationOrderModuleList; //代表按初始化顺序构成的模块链表
}PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY InLoadOrderModuleList;  //代表按加载顺序构成的模块列表
    LIST_ENTRY InMemoryOrderModuleList;    //代表按内存顺序构成的模块列表
    LIST_ENTRY InInitializeationOrderModuleList; //代表按初始化顺序构成的模块链表
    PVOID DllBase;     //该模块的基地址
    PVOID EntryPoint;  //该模块的入口
    ULONG SizeOfImage; //该模块的影像大小
    UNICODE_STRING FullDllName;    //模块的完整路径
    UNICODE_STRING BaseDllName;    //模块名
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
        g_pLdrModule = (PLDR_MODULE)pCur;  // 这里为什么可以直接将pCur转为PLDR_MODULE，见下面代码解释

        // 这是因为 pCur 指向 _LIST_ENTRY 结构体，指向的地址刚好是_LDR_DATA_TABLE_ENTRY的首地址，因此二者在内存上刚好是对齐的。
        // CONTAINING_RECORD这个宏返回成员变量所在结构体的基址，ldte == g_pLdrModule
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
        mov g_pInLoadOrderModule, ebx;				// 第1个链表

        mov ebx, ecx;
        add ebx, 0x14;
        mov g_pInMemoryOrderModule, ebx;			// 第2个链表

        mov ebx, ecx;
        add ebx, 0x1C;
        mov g_pInInitializeationOrderModule, ebx;	// 第3个链表
    }

    printf("点任意按键开始断链");
    getchar();
    ring3BrokenChains(GetModuleHandleA("kernel32.dll"));
    printf("断链成功\n");
    getchar();
    return 0;
}