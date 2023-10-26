// time_t.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <time.h>

using namespace std;

std::string getGlobalTime()
{
	time_t rawtime;

	struct tm info;
	char buffer[80];

	time(&rawtime);

	localtime_s(&info, &rawtime);  //将time_t转为tm
	strftime(buffer, 80, "%Y-%m-%d %H:%M:%S ", &info);
	cout << buffer << endl;

	
	rawtime += 60 * 2;
	localtime_s(&info, &rawtime);  //将time_t转为tm
	strftime(buffer, 80, "%Y-%m-%d %H:%M:%S ", &info);
	cout << buffer << endl;

	return std::string(buffer);

}
int main()
{
    std::cout << getGlobalTime();
}
