// reverseBinary.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdc++.h>
using namespace std;

int toInt(char ch)
{
	if (ch >= '0' && ch <= '9') return ch - '0';
	else if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
	else if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
	return -1;
}
char reverseBinary(int val)
{
	unsigned char m = 0x5;
	val = val >> 1 & m  | (val & m) << 1;
	val = val >> 2 | val << 2;
	val %= 16;
	return val < 10 ? val + '0' : val - 10 + 'A';
}

char myReverse(int val) {
	unsigned char m = 0x5;
	val = val >> 1 & m | (val & m) << 1;
	val = val >> 2 | val << 2;
	val %= 16;
	return val < 10 ? val + '0' : val - 10 + 'A';
}

int to_int(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	else if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	else if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	return -1;
}


int main()
{
    string str = "Hello World!\n";
	string ans;

	for (int i = 0; i < str.size(); i++)
	{
		int nTemp = toInt(str[i]);
		if (nTemp <= 15 && nTemp >= 0)
		{
			ans.push_back(reverseBinary(nTemp));
		}
		else
			ans.push_back(str[i]);
	}

	cout << ans << endl;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
