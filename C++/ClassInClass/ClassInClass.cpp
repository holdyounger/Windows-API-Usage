// ClassInClass.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "A.h"

int main()
{
    std::cout << "Hello World!\n";
    A obja;

    A::B b;
    b.optA(obja);


}