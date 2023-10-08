#include <iostream>
#include <Windows.h>
#include <thread>
using namespace std;

// const char* pipeNamePtr = "\\\\.\\pipe\\test_pip1";
const char* pipeNamePtr = "\\\\.\\pipe\\TrustAgent\\v10_plugin_s_pipe";
const int pipeBufferSize = 65535;

HANDLE namedCreateNamedPipe = CreateNamedPipeA(pipeNamePtr,
	PIPE_ACCESS_DUPLEX,
	PIPE_TYPE_MESSAGE /*| PIPE_WAIT*/ | PIPE_READMODE_MESSAGE,
	PIPE_UNLIMITED_INSTANCES,
	0,
	0, NMPWAIT_WAIT_FOREVER, nullptr);


void StartThreadRead()
{
	while (true)
	{
		char buffer[pipeBufferSize];
		DWORD lenth = 0;
		if (!ReadFile(namedCreateNamedPipe, buffer, pipeBufferSize, &lenth, nullptr)) {

			cout << "read failed : " << GetLastError() << endl;
			break;
		}

		cout << "Client >" << buffer << endl;


	}

}


int main(int argc, char* argv[])
{
	if (namedCreateNamedPipe == INVALID_HANDLE_VALUE) {

		cout << "error : " << namedCreateNamedPipe << endl;
		return 0;
	}

	cout << "namedCreateNamedPipe : " << namedCreateNamedPipe << endl;

	if (!ConnectNamedPipe(namedCreateNamedPipe, nullptr)) {

		cout << "ConnectNamedPipe failed";
		return 0;
	}
	cout << "connected";


	thread thpipe(StartThreadRead);
	thpipe.join();

#if 0
	while (true) {

		DWORD writeNum;
		std::string over = "";
		cout << "Server > :" << endl;
		cin >> over;
		if (!WriteFile(namedCreateNamedPipe, over.c_str(), over.length(), &writeNum, nullptr)) {

			cout << "WriteFile failed : " << GetLastError();
			break;
		}

	}
#endif

	DisconnectNamedPipe(namedCreateNamedPipe);
	CloseHandle(namedCreateNamedPipe);

	return 0;
}