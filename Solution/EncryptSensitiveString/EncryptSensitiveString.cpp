// EncryptSensitiveString.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>

#include "obfuscation.h"
#include "hideImport.h"

void ThreadProc()
{
    MessageBox(NULL, L"我已成功打入敌人内部 By Startu", L"报告首长", 0);
    return;
}

int main()
{
    PVOID lpAllocBase;
    DEFINE_FUNC_PTR("kernel32.dll", VirtualAlloc);
    lpAllocBase = VirtualAlloc(NULL, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    {
        constexpr DWORD hash_VirtualAlloc = ct_hash("kernel32.dll") + ct_hash("VirtualAlloc");
        typedef decltype(VirtualAlloc) type_VirtualAlloc; 
        type_VirtualAlloc* VirtualAlloc = (type_VirtualAlloc*)getProcAddrByHash(hash_VirtualAlloc);
    }

    // DEFINE_FUNC_PTR("user32.dll", MessageBox);

    ThreadProc();

    std::cout << ("Hello World!\n");
    std::cout << XorStringA("Hello World test!\n");
    std::cout << XorStringA("Hello World XorString!\n");

    [] { constexpr ObfuscateCompileTime::ObfuscateString<sizeof("Hello World!\n") / sizeof(char) - 1, 100, char> expr("Hello World!\n", std::make_index_sequence<sizeof("Hello World!\n") / sizeof(char) - 1>()); return expr; }().decrypt();

    [] { constexpr ObfuscateCompileTime::ObfuscateString<sizeof("Hello World test!\n") / sizeof(char) - 1, 108, char> expr("Hello World test!\n", std::make_index_sequence<sizeof("Hello World test!\n") / sizeof(char) - 1>()); return expr; }().decrypt();

    [] { 
        constexpr ObfuscateCompileTime::ObfuscateString<sizeof("Hello World!\n") / sizeof(char) - 1, 8, char> expr("Hello World!\n", std::make_index_sequence<sizeof("Hello World!\n") / sizeof(char) - 1>()); 
    return expr; 
    }().decrypt();
}
