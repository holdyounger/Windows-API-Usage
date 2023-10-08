#include <iostream>
#include <Windows.h>
#include <thread>

using namespace std;

const char* pipeNamePtr = "\\\\.\\pipe\\test_pip1";
const int pipeBufferSize = 65535;


HANDLE namedCreateNamedPipe = CreateFileA(pipeNamePtr,
	GENERIC_READ | GENERIC_WRITE,
	0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);


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

		cout << "server >" << buffer << endl;
	}

}

int main(int argc, char* argv[])
{

	if (namedCreateNamedPipe == INVALID_HANDLE_VALUE) {

		cout << "error : " << namedCreateNamedPipe;
		return 0;
	}

	cout << "namedCreateNamedPipe : " << namedCreateNamedPipe << endl;

#if 0
	thread thpipe(StartThreadRead);
	thpipe.detach();
#endif

	while (true) {

		DWORD writeNum;
		char buf[pipeBufferSize];
		cout << "send > :";
		cin >> buf;
		if (!WriteFile(namedCreateNamedPipe, buf, pipeBufferSize, &writeNum, nullptr)) {

			cout << "write failed";
			break;
		}
	}

	CloseHandle(namedCreateNamedPipe);
}