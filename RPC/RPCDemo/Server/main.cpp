#include <iostream>
using namespace std;

#include "hello_h.h"

#pragma comment(lib,"Rpcrt4.lib")

int main(void)
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
	return 0;
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
	cout << szhello << endl;
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