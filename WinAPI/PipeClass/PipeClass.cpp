// PipeClass.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <memory>
#include "PipeIPC.h"


int main()
{
	//客户端代码
	std::unique_ptr<PipeIPC> m_p(new PipeIPC());
	std::string Names = "NAMEs11s";
	std::string datas = "ddsss";
	m_p->InitPipeIPC(Names, USER_CLIENT);
	while (1) {
		bool isPipeEnd = false;
		if (!m_p->ReadData(datas, isPipeEnd) && isPipeEnd)
		{
			break;
		}
	}
	system("pause");
	return 0;
#if 0
	//服务端代码
	unique_ptr<PipeIPC> m_p(new PipeIPC());
	std::string Names = "NAMEs11s";
	std::string datas;
	m_p->InitPipeIPC(Names, USER_SERVER);
	while (1) {
		std::cout << "请输入数据：";
		std::cin >> datas;
		m_p->WriteData(datas);
	}
	m_p->ReadData(datas);
	system("pause");
#endif
}