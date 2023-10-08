#include "PipeIPC.h"
using namespace std;

//实现
PipeIPC::PipeIPC()
{
	m_pipe_name = ""; //设置空
	m_hPipeHandle = nullptr;
	m_state = USER_CLIENT; //默认为客户端
}

PipeIPC::~PipeIPC()
{
	DisconnectNamedPipe(m_hPipeHandle);
	CloseHandle(m_hPipeHandle);//关闭句柄
}

bool PipeIPC::InitPipeIPC(std::string pipe_name, PIPEUSERTYPE states)
{
	m_state = states;
	std::string spice_name = "\\\\.\\pipe\\";//需要拼接的字符串
	spice_name.append(pipe_name); //添加
	m_pipe_name.append(spice_name);
	if (m_state == USER_SERVER)
	{
		m_hPipeHandle = CreateNamedPipeA(m_pipe_name.c_str(), PIPE_ACCESS_DUPLEX, //双向模式
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT,
			PIPE_UNLIMITED_INSTANCES,//设置最大容纳实例数目
			0,
			0,
			NULL, nullptr);
		//如果管道创建失败
		if (m_hPipeHandle == INVALID_HANDLE_VALUE)
		{
			cout << "" << endl;
			return false;
		}
		//下载成功
		cout << "初始化服务端PIPE成功！" << endl;
	}
	else
	{
		if (WaitNamedPipeA(m_pipe_name.c_str(), NMPWAIT_WAIT_FOREVER) == FALSE)
			return 0;


		if (FALSE == ConnectNamedPipe(m_hPipeHandle, NULL))
		{

		}

		m_hPipeHandle = CreateFileA(

			m_pipe_name.c_str(), //创建或打开的对象(管道)名称

			GENERIC_READ | //对象的访问方式：读访问

			GENERIC_WRITE, //对象的访问方式：写访问

			0, //对象是否共享：0表示不共享

			NULL, //指向一个SECURITY_ATTRIBUTES结构的指针

			OPEN_EXISTING, //对象的创建方式：OPEN_EXISTING表示打开对象(管道)

			FILE_ATTRIBUTE_NORMAL, //设置对象的属性和标志

			NULL);
		//如果管道创建失败
		if (m_hPipeHandle == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		//下载成功
		cout << "初始化客户端PIPE成功！" << endl;
	}

	return true;
}

//读取数据
bool PipeIPC::ReadData(std::string& datas, bool& isPipeEnd)
{
	DWORD rLen = 0;//定义数据长度
	//读取时护具
	char szBuffer[4096] = { 0 };
	if (!ReadFile(m_hPipeHandle, szBuffer, 4096, &rLen, nullptr))
	{

		DWORD error = GetLastError();
		isPipeEnd = (error == ERROR_BROKEN_PIPE) ? true : false;
		//cout << "读取失败: error:" << error << endl;
		return false;
	}
	cout << "读取数据为:" << szBuffer << endl;
	datas = szBuffer;
	return true;
}

//写入数据
bool PipeIPC::WriteData(std::string datas)
{
	if (m_state == USER_SERVER)
	{
		bool connect = ConnectNamedPipe(m_hPipeHandle, NULL);
		DWORD error = GetLastError();
		if (!connect)
		{
			if (error != ERROR_PIPE_CONNECTED)
			{
				cout << "客户端还没连接" << endl;
				//连接失败
				return false;
			}
		}
	}
	//连接成功！
	DWORD wLen;
	//读取数据
	WriteFile(m_hPipeHandle, datas.c_str(), strlen(datas.c_str()), &wLen, NULL);
	return true;
}