#include <AccCtrl.h>
#include <AclAPI.h>

#include "PipeIPC.h"
#ifdef QT_PROJ
#include "QsLog.h"
#endif

#pragma comment(lib, "advapi32.lib")

using namespace std;

PipeIPC::~PipeIPC()
{
	if (m_hPipeHandle != INVALID_HANDLE_VALUE)
	{
		DisconnectNamedPipe(m_hPipeHandle);
		CloseHandle(m_hPipeHandle);
	}
}

//写入数据
bool PipeIPC::WriteData(__in const std::string& datas)
{
#ifdef QS_LOG_VERSION
	QLOG_INFO() << " [PIPE] " << " [Write] ---Start---, datas:" << datas.c_str();
#else
#endif

	int iTry = 0;
	if (m_Role == SERVER)
	{
		bool connect = ConnectNamedPipe(m_hPipeHandle, NULL);
		if (!connect)
		{
#ifdef QS_LOG_VERSION
			QLOG_INFO() << " [PIPE] " << " [INIT] Not Connect ";
#endif // QS_LOG_VERSION
			if (!InitPipeIPC())
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
	}

_RETRY_:
	if (!IsInit())
	{
#ifdef QS_LOG_VERSION
		QLOG_INFO() << " [PIPE] " << " [Write] Not Init ";
#endif // QS_LOG_VERSION
		if (!InitPipeIPC())
		{
			return false;
		}
	}

	//连接成功！
	DWORD wLen;
	const DWORD cbData = datas.size() * sizeof(datas[0]);

	if (!WriteFile(m_hPipeHandle, &cbData, sizeof(cbData), &wLen, NULL))
	{
		if (iTry++ == 0) // 只尝试一次
		{
#ifdef QS_LOG_VERSION
			QLOG_INFO() << " [PIPE]" << " [Write] Try Again ";
#endif // QS_LOG_VERSION
			DisConnect();
			goto _RETRY_;
		}

#ifdef QS_LOG_VERSION
		QLOG_INFO() << " [PIPE]" << __LINE__ << " [Write] Failed " << GetLastError();
#endif // QS_LOG_VERSION
		return false;
	}
	if (!WriteFile(m_hPipeHandle, datas.c_str(), datas.size() * sizeof(datas[0]), &wLen, NULL))
	{
#ifdef QS_LOG_VERSION
		QLOG_INFO() << " [PIPE]" << __LINE__ << " [Write] Failed " << GetLastError();
#endif // QS_LOG_VERSION
		return false;
	}

#ifdef QS_LOG_VERSION
	QLOG_INFO() << " [PIPE] " << " [Write] ---End--- ";
#endif // QS_LOG_VERSION

	return true;
}

//读取数据
bool PipeIPC::ReadData(__out std::string& datas, __out bool& bIsPipeEnd)
{
	if (INVALID_HANDLE_VALUE == m_hPipeHandle || nullptr == m_hPipeHandle)
	{
		InitPipeIPC();
	}

	DWORD cbRead = 0;//定义数据长度
	DWORD cbData = 0;
	if (ReadFile(m_hPipeHandle, &cbData, sizeof(cbData), &cbRead, nullptr) && sizeof(cbData) == cbRead)
	{
		DWORD error = GetLastError();
		bIsPipeEnd = (error == ERROR_BROKEN_PIPE ? true : false);

		datas.resize(cbData);

		if (ReadFile(m_hPipeHandle, (LPVOID)datas.data(), cbData, &cbRead, nullptr) && cbData == cbRead)
		{
			return true;
		}
	}
	else
	{
		DWORD dwError = GetLastError();
		bIsPipeEnd = (dwError == ERROR_BROKEN_PIPE) ? true : false;
	}

	datas.resize(0);
	return false;
}

void PipeIPC::SetPipeName(__in const std::string& name)
{
	m_strName = name;
}

bool PipeIPC::InitPipeIPC()
{
	if (SERVER == m_Role && nullptr != m_hPipeHandle)
	{
		initSecurityAttributes();

		m_hPipeHandle = CreateNamedPipeA(m_strName.c_str(), PIPE_ACCESS_DUPLEX, //双向模式
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE/* | PIPE_NOWAIT*/,
			PIPE_UNLIMITED_INSTANCES,//设置最大容纳实例数目
			0,
			0,
			NULL, (LPSECURITY_ATTRIBUTES)&m_sec_attr);
		//如果管道创建失败
		if (m_hPipeHandle == INVALID_HANDLE_VALUE)
		{
#ifdef QS_LOG_VERSION
			QLOG_INFO() << " [PIPE] " << " [INIT] Create Pipe Failed,GetLastError()= " << GetLastError();
#endif // QS_LOG_VERSION
			return false;
		}

#ifdef QS_LOG_VERSION
		QLOG_INFO() << " [PIPE] " << " [INIT] Create Pipe Success ";
#endif // QS_LOG_VERSION
	}
	else
	{
		if (m_hPipeHandle == INVALID_HANDLE_VALUE)
		{
			m_hPipeHandle = CreateFileA(
				m_strName.c_str(),
				GENERIC_READ | GENERIC_WRITE,
				0,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

			//如果管道创建失败
			if (m_hPipeHandle == INVALID_HANDLE_VALUE)
			{
#ifdef QS_LOG_VERSION
				QLOG_INFO() << " [PIPE] Create TrustAgent Pipe Failed! GetLastError()=" << GetLastError();
#endif // QS_LOG_VERSION
				return false;
			}

			if (!PeekNamedPipe(m_hPipeHandle, NULL, 0, NULL, NULL, NULL))
			{
				if (GetLastError() == ERROR_FILE_NOT_FOUND)
				{
#ifdef QS_LOG_VERSION
					QLOG_INFO() << " [PIPE] Pipe does not exist!";
#endif // QS_LOG_VERSION
				}
				else
				{
#ifdef QS_LOG_VERSION
					QLOG_INFO() << __LINE__ << " [PIPE] Pipe exists but disconnected! Connecting, GetLastError=" << GetLastError();
#endif // QS_LOG_VERSION

					WriteData("start");
					return true;
				}

				return false;
			}
			else {
#ifdef QS_LOG_VERSION
				QLOG_INFO() << " [PIPE]Pipe exists!";
#endif // QS_LOG_VERSION

				WriteData("start");
			}

#ifdef QS_LOG_VERSION
			QLOG_INFO() << " [PIPE] Create TrustAgent Pipe Success!";
#endif // QS_LOG_VERSION

			WriteData("start");

			return true;
		}
		else
		{
			if (!PeekNamedPipe(m_hPipeHandle, NULL, 0, NULL, NULL, NULL))
			{
				if (GetLastError() == ERROR_FILE_NOT_FOUND)
				{
#ifdef QS_LOG_VERSION
					QLOG_INFO() << " [PIPE] Pipe does not exist!";
#endif // QS_LOG_VERSION
				}
				else
				{
#ifdef QS_LOG_VERSION
					QLOG_INFO() << __LINE__ << " [PIPE] Pipe exists but disconnected! Connecting, GetLastError=" << GetLastError();
#endif // QS_LOG_VERSION

					WriteData("start");
					return true;
				}

				return false;
			}
			else
			{
#ifdef QS_LOG_VERSION
				QLOG_INFO() << " [PIPE]Pipe exists!";
#endif // QS_LOG_VERSION

				WriteData("start");
			}
		}

	}

	return true;
}

bool PipeIPC::IsInit()
{
	return m_hPipeHandle == INVALID_HANDLE_VALUE ? false : true;
}

/*
	[details] 创建管道去管理员权限
*/
BOOL PipeIPC::initSecurityAttributes()
{
	PSID pEveryoneSID = NULL;
	PACL pACL = NULL;
	EXPLICIT_ACCESS ea;
	PSECURITY_DESCRIPTOR pSD = NULL;
	void* ea_addr = NULL;
	void* acl_addr = NULL;
	void* sd_addr = NULL;
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;

	if (!AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pEveryoneSID))
	{
		return FALSE;
	}
	ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
	ea.grfAccessPermissions = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
	ea.grfAccessMode = SET_ACCESS;
	ea.grfInheritance = NO_INHERITANCE;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea.Trustee.ptstrName = (LPTSTR)pEveryoneSID;
	ea_addr = &ea;
	if (ERROR_SUCCESS != SetEntriesInAcl(1, &ea, NULL, &pACL))
	{
		goto cleanup;
	}
	acl_addr = &pACL;
	pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	if (NULL == pSD)
	{
		goto cleanup;
	}
	if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
	{
		goto cleanup;
	}
	sd_addr = &pSD;
	if (!SetSecurityDescriptorDacl(pSD, TRUE, pACL, FALSE))
	{
		goto cleanup;
	}
	m_sec_attr.nLength = sizeof(SECURITY_ATTRIBUTES);
	m_sec_attr.lpSecurityDescriptor = pSD;
	m_sec_attr.bInheritHandle = FALSE;
	return TRUE;

cleanup:
	if (pEveryoneSID)
		FreeSid(pEveryoneSID);
	if (pACL)
		LocalFree(pACL);
	if (pSD)
		LocalFree(pSD);
	return FALSE;
}

BOOL PipeIPC::Connect()
{
	return ConnectNamedPipe(m_hPipeHandle, nullptr);
}

void PipeIPC::DisConnect()
{
	DisconnectNamedPipe(m_hPipeHandle);
	CloseHandle(m_hPipeHandle);
	m_hPipeHandle = INVALID_HANDLE_VALUE;
}

void PipeIPC::release()
{
	if (m_hPipeHandle)
	{
		DisconnectNamedPipe(m_hPipeHandle);
		CloseHandle(m_hPipeHandle); // 关闭句柄
		m_hPipeHandle = NULL;
	}
}

