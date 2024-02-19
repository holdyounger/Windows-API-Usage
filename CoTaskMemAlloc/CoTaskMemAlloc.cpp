// CoTaskMemAlloc.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <combaseapi.h>


void test()
{
	const wchar_t c_s[] = L"hello.stg";

	wchar_t* s = (wchar_t*)CoTaskMemAlloc(sizeof(c_s));
	wcscpy_s(s, sizeof(c_s), c_s);
}

int main()
{
    test();
    std::cout << "Hello World!\n";
}