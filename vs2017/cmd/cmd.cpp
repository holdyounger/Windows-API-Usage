#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <sstream>

#define CMD_RESULT_BUF_SIZE 1024
#define CMD_QUERY_DYNAMIC_TCP "netsh int ipv4 show dynamicport tcp"



#pragma warning(disable:4996)
using namespace std;
/*
 * cmd：待执行命令
 * result：命令输出结果
 * 函数返回：0 成功；-1 失败；
 */
int ExecuteCMD(const char *cmd, char *result)
{
	int iRet = -1;
	char buf_ps[CMD_RESULT_BUF_SIZE];
	char ps[CMD_RESULT_BUF_SIZE] = { 0 };
	FILE *ptr;

	strcpy_s(ps, _countof(CMD_QUERY_DYNAMIC_TCP), CMD_QUERY_DYNAMIC_TCP);

	if ((ptr = _popen(ps, "r")) != NULL)
	{
		while (fgets(buf_ps, sizeof(buf_ps), ptr) != NULL)
		{
			strcat_s(result, _countof(buf_ps), buf_ps);
			if (strlen(result) > CMD_RESULT_BUF_SIZE)
			{
				break;
			}
		}
		_pclose(ptr);
		ptr = NULL;
		iRet = 0;  // 处理成功
	}
	else
	{
		printf("popen %s error\n", ps);
		iRet = -1; // 处理失败
	}

	return iRet;
}

vector<unsigned int> getDynamicPortFromstr(string str)
{
	vector<unsigned int> s;
	int len = str.length();

	for (int i = 0; i < len; i++) {
		int CurNum = 0;
		bool flag = false;
		while (!(str[i] >= '0' && str[i] <= '9') && i < len) {
			i++;
		}

		while ((str[i] >= '0' && str[i] <= '9') && i < len) {
			flag = true;
			CurNum = CurNum * 10 + (str[i] - '0');
			i++;
		}
		if (flag)
			s.push_back(CurNum);
	}

	

	return s; //集合大小就是不同整数数量
}

int main()
{
	char result[CMD_RESULT_BUF_SIZE] = { 0 };

	ExecuteCMD("netsh int ipv4 show dynamicport tcp", result);

	printf("This is an example\n\n");
	printf("%s", result);

	

	string str(result);

	auto port = getDynamicPortFromstr(str);
	for (auto it : port)
	{
		cout << "port:" << it << endl;
	}

	printf("\n\nThis is end\n");

	return 0;
}