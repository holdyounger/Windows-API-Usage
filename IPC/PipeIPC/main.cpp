#include "PipeIPC.h"

#define PIPE_TAINS_SERVER_NAME       "\\\\.\\pipe\\TrustAgent\\pipe.ToTrustInstallerUI"
#define PIPE_TAINS_CLIENT_NAME       "\\\\.\\pipe\\TrustAgent\\pipe.ToTrustInstallerExe"

#define PIPE_SCAUP_SERVER_NAME       "\\\\.\\pipe\\TrustAgent\\pipe.ToTrustScatterUI"
#define PIPE_SCAUP_CLIENT_NAME       "\\\\.\\pipe\\TrustAgent\\pipe.ToTrustScatterExe"

__declspec(selectany) PipeIPC* g_pReadTAInst;
__declspec(selectany) PipeIPC* g_pWriteTAInst;
__declspec(selectany) PipeIPC* g_pReadSct;
__declspec(selectany) PipeIPC* g_pWriteSct;


void InitPipe();
bool StartPipeRead(PipeIPC* pipe);


int main(int argc, const char* argv[])
{

	InitPipe();

	g_pWriteTAInst->WriteData("Test shimingming 2023年9月8日15:21:04");

	return 0;
}



void InitPipe()
{
	auto InitReadPipe = [](PipeIPC* pipe, const std::string& name) {
		if (NULL == pipe)
		{
			pipe = new PipeIPC(PipeIPC::SERVER, name);
			pipe->InitPipeIPC();
		}

		static std::thread thdReadPipe = std::thread(StartPipeRead, pipe);
		if (thdReadPipe.joinable())
		{
			thdReadPipe.detach();
		}
	};

	// InitReadPipe(g_pReadTAInst, PIPE_TAINS_SERVER_NAME);
	// InitReadPipe(g_pReadSct, PIPE_SCAUP_SERVER_NAME);


	if (NULL == g_pWriteTAInst)
	{
		g_pWriteTAInst = new PipeIPC(PipeIPC::CLIENT, PIPE_SCAUP_SERVER_NAME);
		g_pWriteTAInst->InitPipeIPC();
	}
// 	if (NULL == g_pWriteSct)
// 	{
// 		g_pWriteSct = new PipeIPC(PipeIPC::CLIENT, PIPE_SCAUP_CLIENT_NAME);
// 		g_pWriteSct->InitPipeIPC();
// 	}
}

bool StartPipeRead(PipeIPC* pipe)
{
	bool isPipeEnd = true;

	while (true)
	{
		if (isPipeEnd)
		{
			pipe->Connect();
		}

		std::string datas;
		if (!pipe->ReadData(datas, isPipeEnd))
		{
			if (isPipeEnd)
			{
				pipe->DisConnect();
			}
		}

		if (!datas.empty())
		{
			// 处理数据

			std::cout << datas;
		}
	}
	return isPipeEnd;
}