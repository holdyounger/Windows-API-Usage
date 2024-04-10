// 7zHelperTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "7zHelper.h"
#include "ZipHelper.h"

int main()
{
    std::cout << "Hello World!\n";

    vector<CString> vecFiles{
        "D:\\Documents\\A_Source\\windows-API-Usage\\Solution\\7zHelperTest\\7zHelper.cpp"
    };

    // Q7zComprocess("D:\\Documents\\A_Source\\windows-API-Usage\\Solution\\7zHelperTest\\7zHelper.zip", "123123", vecFiles);
    QZipComprocess("D:\\Documents\\A_Source\\windows-API-Usage\\Solution\\7zHelperTest\\ZipHelper.zip", "123123", vecFiles);

    system("pause");

    return 0;
}
