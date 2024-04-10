// ReadPEWithFileMapping.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include <iostream>
#include <Windows.h>
#include <ImageHlp.h>

#pragma comment(lib,"Imagehlp.lib")

// --------------------------------------------------
// 定义全局变量,来存储 DOS头部/NT头部/Section头部
// --------------------------------------------------
PIMAGE_DOS_HEADER DosHeader = nullptr;
PIMAGE_NT_HEADERS NtHeader = nullptr;
PIMAGE_FILE_HEADER FileHead = nullptr;
PIMAGE_SECTION_HEADER pSection = nullptr;

// --------------------------------------------------
// 读取并设置文件基址以及文件大小
// --------------------------------------------------
CHAR GlobalFilePath[2048] = { 0 }; // 保存文件路径
DWORD GlobalFileSize = 0;          // 定义文件大小
DWORD GlobalFileBase = 0;          // 保存文件的基地址
DWORD IsOpen = 0;                  // 设置文件是否已经打开

// --------------------------------------------------
// 打开文件操作
// --------------------------------------------------
HANDLE OpenPeFile(LPCSTR FileName)
{
    HANDLE hFile, hMapFile, lpMapAddress = NULL;

    hFile = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("[-] 打开文件失败 \n");
        exit(0);
    }
    GlobalFileSize = GetFileSize(hFile, NULL);
    if (GlobalFileSize != 0)
    {
        printf("[+] 已读入文件 \n");
    }

    hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, GlobalFileSize, NULL);
    if (hMapFile == NULL)
    {
        printf("[-] 创建映射对象失败\n");
        exit(0);
    }

    lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, GlobalFileSize);
    if (lpMapAddress != NULL)
    {
        // 设置读入文件基地址
        GlobalFileBase = (DWORD)lpMapAddress;

        // 获取DOS头并判断是不是一个有效的DOS文件
        DosHeader = (PIMAGE_DOS_HEADER)GlobalFileBase;
        if (DosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        {
            printf("[-] 文件不属于DOS结构 \n");
            exit(0);
        }

        // 获取 NT 头并判断是不是一个有效的PE文件
        NtHeader = (PIMAGE_NT_HEADERS)(GlobalFileBase + DosHeader->e_lfanew);
        if (NtHeader->Signature != IMAGE_NT_SIGNATURE)
        {
            printf("[-] 文件不属于PE结构 \n");
            exit(0);
        }

        // 判断是不是32位程序
        if (NtHeader->OptionalHeader.Magic != 0x010B)
        {
            printf("[-] 无法调试非32位PE文件\n");
            exit(0);
        }

        // 获取到文件头指针
        FileHead = &NtHeader->FileHeader;

        // 获取到节表头
        pSection = IMAGE_FIRST_SECTION(NtHeader);
    }

    return lpMapAddress;
}

int main(int argc, char* argv[])
{
    HANDLE BaseAddr = OpenPeFile("c://future.exe");
    printf("[+] 入口地址 = %x \n", BaseAddr);

    system("pause");
    return 0;
}
