// sha256.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <ostream>
#include <iomanip>
#include <sstream>
#include "sha256.h"

#pragma comment(lib, "libcrypto.lib")

int main()
{
    std::cout << "Hello World!\n";

	const std::string buffer_data = "Hello, World!";  // 示例缓冲区数据  

	unsigned char hash[SHA256_DIGEST_SIZE];
	sha256_buffer(buffer_data.c_str(), buffer_data.length(), hash);

	std::cout << "SHA256: " << hash << std::endl;  // 输出哈希值  
	return 0;


}