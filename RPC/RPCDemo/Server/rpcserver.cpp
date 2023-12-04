#include <iostream>
using namespace std;

#include "../RpcComplier/hello_h.h"
#include "DebugTrace.h"
#include <sddl.h>
#pragma comment(lib,"Rpcrt4.lib")


int g_nType = 0;
RPC_BINDING_HANDLE g_RpcHandle = NULL;


int Start_Ncacn_np()
{
	RPC_STATUS status = 0;

	unsigned int mincall = 1;
	unsigned int maxcall = 20;

	status = RpcServerUseProtseqEp(
		(unsigned char*)"ncacn_np",
		maxcall,
		(unsigned char*)"\\pipe\\hello",
		NULL);
	if (status != 0) {
		cout << "RpcServerUseProtseqEp returns: " << status << endl;
		return -1;
	}

	status = RpcServerRegisterIf(
		hello_v1_0_s_ifspec,
		NULL,
		NULL);
	if (status != 0) {
		cout << "RpcServerRegisterIf returns: " << status << endl;
		return -1;
	}

	cout << "Rpc Server Begin Listening..." << endl;
	status = RpcServerListen(mincall, maxcall, FALSE);
	if (status != 0) {
		cout << "RpcServerListen returns: " << status << endl;
		return -1;
	}

	cin.get();
}

int Start_NcalRpc()
{
	RPC_STATUS status = 0;
	unsigned int mincall = 1;
	unsigned int maxcall = 20;

	RpcServerUseProtseqEp(
		(unsigned char*)"ncalrpc",
		RPC_C_PROTSEQ_MAX_REQS_DEFAULT,
		(unsigned char*)"AppName",
		NULL);

	if (status != 0) {
		cout << "RpcServerUseProtseqEp returns: " << status << endl;
		return -1;
	}

	status = RpcServerRegisterIf(
		hello_v1_0_s_ifspec,
		NULL,
		NULL);
	if (status != 0) {
		cout << "RpcServerRegisterIf returns: " << status << endl;
		return -1;
	}

	cout << "Rpc Server Begin Listening..." << endl;
	status = RpcServerListen(mincall, maxcall, FALSE);
	if (status != 0) {
		cout << "RpcServerListen returns: " << status << endl;
		return -1;
	}

	cin.get();
}

int main(void)
{
	cout << "1: ncacn_np; 2. ncalrpc\nSelect:";
	cin >> g_nType;

	switch (g_nType)
	{
	case 1:
	{
		Start_Ncacn_np();
		break;
	}
	case 2:
	{
		Start_NcalRpc();
		break;
	}
	default:
		break;
	}
	
	return 0;
}

std::wstring GetClientSIDString()
{
	std::wstring clientSID = L"S-1-0-0";

	RPC_STATUS status = RpcImpersonateClient(nullptr);
	if (status != RPC_S_OK)
	{
		Debug_Error(_T("RpcImpersonateClient failed during GetClientSIDString"), status);
		return clientSID;
	}

	HANDLE hToken = nullptr;
	if (!OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, true, &hToken))
	{
		Debug_Error(_T("OpenThreadToken failed during GetClientSIDString"));
	}
	else
	{
		DWORD dwSize = 0;
		if (!GetTokenInformation(hToken, TokenUser, nullptr, 0, &dwSize) && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			Debug_Error(_T("failed to get token information size"));
		}
		else
		{
			PTOKEN_USER tokenUser = (PTOKEN_USER)LocalAlloc(LPTR, dwSize);
			if (tokenUser == nullptr)
			{
				Debug_Error(_T("failed to allocate token information buffer"));
			}
			else
			{
				if (!GetTokenInformation(hToken, TokenUser, tokenUser, dwSize, &dwSize))
				{
					Debug_Error(_T("failed to get token information"));
				}
				else
				{
					LPWSTR sidString = nullptr;
					if (!ConvertSidToStringSidW(tokenUser->User.Sid, &sidString))
					{
						Debug_Error(_T("failed to convert sid to string"));
					}
					clientSID.assign(sidString);
				}

				LocalFree(tokenUser);
			}
		}

		CloseHandle(hToken);
	}

	RevertToSelf();

	return clientSID;
}

/************************************************************************/
/*                        MIDL malloc & free                            */
/************************************************************************/

void* __RPC_USER MIDL_user_allocate(size_t len)
{
	return (malloc(len));
}

void __RPC_USER MIDL_user_free(void* ptr)
{
	free(ptr);
}

/************************************************************************/
/*                       Interfaces                                     */
/************************************************************************/

void HelloProc(unsigned char* szhello)
{

	unsigned int Pid;
	if (!g_RpcHandle)
	{
		RPC_STATUS rpcs = RpcServerInqBindingHandle(&g_RpcHandle);

		if (rpcs != RPC_S_OK)
		{
			cout << "Get Handle Failed" << endl;
			exit(-1);
		}
		
		cout << "Get handle:" << hex << &g_RpcHandle << endl;
	}

	if (1)
	{
		if (g_nType == 2)
		{
			RPC_STATUS rpcs = RpcServerInqBindingHandle(&g_RpcHandle);

			if (rpcs != RPC_S_OK)
			{
				cout << "Get Handle Failed" << endl;
				exit(-1);
			}

			cout << "Get handle:" << hex << g_RpcHandle << endl;
			unsigned int Pid;
			RPC_STATUS rpcStas = I_RpcBindingInqLocalClientPID(g_RpcHandle, (unsigned long*)&Pid);
			if (rpcs != RPC_S_OK)
			{
				cout << "Get Handle Failed" << endl;
				exit(-1);
			}

			cout << "Get Pid:" << dec << Pid << endl;
		}

		if (0 == strcmp((const char*)szhello,"sid"))
		{
			std::wstring sid = GetClientSIDString();
			wcout << L"sid:" << sid << endl;
		}
	}

	if(0)
	{
		RPC_CALL_ATTRIBUTES CallAttributes;  // this maps to RPC_CALL_ATTRIBUTES_V1

		memset(&CallAttributes, 0, sizeof(CallAttributes));
		CallAttributes.Version = RPC_CALL_ATTRIBUTES_VERSION;    // maps to 1
		CallAttributes.Flags = RPC_QUERY_CLIENT_PRINCIPAL_NAME | RPC_QUERY_CALL_LOCAL_ADDRESS;//....
		RPC_STATUS Status = RpcServerInqCallAttributes(0, &CallAttributes);

		if (Status != RPC_S_OK)
		{
			cout << "Get Pid Failed" << endl;
			exit(-1);
		}

		cout << "Pid:" << 1 << "\tMsg:" << szhello << endl;
	}
}

void Shutdown(void)
{
	RPC_STATUS status = 0;

	status = RpcMgmtStopServerListening(NULL);
	if (status != 0) {
		cout << "RpcMgmtStopServerListening returns: " << status << "!" << endl;
	}

	status = RpcServerUnregisterIf(NULL, NULL, FALSE);
	if (status != 0) {
		cout << "RpcServerUnregisterIf returns: " << status << "!" << endl;
	}
}