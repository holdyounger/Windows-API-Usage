// ReadFile.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <windows.h>  
#include <iostream>  
#include <stdio.h>  

struct student {
    char name[20];
    int age;
    float score;
};
void ReadWithStruct()
{
    FILE* fp;
    struct student stu = { "Tom", 18, 95.5 };
    struct student stu_read;

    // 打开文件，以二进制方式写入  
    fp = fopen("student.dat", "wb");
    if (fp == NULL) {
        printf("Error opening file\n");
        return ;
    }

    // 写入结构体数据  
    fwrite(&stu, sizeof(struct student), 1, fp);

    // 关闭文件  
    fclose(fp);

    // 打开文件，以二进制方式读取  
    fp = fopen("student.dat", "rb");
    if (fp == NULL) {
        printf("Error opening file\n");
        return ;
    }

    // 读取结构体数据  
    fread(&stu_read, sizeof(struct student), 1, fp);

    // 输出读取的数据  
    printf("Name: %s\n", stu_read.name);
    printf("Age: %d\n", stu_read.age);
    printf("Score: %.1f\n", stu_read.score);

    // 关闭文件  
    fclose(fp);

    return ;
    
}

void ReadFile()
{
    HANDLE hFile = CreateFile(L"D:\\Documents\\A_Source\\CWPP\\EmbeddedLDAP.tran", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cout << "无法打开文件" << std::endl;
        return ;
    }

    DWORD dwFileSize = GetFileSize(hFile, NULL);
    char* buffer = new char[dwFileSize + 1];
    DWORD dwBytesRead = 0;
    ReadFile(hFile, buffer, dwFileSize, &dwBytesRead, NULL);
    buffer[dwBytesRead] = '\0';

    std::cout << "文件内容：" << std::endl << buffer << std::endl;

    CloseHandle(hFile);
    delete[] buffer;
}

int main() {
    ReadWithStruct();
    
    return 0;
}
