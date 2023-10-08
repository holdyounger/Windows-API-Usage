#include "TAPipeIPC.h"
#include <string>
#include <Windows.h>

using namespace std;
#define PIPE_SERVER_NAME       "\\\\.\\pipe\\TrustAgent\\v10_plugin_s_pipe"
#define PIPE_CLIENT_NAME       "\\\\.\\pipe\\TrustAgent\\v10_plugin_c_pipe"

PipeIPC::~PipeIPC()
{
	if (m_hPipeHandle != INVALID_HANDLE_VALUE)
	{
		DisconnectNamedPipe(m_hPipeHandle);
		CloseHandle(m_hPipeHandle);
	}
}

bool PipeIPC::InitPipeIPC()
{
	if (USER_SERVER == m_Role)
	{
		cout << "ser" << endl;
		m_hPipeHandle = CreateNamedPipeA(PIPE_CLIENT_NAME, PIPE_ACCESS_DUPLEX, //双向模式
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE/* | PIPE_NOWAIT*/,
			PIPE_UNLIMITED_INSTANCES,//设置最大容纳实例数目
			0,
			0,
			NULL, nullptr);
		//如果管道创建失败
		if (m_hPipeHandle == INVALID_HANDLE_VALUE)
		{
			return false;
		}
	}
	else
	{
		cout << "Cli" << endl;
		if (WaitNamedPipeA(PIPE_CLIENT_NAME, NMPWAIT_WAIT_FOREVER) == FALSE)
			return 0;

		m_hPipeHandle = CreateFileA(
			PIPE_CLIENT_NAME,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		//如果管道创建失败
		if (m_hPipeHandle == INVALID_HANDLE_VALUE)
		{
			return false;
		}
	}

	return true;
}

//读取数据
bool PipeIPC::ReadData(__in std::string& datas, bool& isPipeEnd)
{
	DWORD cbRead = 0;//定义数据长度
	DWORD cbData = 0;
	if (ReadFile(m_hPipeHandle, &cbData, sizeof(cbData), &cbRead, nullptr) && sizeof(cbData) == cbRead)
	{
		datas.resize(cbData);

		if (ReadFile(m_hPipeHandle, (LPVOID)datas.data(), cbData, &cbRead, nullptr) && cbData == cbRead)
		{
			return true;
		}
	}

	datas.resize(0);
	return false;
}

//写入数据
bool PipeIPC::WriteData(const std::string& datas)
{
	if (m_Role == USER_SERVER)
	{
		bool connect = ConnectNamedPipe(m_hPipeHandle, NULL);
		if (!connect)
		{
			DWORD error = GetLastError();
			if (error != ERROR_PIPE_CONNECTED)
			{
				//cout << "客户端还没连接" << endl;
				//连接失败
				return false;
			}
		}
	}
	//连接成功！
	DWORD wLen;


	const DWORD cbData = datas.size() * sizeof(datas[0]);

	if (!WriteFile(m_hPipeHandle, &cbData, sizeof(cbData), &wLen, NULL))
		return false;
	if (!WriteFile(m_hPipeHandle, datas.c_str(), datas.size() * sizeof(datas[0]), &wLen, NULL))
		return false;

	return true;
}